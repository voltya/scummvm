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

#include "composer4/composer4.h"

#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "composer4/console.h"
#include "composer4/defs.h"
#include "composer4/detection.h"
#include "composer4/library.h"
#include "engines/util.h"
#include "graphics/palette.h"

namespace Composer4 {

Composer4Engine *g_engine;

Composer4Engine::Composer4Engine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
																					 _gameDescription(gameDesc), _randomSource("Composer4") {
	g_engine = this;
}

Composer4Engine::~Composer4Engine() {
	delete _screen;
}

uint32 Composer4Engine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String Composer4Engine::getGameId() const {
	return _gameDescription->gameId;
}

Common::Error Composer4Engine::run() {
	// Initialize 320x200 paletted graphics mode
	initGraphics(320, 200);
	_screen = new Graphics::Screen();

	// Set the engine's debugger console
	setDebugger(new Console());

	if (!_bookIni.loadFromFile("BOOK.INI"))
		return Common::kNoGameDataFoundError;

	Common::String windowTitle;
	if (_bookIni.getKey("WindowTitle", "Common", windowTitle)) {
		_system->setWindowCaption(windowTitle);
	}

	uint fps = 8;
	Common::String string_fps;
	if (_bookIni.getKey("FPS", "Common", string_fps)) {
		fps = strtol(string_fps.c_str(), nullptr, 10);
		if (!fps) {
			fps = 8;
		}
	}

	return Common::kNoError;
}

Common::Error Composer4Engine::syncGame(Common::Serializer &s) {
	// The Serializer has methods isLoading() and isSaving()
	// if you need to specific steps; for example setting
	// an array size after reading it's length, whereas
	// for saving it would write the existing array's length
	int dummy = 0;
	s.syncAsUint32LE(dummy);

	return Common::kNoError;
}

Variable Composer4Engine::callFunction(FunctionOpcode opcode, Common::Array<Variable> &vars) {
	switch (opcode) {
	case FunctionOpcode::kEnableLibrary:
		if (Library *library = findLibrary(vars[0].i32))
			library->enable(vars[1].u8);
		return 0;
	case FunctionOpcode::kLoadLibrary:
		if (findLibrary(vars[0].u16)) {
			return 0;
		}

		if (vars[1].i32 <= 4) { // priority
			return loadLibrary(vars[0].u16);
		}

		_libraryLoadTasks.push_back({vars[0].i32, true});
		return 1;
	case FunctionOpcode::kFreeLibrary:
		_libraryLoadTasks.push_back({vars[0].i32, false});
		return 0;
	}

	return {};
}

Common::Path Composer4Engine::getFileName(int id, const Common::String &section) const {
	Common::String stringId = Common::String::format("%d", id);
	Common::String fileName;
	if (_bookIni.getKey(stringId, section, fileName)) {

		if (!fileName.empty() && fileName[0] == '~') {
			// after second slash is our path
			fileName.replace(0, fileName.findFirstOf('\\', 3) + 1, "");
		}

		return {fileName, '\\'};
	}

	return {};
}

Common::SeekableReadStream *Composer4Engine::openResource(int id, ResourceType type) {
	for (auto *library : _libraries) {
		auto *stream = library->openResource(id, type);
		if (stream)
			return stream;
	}
	return nullptr;
}

bool Composer4Engine::loadLibrary(int id) {
	if (!id) {
		Common::String startupId;
		if (_bookIni.getKey("Startup", "Common", startupId)) {
			id = strtol(startupId.c_str(), nullptr, 10);
		}
	}

	if (findLibrary(id)) {
		return false;
	}

	auto *library = new Library(id);
	_libraries.insert_at(0, library);

	if (_libraries.front()->open(getFileName(id, "Libs"))) {
		return true;
	}

	delete _libraries.front();
	_libraries.remove_at(0);

	return false;
}

Library *Composer4Engine::findLibrary(int id) {
	for (auto *library : _libraries) {
		if (library->getID() == id) {
			return library;
		}
	}
	return nullptr;
}

void Composer4Engine::freeLibrary(int id) {
	auto remove_library = [this](int id) {
		for (uint i = 0; i < _libraries.size(); ++i) {
			if (_libraries[i]->getID() == id) {
				delete _libraries[i];
				_libraries.remove_at(i);
				return true;
			}
		}
		return false;
	};

	if (remove_library(id)) {
		// clearTimers();
		callFunction(FunctionOpcode::kxLibraryFree, id);
	}
}

} // End of namespace Composer4
