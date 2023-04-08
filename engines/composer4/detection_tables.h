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

namespace Composer4 {

const PlainGameDescriptor gameNames[] = {
	{"jspreschool99", "JumpStart Preschool 1999 Edition"},
	{"jsmath", "JumpStart Math for Kindergartners"},
	{"fisherprice", "Fisher-Price's Ready for School: Reading"},
	{0, 0}};

const ADGameDescription gameDescriptions[] = {
	{"jspreschool99",
	 nullptr,
	 AD_ENTRY1s("BOOK.INI", NULL, -1),
	 Common::EN_ANY,
	 Common::kPlatformWindows,
	 ADGF_UNSTABLE | ADGF_DROPPLATFORM,
	 GUIO1(GUIO_NONE)},

	{"jsmath",
	 nullptr,
	 AD_ENTRY1s("BOOK.INI", NULL, -1),
	 Common::EN_ANY,
	 Common::kPlatformWindows,
	 ADGF_UNSTABLE | ADGF_DROPPLATFORM,
	 GUIO1(GUIO_NONE)},

	{"fisherprice",
	 nullptr,
	 AD_ENTRY1s("BOOK.INI", NULL, -1),
	 Common::EN_ANY,
	 Common::kPlatformWindows,
	 ADGF_UNSTABLE | ADGF_DROPPLATFORM,
	 GUIO1(GUIO_NONE)},

	AD_TABLE_END_MARKER};

} // End of namespace Composer4
