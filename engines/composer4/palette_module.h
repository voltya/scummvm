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

#ifndef COMPOSER4_PALETTE_MODULE_H
#define COMPOSER4_PALETTE_MODULE_H

#include "graphics/screen.h"

#include "composer4/module.h"

namespace Composer4 {

class PaletteModule final : public Module {
public:
	~PaletteModule() override = default;
	bool callFunction(FunctionOpcode opcode, Common::Array<Variable> &arguments, Variable &ret_value) override;

private:
	static int setPalette(uint16 id, byte brightness);
	static void setPaletteFragment(byte *src, uint index, uint count, byte brightness);
	static void getPaletteFragment(byte *dst, uint index, uint count);
	static void getResourcePaletteFragment(uint16 id, byte *dst, uint index, uint count);
};

} // End of namespace Composer4

#endif
