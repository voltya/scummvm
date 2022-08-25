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

#include "audio/mixer.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/palette.h"

#include "composer4/button.h"
#include "composer4/composer4.h"
#include "composer4/console.h"
#include "composer4/library.h"

enum SoundChannel {
	kVoiceChannel = 0,
	kSfx1Channel,
	kSfx2Channel,
	kMusicChannel,

	kChannelsCount = kMusicChannel + 1
};

static Audio::Mixer::SoundType mapChannelToSoundType(uint index) {
	switch (index) {
	case kVoiceChannel:
		return Audio::Mixer::kSpeechSoundType;
	case kSfx1Channel:
	case kSfx2Channel:
		return Audio::Mixer::kSFXSoundType;
	case kMusicChannel:
		return Audio::Mixer::kMusicSoundType;
	default:
		assert(0);
		return Audio::Mixer::kPlainSoundType;
	}
}

namespace Composer4 {

Composer4Engine *g_engine;

Composer4Engine::Composer4Engine(OSystem *syst, const ADGameDescription *gameDesc)
	: Engine(syst), _gameDescription(gameDesc), _randomSource("Composer4") {
	g_engine = this;
}

Composer4Engine::~Composer4Engine() {
	delete _screen;

	for (const auto *library : _libraries) {
		delete library;
	}
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

	_buttonsMan.reset(new ButtonsContainer());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	// Draw a series of boxes on screen as a sample
	for (int i = 0; i < 100; ++i)
		_screen->frameRect(Common::Rect(i, i, 320 - i, 200 - i), i);
	_screen->update();

	// Simple event handling loop
	byte pal[256 * 3] = {0};
	Common::Event e;
	int offset = 0;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}

		// Cycle through a simple palette
		++offset;
		for (int i = 0; i < 256; ++i)
			pal[i * 3 + 1] = (i + offset) % 256;
		g_system->getPaletteManager()->setPalette(pal, 0, 256);
		_screen->update();

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		g_system->delayMillis(10);
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

FunctionResult Composer4Engine::callFunction(uint16 opcode, Common::Array<Variable> &arguments) {
	FunctionResult result;

	auto eventResult = runEvent(opcode, arguments);
	if (eventResult)
		return eventResult;

	switch (opcode) {
	case kActivateButton:
	case kDeactivateButton:
		_buttonsMan->activateButton(arguments[0].u16, opcode == kActivateButton);
		// update cursor
		return FunctionResult(1);

	case kStartTimer:
		startTimer(arguments[0].u32, arguments[1].u32, arguments[2].u16, UINT_MAX);
		return FunctionResult(0);
	case kStopTimer:
		stopTimer(arguments[0].u32);
		return FunctionResult(0);
	case kSingleEvent:
		startTimer(arguments[0].u32, arguments[1].u32, arguments[2].u16, 1);
		return FunctionResult(0);
	case kSuspendTimer:
		suspendTimer(arguments[0].u32, arguments[1].u8);
		return FunctionResult(0);

	case kSetSoundVolume: {
		int volume = (arguments[1].u32 + 5000) * Audio::Mixer::kMaxMixerVolume / 5000;
		_mixer->setVolumeForSoundType(mapChannelToSoundType(arguments[0].u32), volume);
		return FunctionResult(0);
	}

/*
	case kReloadLibrary:
		_libraryLoadTasks.push_back({arguments[0].u16, true});
		break;
*/
	case kLoadLibrary: {
		if (findLibraryIndex(arguments[0].u16) != -1) {
			return FunctionResult(0);
		}

		if (arguments[1].u32 <= 4) { // priority
			return FunctionResult(loadLibrary(arguments[0].u16));
		}

		return FunctionResult(1);
	}
	case kFreeLibrary:
		_libraryLoadTasks.push_back({arguments[0].u16, false});
		break;
	case kEnableLibrary: {
		auto index = findLibraryIndex(arguments[0].u16);
		if (index != -1) {
			_libraries[index]->enable(arguments[1].u8);
		}
		return FunctionResult(0);
	}

	case kSetPalette:
		return FunctionResult(setPalette(arguments[0].u16, arguments[1].u8));
	case kSetPaletteFragment:
		setPaletteFragment(static_cast<byte *>(arguments[0].pointer), arguments[1].u32, arguments[2].u32, arguments[3].u8);
		return FunctionResult(0);
	case kGetPaletteFragment:
		getPaletteFragment(static_cast<byte *>(arguments[0].pointer), arguments[1].u32, arguments[2].u32);
		return FunctionResult(0);
	case kGetResourcePaletteFragment:
		getResourcePaletteFragment(arguments[0].u16, static_cast<byte *>(arguments[1].pointer), arguments[2].u32, arguments[3].u32);
		return FunctionResult(0);

	default:
		error("function %d is not supported yet", opcode);
		break;
	}

	return result;
}

FunctionResult Composer4Engine::runEvent(uint16 id, Common::Array<Variable> &arguments) {
	constexpr uint kPointer = 0x40;
	// constexpr uint kID = 0x80;

	Common::ScopedPtr<Common::SeekableReadStream> stream{loadResource(id, ResourceType::kEvent)};

	if (!stream || !stream->skip(16))
		return FunctionResult{};

	const uint args_count = stream->readUint32LE();
	assert(arguments.size() == args_count);

	// function signature is stored here like (int *, int, string)
	for (auto &argument : arguments) {
		const uint argument_type = stream->readUint32LE();
		if (argument_type & kPointer) {
			// argument.u32 = static_cast<byte *>(argument.pointer) - static_cast<byte *>(_scriptEngine->getRawMemory(0));
		}
	}

	uint script_id = 0;
	const uint handlers_count = stream->readUint32LE();
	if (!handlers_count)
		return FunctionResult();

	if (args_count > 0) {
		// similar to switch
		uint curr_script = 0;
		uint curr_handler = 0;

		for (uint i = 0; i < handlers_count; ++i) {
			curr_script = stream->readUint32LE();
			curr_handler = stream->readUint32LE();

			if (curr_handler == arguments[0].u32) {
				script_id = curr_script;
				break;
			}
		}

		if (!script_id && curr_handler == 0) // default handler
			script_id = curr_script;
	} else {
		script_id = stream->readUint32LE();
	}

	if (!script_id)
		return FunctionResult();

	return runScript(script_id, arguments);
}

FunctionResult Composer4Engine::runScript(uint16 id, Common::Array<Variable> &arguments) {
	_callsDepth++;

	Variable result; // = _scriptEngine->run(id, arguments);
	if (_callsDepth == 1) {
		for (const auto &task : _libraryLoadTasks) {
			if (task.load) {
				loadLibrary(task.id);
			} else {
				freeLibrary(task.id);
			}
		}
		_libraryLoadTasks.clear();
	}
	_callsDepth--;
	return result;
}

Common::SeekableReadStream *Composer4Engine::loadResource(uint16 id, ResourceType type, bool segmented) {
	for (const auto &library : _libraries) {
		auto *resource = library->loadResource(id, type, segmented);
		if (resource)
			return resource;
	}
	return nullptr;
}

void Composer4Engine::onResourceLoad(uint16 id, ResourceType type) {
	switch (type) {
	case ResourceType::kButton:
		if (const Common::ScopedPtr<Common::SeekableReadStream> resource{loadResource(id, type)}) {
			_buttonsMan->addButton(id, *resource);
		}
		// update cursor
		break;
	case ResourceType::kVariable:
	default:
		break;
	}
}

void Composer4Engine::onResourceFree(uint16 id, ResourceType type) {
	switch (type) {
	case ResourceType::kButton:
		_buttonsMan->removeButton(id);
		break;
	case ResourceType::kVariable:
	default:
		break;
	}
}

int32 Composer4Engine::findLibraryIndex(uint16 id) const {
	for (uint i = 0; i < _libraries.size(); ++i) {
		if (_libraries[i]->getId() == id) {
			return i;
		}
	}
	return -1;
}

uint16_t Composer4Engine::loadLibrary(uint16 id) {
	if (findLibraryIndex(id) != -1) {
		return 0;
	}

	Common::String fileName;
	// todo: get fileName from ini

	_libraries.push_back(new Library(id));
	// important: we should add library to list before opening
	if (_libraries.back()->open(fileName)) {
		return id;
	}

	delete _libraries.back();
	_libraries.pop_back();
	return 0;
}

void Composer4Engine::freeLibrary(uint16 id) {
	for (uint i = 0; i < _libraries.size(); ++i) {
		if (_libraries[i]->getId() == id) {
			delete _libraries[i];
			_libraries.erase(_libraries.begin() + i);
		}
	}

	const auto libraryIndex = findLibraryIndex(id);
	if (libraryIndex != -1) {
		delete _libraries.remove_at(libraryIndex);
	}
	clearDeadTimers();
	if (libraryIndex != -1) {
		Common::Array<Variable> arguments = {id};
		runEvent(kXLibraryFree, arguments);
	}
}

void Composer4Engine::startTimer(uint index, uint duration, uint16 id, uint count) {
	_timers[index].baseTime = _system->getMillis();
	_timers[index].duration = 10 * duration;
	_timers[index].scriptId = id;
	_timers[index].count = count;
}

void Composer4Engine::stopTimer(uint index) {
	_timers[index].scriptId = 0;
	_timers[index].count = 0;
}

void Composer4Engine::suspendTimer(uint index, bool suspend) {
	if (suspend) {
		if (!_timers[index].duration)
			return;
		_timers[index].savedCount = _timers[index].count;
		_timers[index].baseTime -= _system->getMillis();
		_timers[index].count = 0;
	} else {
		if (_timers[index].duration)
			return;
		_timers[index].count = _timers[index].savedCount;
		_timers[index].baseTime += _system->getMillis();
		_timers[index].savedCount = 0;
	}
}

void Composer4Engine::clearDeadTimers() {
	for (uint i = 0; i < ARRAYSIZE(_timers); ++i) {
		Common::ScopedPtr<Common::SeekableReadStream> stream{loadResource(_timers[i].scriptId, ResourceType::kSound)};
		if (!stream)
			stopTimer(i);
	}
}

void Composer4Engine::updateTimers(uint time) {
	for (uint i = 0; i < ARRAYSIZE(_timers); ++i) {
		if (_timers[i].count && (_timers[i].baseTime + _timers[i].duration) <= time) {
			if (_timers[i].count != UINT_MAX) {
				_timers[i].count--;
			}
			_timers[i].baseTime = time;

			Common::Array<Variable> arguments = {Variable(i)};
			if (_timers[i].scriptId) {
				runScript(_timers[i].scriptId, arguments);
			} else {
				runEvent(kXTimer, arguments);
			}
		}
	}
}

int Composer4Engine::setPalette(uint16 id, byte brightness) {
	Common::ScopedPtr<Common::SeekableReadStream> stream{loadResource(id, ResourceType::kPalette)};
	if (id) {
		Common::ScopedPtr<Common::SeekableReadStream> stream{loadResource(id, ResourceType::kPalette)};
		if (stream) {
			uint16 count = MIN<uint16_t>(256, stream->readUint16LE());
			byte palette[256 * 3];
			stream->read(palette, count * 3);
			setPaletteFragment(palette, 0, count, brightness);
			return 1;
		}
		return 0;
	}

	// alter brightness
	setPaletteFragment(nullptr, 0, 256, brightness);
	return 1;
}

void Composer4Engine::setPaletteFragment(byte *src, uint index, uint count, byte brightness) const {
	if (index > 255)
		return;

	count = MIN(count, 256 - index);
	brightness = MIN<byte>(brightness, 100);

	byte entries[256 * 3];

	_system->getPaletteManager()->grabPalette(entries, index, count);

	if (src) {
		memcpy(entries + index * 3, src, count * 3);
	}

	for (uint i = 0; i < count; ++i) {
		entries[i] = (entries[i] * brightness) / 100;
	}

	entries[ARRAYSIZE(entries) - 1] = UINT8_MAX;
	entries[ARRAYSIZE(entries) - 2] = UINT8_MAX;
	entries[ARRAYSIZE(entries) - 3] = UINT8_MAX;
	entries[0] = 0;
	entries[1] = 0;
	entries[2] = 0;

	_system->getPaletteManager()->setPalette(entries, 0, 256);
}

void Composer4Engine::getPaletteFragment(byte *dst, uint index, uint count) const {
	if (index <= 255) {
		count = MIN(count, 256 - index);
		_system->getPaletteManager()->grabPalette(dst, index, count);
	}
}

void Composer4Engine::getResourcePaletteFragment(uint16 id, byte *dst, uint index, uint count) {
	Common::ScopedPtr<Common::SeekableReadStream> stream{loadResource(id, ResourceType::kPalette)};
	if (!stream)
		return;
	uint16 entriesCount = stream->readUint16LE();

	index = MIN<uint>(entriesCount - 1, index);
	count = MIN(count, entriesCount - index);

	stream->skip(3 * index);
	stream->read(dst, count * 3);
}

} // namespace Composer4
