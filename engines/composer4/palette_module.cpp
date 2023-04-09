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

#include "composer4/palette_module.h"
#include "composer4/composer4.h"

namespace Composer4 {

bool PaletteModule::callFunction(FunctionOpcode opcode, Common::Array<Variable> &arguments, Variable &ret_value) {
	switch (opcode) {
	case FunctionOpcode::kSetPalette:
		ret_value = setPalette(arguments[0].u16, arguments[1].u8);
		return true;
	case FunctionOpcode::kSetPaletteFragment:
		setPaletteFragment(static_cast<byte *>(arguments[0].pointer), arguments[1].u32, arguments[2].u32, arguments[3].u8);
		return true;
	case FunctionOpcode::kGetPaletteFragment:
		getPaletteFragment(static_cast<byte *>(arguments[0].pointer), arguments[1].u32, arguments[2].u32);
		return true;
	case FunctionOpcode::kGetResourcePaletteFragment:
		getResourcePaletteFragment(arguments[0].u16, static_cast<byte *>(arguments[1].pointer), arguments[2].u32, arguments[3].u32);
		return true;
	default:
		return false;
	}
}

int PaletteModule::setPalette(uint16 id, byte brightness) {
	if (id) {
		Common::ScopedPtr<Common::SeekableReadStream> stream{g_engine->openResource(id, ResourceType::kPalette)};
		if (stream) {
			uint16 count = MIN<uint16>(PALETTE_COUNT, stream->readUint16LE());

			byte palette[PALETTE_SIZE];
			stream->read(palette, count * 3);

			setPaletteFragment(palette, 0, count, brightness);
			return 1;
		}
		return 0;
	}

	// alter brightness
	setPaletteFragment(nullptr, 0, PALETTE_COUNT, brightness);
	return 1;
}

void PaletteModule::setPaletteFragment(byte *src, uint index, uint count, byte brightness) {
	if (index >= PALETTE_COUNT)
		return;

	count = MIN(count, PALETTE_COUNT - index);
	brightness = MIN<byte>(brightness, 100);

	byte palette[PALETTE_SIZE];
	g_engine->_screen->getPalette(palette);

	if (src) {
		memcpy(palette + index * 3, src, count * 3);
	}

	for (uint i = index * 3; i < count * 3; ++i) {
		palette[i] = palette[i] * brightness / 100;
	}

	palette[PALETTE_SIZE - 1] = 0xff;
	palette[PALETTE_SIZE - 2] = 0xff;
	palette[PALETTE_SIZE - 3] = 0xff;
	palette[0] = 0;
	palette[1] = 0;
	palette[2] = 0;

	g_engine->_screen->setPalette(palette);
}

void PaletteModule::getPaletteFragment(byte *dst, uint index, uint count) {
	if (index < PALETTE_COUNT) {
		count = MIN(count, PALETTE_COUNT - index);
		g_engine->_screen->getPalette(dst, index, count);
	}
}

void PaletteModule::getResourcePaletteFragment(uint16 id, byte *dst, uint index, uint count) {
	Common::ScopedPtr<Common::SeekableReadStream> stream{g_engine->openResource(id, ResourceType::kPalette)};
	if (!stream)
		return;

	const uint16 entriesCount = stream->readUint16LE();

	index = MIN<uint>(entriesCount - 1, index);
	count = MIN(count, entriesCount - index);

	stream->skip(3 * index);
	stream->read(dst, count * 3);
}

} // End of namespace Composer4
