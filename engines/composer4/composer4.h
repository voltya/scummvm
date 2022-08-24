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

#ifndef COMPOSER4_COMPOSER4_H
#define COMPOSER4_COMPOSER4_H

#include "common/error.h"
#include "common/fs.h"
#include "common/hash-str.h"
#include "common/random.h"
#include "common/scummsys.h"
#include "common/serializer.h"
#include "common/system.h"
#include "common/util.h"
#include "engines/engine.h"
#include "engines/savestate.h"
#include "graphics/screen.h"

#include "composer4/base.h"
#include "composer4/detection.h"

namespace Composer4 {

struct Composer4GameDescription;

class Library;
class ButtonsContainer;

// Based on 4.0.0.36
class Composer4Engine : public Engine {
private:
	const ADGameDescription *_gameDescription;
	Common::RandomSource _randomSource;

protected:
	// Engine APIs
	Common::Error run() override;

public:
	Graphics::Screen *_screen = nullptr;

public:
	Composer4Engine(OSystem *syst, const ADGameDescription *gameDesc);
	~Composer4Engine() override;

	uint32 getFeatures() const;

	/**
	 * Returns the game Id
	 */
	Common::String getGameId() const;

	/**
	 * Gets a random number
	 */
	uint32 getRandomNumber(uint maxNum) {
		return _randomSource.getRandomNumber(maxNum);
	}

	bool hasFeature(EngineFeature f) const override {
		return (f == kSupportsLoadingDuringRuntime) ||
			   (f == kSupportsSavingDuringRuntime) ||
			   (f == kSupportsReturnToLauncher);
	};

	bool canLoadGameStateCurrently() override {
		return true;
	}
	bool canSaveGameStateCurrently() override {
		return true;
	}

	/**
	 * Uses a serializer to allow implementing savegame
	 * loading and saving using a single method
	 */
	Common::Error syncGame(Common::Serializer &s);

	Common::Error saveGameStream(Common::WriteStream *stream, bool isAutosave = false) override {
		Common::Serializer s(nullptr, stream);
		return syncGame(s);
	}
	Common::Error loadGameStream(Common::SeekableReadStream *stream) override {
		Common::Serializer s(stream, nullptr);
		return syncGame(s);
	}

	FunctionResult callFunction(uint16 opcode, Common::Array<Variable> &arguments);
	FunctionResult runEvent(uint16 id, Common::Array<Variable> &arguments);
	FunctionResult runScript(uint16 id, Common::Array<Variable> &arguments);

	Common::SeekableReadStream *loadResource(uint16 id, ResourceType type, bool segmented = false);

	void onResourceLoad(uint16 id, ResourceType type);
	void onResourceFree(uint16 id, ResourceType type);

	int32 findLibraryIndex(uint16 id) const;
	uint16_t loadLibrary(uint16 id);
	void freeLibrary(uint16 id);

	void startTimer(uint index, uint duration, uint16 id, uint count);
	void stopTimer(uint index);
	void suspendTimer(uint index, bool suspend);
	void clearDeadTimers();
	void updateTimers(uint time);

private:
	Common::Array<Library *> _libraries;
	Common::ScopedPtr<ButtonsContainer> _buttonsMan;

	struct Timer {
		uint32 baseTime = 0;
		uint32 duration = 0;
		uint32 count = 0;
		uint16 scriptId = 0;
		uint32 savedCount = 0;
	};
	Timer _timers[16];

	struct LibraryLoadTask {
		uint16 id = 0;
		bool load = false;
	};
	Common::Array<LibraryLoadTask> _libraryLoadTasks;
	uint _callsDepth = 0;
};

extern Composer4Engine *g_engine;
#define SHOULD_QUIT ::Composer4::g_engine->shouldQuit()

} // namespace Composer4

#endif
