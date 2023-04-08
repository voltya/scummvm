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

#include "composer4/timers_man.h"
#include "composer4/composer4.h"

namespace Composer4 {

static constexpr uint kInfinityTimer = 0xffffffff;

bool TimersManager::callFunction(FunctionOpcode opcode, Common::Array<Variable> &arguments, Variable &ret_value) {
	switch (opcode) {
	case FunctionOpcode::kStartTimer:
		startTimer(arguments[0].u32, arguments[1].u32, arguments[2].u16, kInfinityTimer);
		return true;
	case FunctionOpcode::kStopTimer:
		stopTimer(arguments[0].u32);
		return true;
	case FunctionOpcode::kSuspendTimer:
		suspendTimer(arguments[0].u32, arguments[1].u8);
		return true;
	case FunctionOpcode::kSingleEvent:
		startTimer(arguments[0].u32, arguments[1].u32, arguments[2].u16, 1);
		return true;
	default:
		return false;
	}
}

void TimersManager::clearDeadTimers() {
	for (uint i = 0; i < kTimersCount; ++i) {
		Common::ScopedPtr<Common::SeekableReadStream> stream{g_engine->openResource(_timers[i].scriptId, ResourceType::kScript)};
		if (!stream)
			stopTimer(i);
	}
}

void TimersManager::update(uint time) {
	for (uint i = 0; i < kTimersCount; ++i) {
		if (_timers[i].count && (_timers[i].baseTime + _timers[i].duration) <= time) {
			if (_timers[i].count != kInfinityTimer) {
				_timers[i].count--;
			}
			_timers[i].baseTime = time;

			Common::Array<Variable> arguments = {Variable(i)};
			if (_timers[i].scriptId) {
				// g_engine->runScript(_timers[i].scriptId, arguments);
			} else {
				g_engine->callFunction(FunctionOpcode::kxTimer, arguments);
			}
		}
	}
}

void TimersManager::startTimer(uint index, uint duration, uint16 id, uint count) {
	assert(index < kTimersCount);
	if (index < kTimersCount) {
		_timers[index].baseTime = g_system->getMillis();
		_timers[index].duration = 10 * duration;
		_timers[index].scriptId = id;
		_timers[index].count = count;
	}
}

void TimersManager::stopTimer(uint index) {
	assert(index < kTimersCount);
	if (index < kTimersCount) {
		_timers[index].scriptId = 0;
		_timers[index].count = 0;
	}
}

void TimersManager::suspendTimer(uint index, bool suspend) {
	assert(index < kTimersCount);
	if (index < kTimersCount) {
		if (suspend) {
			if (!_timers[index].count)
				return;

			_timers[index].savedCount = _timers[index].count;
			_timers[index].baseTime -= g_system->getMillis();
			_timers[index].count = 0;
		} else {
			if (_timers[index].count)
				return;

			_timers[index].count = _timers[index].savedCount;
			_timers[index].baseTime += g_system->getMillis();
			_timers[index].savedCount = 0;
		}
	}
}

} // End of namespace Composer4
