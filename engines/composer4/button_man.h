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

#ifndef COMPOSER4_BUTTON_MAN_H
#define COMPOSER4_BUTTON_MAN_H

#include "composer4/module.h"

namespace Common {
struct Point;
}

namespace Composer4 {

struct Button;

class ButtonManager : public Module {
public:
	ButtonManager();
	~ButtonManager() override;

	bool callFunction(FunctionOpcode opcode, Common::Array<Variable> &arguments, Variable &ret_value) override;

private:
	void addButton(uint16 id);
	void removeButton(uint16 id);

	const Button *findButton(Common::Point point, uint16 sprite_id) const;

	void activateButton(uint16 id, bool active);

private:
	Common::SortedArray<Button *, const Button *> _buttons;
};

} // End of namespace Composer4

#endif // COMPOSER4_BUTTON_MAN_H
