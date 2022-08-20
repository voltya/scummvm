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

#ifndef COMPOSER4_DETECTION_TABLES_H
#define COMPOSER4_DETECTION_TABLES_H

namespace Composer4 {

const PlainGameDescriptor GameNames[] = {
	{"jspreschool99", "JumpStart Preschool 1999 Edition"},
	{0, 0}};

const ADGameDescription GameDescriptions[] = {
	{"jspreschool99",
	 nullptr,
	 AD_ENTRY2s(
		 "PRSCHL99.EXE", "9202e1b99191bf90091e44ba1027fcfe", 536576,
		 "BOOK.INI", "9d5afb6e5fef2c68ee25addf12778973", 1443),
	 Common::EN_ANY,
	 Common::kPlatformWindows,
	 ADGF_UNSTABLE | ADGF_DROPPLATFORM,
	 GUIO1(GUIO_NONE)},

	AD_TABLE_END_MARKER};

} // End of namespace Composer4

#endif
