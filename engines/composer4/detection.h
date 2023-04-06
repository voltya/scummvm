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

#ifndef COMPOSER4_DETECTION_H
#define COMPOSER4_DETECTION_H

#include "engines/advancedDetector.h"

namespace Composer4 {

enum Composer4DebugChannels {
	kDebugGraphics = 1 << 0,
	kDebugPath     = 1 << 1,
	kDebugScan     = 1 << 2,
	kDebugFilePath = 1 << 3,
	kDebugScript   = 1 << 4,
};

extern const PlainGameDescriptor composer4Games[];

extern const ADGameDescription gameDescriptions[];

#define GAMEOPTION_ORIGINAL_SAVELOAD GUIO_GAMEOPTIONS1

} // End of namespace Composer4

class Composer4MetaEngineDetection : public AdvancedMetaEngineDetection {
	static const DebugChannelDef debugFlagList[];

public:
	Composer4MetaEngineDetection();
	~Composer4MetaEngineDetection() override {}

	const char *getName() const override {
		return "composer4";
	}

	const char *getEngineName() const override {
		return "Composer4";
	}

	const char *getOriginalCopyright() const override {
		return "Composer4 (C)";
	}

	const DebugChannelDef *getDebugChannels() const override {
		return debugFlagList;
	}
};

#endif // COMPOSER4_DETECTION_H
