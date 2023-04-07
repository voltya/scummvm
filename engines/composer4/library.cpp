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
#include "composer4/defs.h"
#include "composer4/library.h"

namespace Composer4 {

Library::Library(int id)
	: _id(id), _enabled(true) {}

Library::~Library() {
	for (const auto &type_resources_pair : _resources) {
		for (auto &resource : type_resources_pair.second) {
			g_engine->callFunction(FunctionOpcode::kxResourceRelease, resource.id, (uint32)type_resources_pair.first);
		}
	}
}

bool Library::open(const Common::Path &filename) {
	if (!_file.open(filename)) {
		return false;
	}

	const uint32 signature = _file.readUint32BE();
	if (signature != MKTAG('L', 'B', 'R', 'C'))
		return false;

	_file.skip(8);

	const uint32 version = _file.readUint32LE();

	return (version >= MKTAG16(2, 1)) && readArchive(_file.pos(), true);
}

Common::SeekableReadStream *Library::openResource(int id, ResourceType type) {
	if (!_enabled)
		return nullptr;

	for (const auto &type_resources_pair : _resources) {
		if (type_resources_pair.first == type) {
			continue;
		}

		if (type == ResourceType::kSound) {
			Common::ScopedPtr<Common::MemoryReadWriteStream> stream{new Common::MemoryReadWriteStream{DisposeAfterUse::YES}};

			for (auto &resource : type_resources_pair.second) {
				if (resource.id == id) {
					if (_file.seek(resource.offset)) {
						stream->writeStream(_file.readStream(resource.size));
					} else {
						return nullptr;
					}
				}
			}

			stream->seek(0, SEEK_SET);
			return stream.release();
		}

		for (auto &resource : type_resources_pair.second) {
			if (resource.id == id) {
				if (_file.seek(resource.offset)) {
					return _file.readStream(resource.size);
				}
				return nullptr;
			}
		}
	}

	return nullptr;
}

bool Library::readArchive(uint32 offset, bool root) {
	if (!_file.seek(offset))
		return false;

	const uint types_count = _file.readUint32LE();
	/* const uint table_size = */ _file.readUint32LE(); // root has extra 16 bytes

	if (!_file.skip(8)) // unknown 8 bytes
		return false;

	auto get_type_resources = [this](ResourceType type) -> Common::Array<ResourceEntry> & {
		for (auto &type_resources_pair : _resources) {
			if (type_resources_pair.first == type) {
				return type_resources_pair.second;
			}
		}
		_resources.push_back({type, {}});
		return _resources.back().second;
	};

	const uint table_offset = _file.pos();
	for (uint i = 0; i < types_count; ++i) {
		const auto type = static_cast<ResourceType>(_file.readUint32BE());
		const uint type_offset = _file.readUint32LE();

		if (!type_offset)
			continue;

		if (type == ResourceType::kNames ||
			type == ResourceType::kVariable ||
			type == ResourceType::kMouthSync) {
			continue;
		}

		const auto pos_to_next_type = _file.pos();

		if (!_file.seek(table_offset + type_offset))
			return false;

		ResourceEntry resource{};
		while (true) {
			auto &resources = get_type_resources(type);

			resource.offset = _file.readUint32LE();
			if (!resource.offset)
				break;

			resource.size = _file.readUint32LE();
			_file.skip(4); // ptr
			resource.id = _file.readUint16LE();
			const bool isArchive = (_file.readUint16LE() & 0x2);
			_file.skip(4); // ref_count

			if (!isArchive) {
				resource.offset = resource.offset + offset;
				resources.push_back(resource);
				if (root) {
					g_engine->callFunction(FunctionOpcode::kxResourceLoad, resource.id, (uint32)type);
				}
			} else {
				auto pos = _file.pos();
				readArchive(resource.offset, false);
				_file.seek(pos);
			}
		}

		if (!_file.seek(pos_to_next_type))
			return false;
	}

	if (root) {
		g_engine->callFunction(FunctionOpcode::kxLibraryLoad, _id);
	}

	return true;
}

} // End of namespace Composer4
