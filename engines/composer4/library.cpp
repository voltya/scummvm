/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/memstream.h"

#include "composer4/composer4.h"
#include "composer4/library.h"

namespace Composer4 {

Library::Library(uint16 id)
	: _id(id), _enabled(true) {}

Library::~Library() {
	for (const auto &archive : _archives) {
		onArchiveClosing(archive);
	}
}

bool Library::open(const Common::String &fileName) {
	if (!_file.open(fileName))
		return false;

	const uint32 signature = _file.readUint32BE();
	if (signature != MKTAG('L', 'B', 'R', 'C'))
		return false;

	/* const uint32 table_size = */ _file.readUint32LE();     // with 32 bytes header
	/* const uint32 resources_size = */ _file.readUint32LE(); // without sub-archives

	const uint32 version = _file.readUint32LE();

	return (version >= MKTAG16(2, 1)) && readArchive(_id, _file.pos());
}

Common::SeekableReadStream *Library::loadResource(uint16 id, ResourceType type, bool segmented) {
	if (segmented) {
		for (const auto &archive : _archives) {
			uint32 resource_size = 0;
			for (const auto &resource : archive.resources) {
				if (resource.id == id && resource.type == type) {
					resource_size += resource.size;
				}
			}

			if (!resource_size)
				continue;

			byte *data = static_cast<byte *>(malloc(resource_size));
			byte *data_ptr = data;
			for (const auto &resource : archive.resources) {
				if (resource.id == id && resource.type == type) {
					const bool result = _file.seek(resource.offset) && resource_size == _file.read(data_ptr, resource.size);
					if (!result) {
						free(data);
						return nullptr;
					}
					data_ptr += resource.size;
				}
			}

			return new Common::MemoryReadStream(data, resource_size, DisposeAfterUse::YES);
		}

		return nullptr;
	}

	for (const auto &archive : _archives) {
		const auto *resDesc = archive.findResourceDescription(id, type);
		if (resDesc) {
			if (resDesc->isArchive) {
				if (readArchive(resDesc->id, resDesc->offset)) {
					resDesc = _archives.back().findResourceDescription(id, type);
				} else {
					return nullptr;
				}
			}

			if (!_file.seek(resDesc->offset))
				return nullptr;

			return _file.readStream(resDesc->size);
		}
	}

	return nullptr;
}

void Library::releaseResource(uint16 id, ResourceType type) {
	if (_archives.empty())
		return;

	const auto *resDesc = _archives.front().findResourceDescription(id, type);
	if (!resDesc || !resDesc->isArchive)
		return;

	for (auto it = _archives.begin(); it != _archives.end(); ++it) {
		if (it->id == resDesc->id && it->type == resDesc->type) {
			onArchiveClosing(*it);
			_archives.erase(it);
			break;
		}
	}
}

bool Library::readArchive(uint16 id, uint offset) {
	if (!_file.seek(offset))
		return false;

	Archive archive;
	archive.id = id;

	const uint types_count = _file.readUint32LE();
	/* const uint table_size = */ _file.readUint32LE(); // root has extra 16 bytes

	if (!_file.skip(8)) // unknown 8 bytes
		return false;

	const uint table_offset = _file.pos();

	for (uint i = 0; i < types_count; ++i) {
		const uint type = _file.readUint32BE();
		const uint type_offset = _file.readUint32LE();

		if (!type_offset)
			continue;

		const auto pos_to_next_type = _file.pos();

		if (!_file.seek(table_offset + type_offset))
			return false;

		ResourceDescription resource{};

		resource.type = static_cast<ResourceType>(type);

		while (true) {
			resource.offset = _file.readUint32LE();
			if (!resource.offset)
				break;

			resource.size = _file.readUint32LE();
			_file.readUint32LE(); // ptr
			resource.id = _file.readUint16LE();
			resource.isArchive = (_file.readUint16LE() & 0x2);
			_file.readUint32LE(); // ref_count

			if (!resource.isArchive) {
				resource.offset += offset;
			}

			archive.resources.push_back(resource);
		}

		if (!_file.seek(pos_to_next_type))
			return false;
	}

	_archives.push_back(archive);

	if (_archives.size() == 1) {
		for (const auto &resource : _archives[0].resources) {
			g_engine->onResourceLoad(resource.id, resource.type); // kXResourceLoad
		}

		Common::Array<Variable> arguments = {archive.id};
		g_engine->callFunction(kXLibraryLoad, arguments);
	}

	return true;
}

void Library::onArchiveClosing(const Archive &archive) {
	for (const auto &resource : archive.resources) {
		g_engine->onResourceFree(resource.id, resource.type); // kXResourceFree
	}
}

const Library::ResourceDescription *Library::Archive::findResourceDescription(uint16 resourceId, ResourceType resourceType) const {
	for (const auto &resource : resources) {
		if (resource.id == resourceId && resource.type == resourceType) {
			return &resource;
		}
	}
	return nullptr;
}

} // End of namespace Composer4
