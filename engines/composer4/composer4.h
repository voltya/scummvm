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

#ifndef COMPOSER4_H
#define COMPOSER4_H

#include "common/scummsys.h"
#include "common/system.h"
#include "common/error.h"
#include "common/fs.h"
#include "common/hash-str.h"
#include "common/random.h"
#include "common/serializer.h"
#include "common/util.h"
#include "engines/engine.h"
#include "engines/savestate.h"
#include "graphics/screen.h"

#include "composer4/detection.h"
#include "composer4/defs.h"

namespace Composer4 {

class TimersManager;
class Module;
class Library;

struct Composer4GameDescription;

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
		return
		    (f == kSupportsLoadingDuringRuntime) ||
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

	template<typename... Args>
	Variable callFunction(FunctionOpcode opcode, Args... args) {
		Common::Array<Variable> arguments = {args...};
		return callFunction(opcode, arguments);
	}

	Variable callFunction(FunctionOpcode opcode, Common::Array<Variable> &vars);


	Common::Path getFileName(int id, const Common::String &section) const;
	Common::SeekableReadStream *openResource(int id, ResourceType type);

	bool loadLibrary(int id);
	void freeLibrary(int id);
	Library *findLibrary(int id);

private:
	struct LibraryLoadTask {
		int id;
		bool load;
	};
	Common::Array<Library *> _libraries;
	Common::Array<LibraryLoadTask> _libraryLoadTasks;

	Common::INIFile _bookIni;

	TimersManager *_timersMan;
	Common::Array<Module *> _modules;
};

extern Composer4Engine *g_engine;
#define SHOULD_QUIT ::Composer4::g_engine->shouldQuit()

} // End of namespace Composer4

#endif // COMPOSER4_H
