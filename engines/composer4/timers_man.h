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

#ifndef COMPOSER4_TIMERS_MAN_H
#define COMPOSER4_TIMERS_MAN_H

#include "composer4/module.h"

namespace Composer4 {

class TimersManager : public Module {
public:
	~TimersManager() override = default;

	bool callFunction(FunctionOpcode opcode, Common::Array<Variable> &arguments, Variable &ret_value) override;

	void clearDeadTimers();
	void update(uint time);

private:
	void startTimer(uint index, uint duration, uint16 id, uint count);
	void stopTimer(uint index);
	void suspendTimer(uint index, bool suspend);

private:
	static constexpr uint kTimersCount = 16;
	struct Timer {
		uint32 baseTime = 0;
		uint32 duration = 0;
		uint32 count = 0;
		uint16 scriptId = 0;
		uint32 savedCount = 0;
	};
	Timer _timers[kTimersCount];
};

} // End of namespace Composer4

#endif
