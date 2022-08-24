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

#ifndef COMPOSER4_BUTTON_H
#define COMPOSER4_BUTTON_H

#include "common/array.h"
#include "common/rect.h"

namespace Common {
class SeekableReadStream;
} // End of namespace Common

namespace Composer4 {

enum class ButtonType : uint16;

struct Button {
	uint16 id;
	ButtonType type;
	uint16 z;
	uint16 scriptId;
	uint16 scriptIdIn;
	uint16 scriptIdOut;
	bool active;

	bool contains(Common::Point point) const;

	Common::Rect rect;
	Common::Array<uint16> spriteIds;
	// Common::Array<Common::Point> polygon;
};

class ButtonsContainer {
public:
	ButtonsContainer();

	void addButton(uint16 id, Common::SeekableReadStream &stream);
	void removeButton(uint16 id);

	void activateButton(uint16 id, bool active);

	const Button *findButton(Common::Point point, uint16 sprite_id) const;

private:
	Common::SortedArray<Button, const Button &> _buttons;
};

} // End of namespace Composer4

#endif
