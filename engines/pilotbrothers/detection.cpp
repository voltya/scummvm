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

static const PlainGameDescriptor pilotBrothersGames[] = {
	{"pilotbrothers1", "Pilot Brothers 1"},
	{"pilotbrothers2", "Pilot Brothers 2"},
	{0, 0}
};

static const ADGameDescription gameDescriptions[] = {

	// Братья Пилоты: По следам полосатого слона
	{
		"pilotbrothers1",
		0,
		AD_ENTRY1s("PILOTS.EXE", "152f751d3c1b325e91411dd75de54e95", 48357155),
		Common::RU_RUS,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	// Братья Пилоты: Дело о серийном маньяке
	{
		"pilotbrothers2",
		0,
		AD_ENTRY1s("PILOTS2.EXE", "a0353dfb46043d1b2d1ef8ab6c204b33", 582283983),
		Common::RU_RUS,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	AD_TABLE_END_MARKER
};

} // End of namespace PilotBrothers

class PilotBrothersMetaEngine : public AdvancedMetaEngine {
public:
	PilotBrothersMetaEngine() : AdvancedMetaEngine(PilotBrothers::gameDescriptions, sizeof(ADGameDescription), PilotBrothers::pilotBrothersGames) {
		_gameIds = PilotBrothers::pilotBrothersGames;
		_maxScanDepth = 2;
	}

	const char *getName() const override {
		return "Pilot Brothers Engine";
	}

	const char *getOriginalCopyright() const override {
		return "Pilot Brothers Engine (C) Gamos";
	}

	bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
};


bool PilotBrothersMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	if (desc)
		*engine = new PilotBrothers::PilotBrothersEngine(syst, desc);

 	return desc != nullptr;
}

#if PLUGIN_ENABLED_DYNAMIC(PILOTBROTHERS)
REGISTER_PLUGIN_DYNAMIC(PILOTBROTHERS, PLUGIN_TYPE_ENGINE, PinkMetaEngine);
#else
REGISTER_PLUGIN_STATIC(PILOTBROTHERS, PLUGIN_TYPE_ENGINE, PilotBrothersMetaEngine);
#endif
