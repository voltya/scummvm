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

#include "common/stream.h"

#include "composer4/button.h"

namespace Composer4 {

enum class ButtonType : uint16 {
	kRectButton = 0,
	kEllipticButton = 1,
	// kPolygonButton = 3, // not used in games
	kSpriteButton = 4
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
	case ButtonType::kSpriteButton:     // fall through
	default:
		error("Unexpected button type %d", static_cast<uint16>(type));
	}
}

ButtonsContainer::ButtonsContainer()
	: _buttons([](auto &a, auto &b) { return a.z - b.z; }) {}

void ButtonsContainer::addButton(uint16 id, Common::SeekableReadStream &stream) {
	const uint16 firstWord = stream.readUint16LE();
	auto type = static_cast<ButtonType>(firstWord & 0x0fff);

	Button button;
	button.id = id;
	button.type = type;
	button.z = stream.readUint16LE();
	button.scriptId = stream.readUint16LE();
	button.scriptIdIn = stream.readUint16LE();
	button.scriptIdOut = stream.readUint16LE();
	button.active = firstWord >> 15;

	stream.skip(4);

	bool isError = false;
	switch (type) {
	case ButtonType::kRectButton:
	case ButtonType::kEllipticButton:
		button.rect.left = stream.readSint16LE();
		button.rect.top = stream.readSint16LE();
		button.rect.right = stream.readSint16LE();
		button.rect.bottom = stream.readSint16LE();
		break;
	/*
	case ButtonType::kPolygonButton:
		button.polygon.resize(stream.readUint16LE());
		for (auto &point : button.polygon) {
			point.x = stream.readSint16LE();
			point.y = stream.readSint16LE();
		}
		isError = button.polygon.empty();
		break;
	*/	
	case ButtonType::kSpriteButton:
		button.spriteIds.resize(stream.readUint16LE());
		isError = button.spriteIds.empty();
		for (auto &spriteId : button.spriteIds) {
			spriteId = stream.readUint16LE();
		}
		break;
	// case ButtonType::kPolygonButton: // fall through
	default:
		error("Unexpected button type %d", static_cast<uint16>(type));
	}

	if (!isError) {
		_buttons.insert(button);
	}
}

void ButtonsContainer::removeButton(uint16 id) {
	for (auto it = _buttons.begin(); it != _buttons.end();) {
		if (it->id == id)
			it = _buttons.erase(it);
		else
			++it;
	}
}

void ButtonsContainer::activateButton(uint16 id, bool active) {
	for (auto &button : _buttons) {
		if (button.id == id)
			button.active = active;
		// no break intended
	}
}

const Button *ButtonsContainer::findButton(Common::Point point, uint16 sprite_id) const {
	for (const auto &button : _buttons) {
		if (!button.active)
			continue;

		if (button.type != ButtonType::kSpriteButton) {
			if (button.contains(point))
				return &button;
		}

		if (sprite_id == 0)
			continue;

		if (Common::find(button.spriteIds.begin(), button.spriteIds.end(), sprite_id) != button.spriteIds.end())
			return &button;
	}

	return nullptr;
}

} // End of namespace Composer4
