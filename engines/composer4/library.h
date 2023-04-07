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

#include "common/array.h"
#include "common/file.h"

namespace Composer4 {

enum class ResourceType : uint32;

class Library {
public:
	Library(int id);
	~Library();

	Library(Library &) = delete;
	Library(Library &&) = delete;

	Library &operator=(Library &) = delete;
	Library &operator=(Library &&) = delete;

	bool open(const Common::Path &filename);

	void enable(bool enable) { _enabled = enable; }
	bool isEnabled() const { return _enabled; }

	int getID() const { return _id; }

	Common::SeekableReadStream *openResource(int id, ResourceType type);

private:
	bool readArchive(uint32 offset, bool root);

private:
	struct ResourceEntry {
		uint16 id;
		uint32 offset;
		uint32 size;
	};

	int _id;
	bool _enabled;
	Common::Array<Common::Pair<ResourceType, Common::Array<ResourceEntry>>> _resources;
	Common::File _file;
};

} // End of namespace Composer4

#endif // COMPOSER4_LIBRARY_H
