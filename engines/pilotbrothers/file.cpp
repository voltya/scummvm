/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "pilotbrothers/pilotbrothers.h"

namespace PilotBrothers {

Common::Error PilotBrothersEngine::initFile() {
	if (!_file.open(_desc.filesDescriptions[0].fileName))
		return Common::kNoGameDataFoundError;

	_file.seek(-12, SEEK_END);
	_file.readUint32LE(); // size of unk struct (11)
	if (_file.readUint32BE() != 4 || _file.readUint32BE() != MKTAG('=', 'V', 'S', '=')) // Vadim Sytnikov
		return Common::kNoGameDataFoundError;

	_file.seek(-23, SEEK_END);

	_countOfByteCodeFiles = _file.readUint32LE();
	_offsetToByteCodeFiles = _file.readUint32LE();
	_offsetToTableOfOffsetsOfByteCodeFiles = 23 + 5 * _countOfByteCodeFiles + 5;

	return Common::kNoError;
}

} // End of namespace PilotBrothers

