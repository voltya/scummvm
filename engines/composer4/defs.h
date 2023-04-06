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

#ifndef COMPOSER4_DEFS_H
#define COMPOSER4_DEFS_H

#include "common/endian.h"

namespace Composer4 {

enum class ResourceType {
	kBitmap = MKTAG('B', 'M', 'A', 'P'),
	kSound = MKTAG('W', 'A', 'V', 'E'),
	kPalette = MKTAG('C', 'T', 'B', 'L'),
	kButton = MKTAG('B', 'U', 'T', 'N'),
	kAnimation = MKTAG('A', 'N', 'I', 'M'),
	kString = MKTAG('S', 'T', 'R', ' '),
	kEvent = MKTAG('E', 'V', 'N', 'T'),
	kScript = MKTAG('S', 'C', 'R', 'P'),
	kWalkMap = MKTAG('W', 'M', 'A', 'P'),
	kShadeTable = MKTAG('S', 'T', 'B', 'L'),
	kTable = MKTAG('D', 'T', 'A', 'B'),
	kVariable = MKTAG('V', 'A', 'R', 'I'),
	kMouthSync = MKTAG('A', 'A', 'A', '1'),
	kUniFont = MKTAG('U', 'F', 'I', '1'),
	kFont = MKTAG('F', 'O', 'N', 'T'),
};

} // End of namespace Composer4

#endif // COMPOSER4_DEFS_H
