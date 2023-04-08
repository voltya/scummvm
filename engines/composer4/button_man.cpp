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

#include "composer4/button_man.h"
#include "composer4/composer4.h"

namespace Composer4 {

enum class ButtonType : uint16 {
	kRectButton = 0,
	kEllipticButton = 1,
	kPolygonButton = 3,
	kSpriteButton = 4
};

struct Button {
	uint16 id = 0;
	ButtonType type = ButtonType::kRectButton;
	uint16 z = 0;
	// uint16 scriptId = 0;
	// uint16 scriptIdIn = 0;
	// uint16 scriptIdOut = 0;
	bool active = false;

	bool contains(Common::Point point) const;

	Common::Rect rect;
	Common::Array<uint16> spriteIds;
};

bool Button::contains(Common::Point point) const {
	switch (type) {
	case ButtonType::kRectButton:
		return rect.contains(point);
	case ButtonType::kEllipticButton: {
		if (!rect.contains(point))
			return false;

		const int16 a = rect.width() / 2;
		const int16 b = rect.height() / 2;

		const Common::Point adjustedPos = point - Common::Point(rect.left + a, rect.top + b);
		return ((adjustedPos.x * adjustedPos.x) / (a * a) + (adjustedPos.y * adjustedPos.y) / (b * b) < 1);
	}
	// case ButtonType::kPolygonButton: // fall through
	case ButtonType::kSpriteButton: // fall through
	default:
		error("Unexpected button type %hu", static_cast<uint16>(type));
	}
}

ButtonManager::ButtonManager()
	: _buttons([](const Button *a, const Button *b) { return a->z - b->z; }) {}

ButtonManager::~ButtonManager() {
	for (const auto *button : _buttons) {
		delete button;
	}
}

bool ButtonManager::callFunction(FunctionOpcode opcode, Common::Array<Variable> &arguments, Variable &ret_value) {
	switch (opcode) {
	case FunctionOpcode::kxResourceLoad:
		if (arguments[1].u32 == static_cast<uint32>(ResourceType::kButton)) {
			addButton(arguments[0].u16);
		}
		return false;
	case FunctionOpcode::kxResourceRelease:
		if (arguments[1].u32 == static_cast<uint32>(ResourceType::kButton)) {
			removeButton(arguments[0].u16);
		}
		return false;
	case FunctionOpcode::kActivateButton:
	case FunctionOpcode::kDeactivateButton:
		activateButton(arguments[0].u16, opcode == FunctionOpcode::kActivateButton);
		ret_value = 1;
		return true;
	case FunctionOpcode::kGetButton: {
		Common::Point p{arguments[0].i16, arguments[1].i16};
		// TODO: SpriteEngine;
		auto *button = findButton(p, 0);
		ret_value.i32 = button ? button->id : 0;
		return true;
	}
	default:
		return false;
	}
}

void ButtonManager::addButton(uint16 id) {
	Common::ScopedPtr<Common::SeekableReadStream> stream{g_engine->openResource(id, ResourceType::kButton)};
	if (!stream) {
		return;
	}

	const uint16 firstWord = stream->readUint16LE();
	auto type = static_cast<ButtonType>(firstWord & 0x0fff);

	Common::ScopedPtr<Button> button{new Button()};
	button->id = id;
	button->type = type;
	button->z = stream->readUint16LE();

	// button->scriptId = stream->readUint16LE();
	// button->scriptIdIn = stream->readUint16LE();
	// button->scriptIdOut = stream->readUint16LE();
	stream->skip(6);

	button->active = firstWord >> 15;

	stream->skip(4);

	bool isError = false;
	switch (type) {
	case ButtonType::kRectButton:
	case ButtonType::kEllipticButton:
		stream->readUint16LE();
		button->rect.left = stream->readSint16LE();
		button->rect.top = stream->readSint16LE();
		button->rect.right = stream->readSint16LE();
		button->rect.bottom = stream->readSint16LE();
		assert(button->rect.isValidRect());
		break;
	case ButtonType::kSpriteButton:
		button->spriteIds.resize(stream->readUint16LE());
		isError = button->spriteIds.empty();
		for (auto &spriteId : button->spriteIds) {
			spriteId = stream->readUint16LE();
		}
		break;
	case ButtonType::kPolygonButton:
		// Fisher-Price Ready For School Reading has such button, but engine ignores it
		isError = true;
		break;
	default:
		error("Unexpected button type %hu", static_cast<uint16>(type));
		break;
	}

	if (!isError) {
		_buttons.insert(button.release());
	}
}

void ButtonManager::removeButton(uint16 id) {
	for (auto it = _buttons.begin(); it != _buttons.end();) {
		if ((*it)->id == id) {
			delete *it;
			it = _buttons.erase(it);
		} else {
			++it;
		}
	}
}

void ButtonManager::activateButton(uint16 id, bool active) {
	for (auto *button : _buttons) {
		if (button->id == id)
			button->active = active;
		// no break intended
	}
}

const Button *ButtonManager::findButton(Common::Point point, uint16 sprite_id) const {
	for (const auto *button : _buttons) {
		if (!button->active)
			continue;

		if (button->type != ButtonType::kSpriteButton) {
			if (button->contains(point))
				return button;
		}

		if (sprite_id == 0)
			continue;

		if (Common::find(button->spriteIds.begin(), button->spriteIds.end(), sprite_id) != button->spriteIds.end())
			return button;
	}

	return nullptr;
}

} // End of namespace Composer4
