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

#ifndef COMPOSER4_LIBRARY_H
#define COMPOSER4_LIBRARY_H

#include "common/file.h"

namespace Composer4 {

enum class ResourceType : uint32;

class Library {
public:
	explicit Library(uint16 id);
	~Library();

	bool open(const Common::String &fileName);

	Common::SeekableReadStream *loadResource(uint16 id, ResourceType type, bool segmented = false);
	void releaseResource(uint16 id, ResourceType type);

	uint16 getId() const { return _id; }

	bool isEnabled() const { return _enabled; }
	void enable(bool enable) { _enabled = enable; }

private:
	struct ResourceDescription {
		uint16 id;
		ResourceType type;
		uint32 offset;
		uint32 size;
		bool isArchive;
	};

	struct Archive {
		const ResourceDescription *findResourceDescription(uint16 id, ResourceType type) const;

		uint16 id = 0;
		ResourceType type = static_cast<ResourceType>(0);
		Common::Array<ResourceDescription> resources;
	};

	bool readArchive(uint16 id, uint offset);
	static void onArchiveClosing(const Archive &archive);

	uint16 _id;
	bool _enabled;
	Common::File _file;
	Common::Array<Archive> _archives;
};

} // End of namespace Composer4

#endif
