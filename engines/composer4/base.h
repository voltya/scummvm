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

#ifndef COMPOSER4_BASE_H
#define COMPOSER4_BASE_H

#include "common/endian.h"

namespace Composer4 {

// Dumped from resources with debug info

enum class ResourceType : uint32 {
	kAnimation = MKTAG('A', 'N', 'I', 'M'),
	kBitmap = MKTAG('B', 'M', 'A', 'P'),
	kButton = MKTAG('B', 'U', 'T', 'N'),
	kPalette = MKTAG('C', 'T', 'B', 'L'),
	kTable = MKTAG('D', 'T', 'A', 'B'),
	kEvent = MKTAG('E', 'V', 'N', 'T'),
	kScript = MKTAG('S', 'C', 'R', 'P'),
	kShadeTable = MKTAG('S', 'T', 'B', 'L'),
	kText = MKTAG('S', 'T', 'R', ' '),
	kVariable = MKTAG('V', 'A', 'R', 'I'),
	kSound = MKTAG('W', 'A', 'V', 'E'),
	kWalkMap = MKTAG('W', 'M', 'A', 'P'),
	kUniFont = MKTAG('U', 'F', 'I', '1')
};

enum class ArgumentSizeType {
	kVoid = 0,
	kUChar = 1,
	kInt = 2,
	kFloat = 3,
	kShort = 5,
	kString = 8,
};

union Variable {
	Variable() : Variable(nullptr) {}
	Variable(byte v) : Variable(nullptr) { u8 = v; }
	Variable(int8 v) : Variable(nullptr) { u8 = v; }
	Variable(int16 v) : Variable(nullptr) { i16 = v; }
	Variable(uint16 v) : Variable(nullptr) { u16 = v; }
	Variable(int32 v) : Variable(nullptr) { i32 = v; }
	Variable(uint32 v) : Variable(nullptr) { u32 = v; }
	Variable(float v) : Variable(nullptr) { f32 = v; }
	Variable(void *v) : pointer(v) {}

	byte u8;
	int8 i8;
	uint16 u16;
	int16 i16;
	uint32 u32;
	int32 i32;
	float f32;
	void *pointer; // may be unaligned ; use MemoryValue<T> to safely read/write
};

class FunctionResult {
public:
	FunctionResult() = default;
	FunctionResult(Variable v) {
		_var = v;
		_valid = true;
	}

	Variable get() const {
		assert(_valid);
		return _var;
	}

	operator bool() const { return _valid; }

private:
	Variable _var;
	bool _valid = false;
};

// Dumped from debug resources
enum FunctionOpcode : uint16 {
	// Vm events
	kXAnimationStart = 0x1,
	kXAnimationEnd = 0x2,
	kXLibraryLoad = 0x3,
	kXLibraryFree = 0x4,
	kXKeyDown = 0x5,
	kXKeyChar = 0x6,
	kXKeyUp = 0x7,
	kXQuitRequest = 0x8,
	kXAnimationTick = 0x9,
	kXLButtonDown = 0xa,
	kXLButtonUp = 0xb,
	kXRButtonDown = 0xc,
	kXRButtonUp = 0xd,
	kXMouseIn = 0xe,
	kXMouseOut = 0xf,
	kXMouseOver = 0x10,
	kXTimer = 0x11,
	// kXResourceLoad = 0x15,
	// kXResourceFree = 0x16,
	kFWDone = 0x12,
	kFWSkipCustom = 0x13,
	kFWAreaChanged = 0x14,
	kXLibLoadError = 0x19,
	kXSmackerStart = 0x1e,
	kXSmackerEnd = 0x1f,
	kXSmackerFrame = 0x20,
	kXKAUserError = 0x32,
	kXTimeToNextTick = 0x5a,
	// Vm events end

	kFirstFunctionOpcode = 0x88b8,
	kStopAnimation = 0x88ba,
	kStartTimer = 0x88bc,
	kStopTimer = 0x88bd,
	kSetCursor = 0x88be,
	kGetCursor = 0x88bf,
	kShowCursor = 0x88c0,
	kHideCursor = 0x88c1,
	kActivateButton = 0x88c3,
	kDeactivateButton = 0x88c4,
	kReloadLibrary = 0x88c5,
	kLoadLibrary = 0x88c6,
	kFreeLibrary = 0x88c7,
	kSetPalette = 0x88c8,
	kSaveVars = 0x88c9,
	kRestoreVars = 0x88ca,
	kSingleEvent = 0x88cb,
	kGetCursorPos = 0x88cc,
	kSetBackground = 0x88cd,
	kSetDefBgColor = 0x88ce,
	kClearAllSprites = 0x88cf,
	kDrawSprite = 0x88d0,
	kClearSprite = 0x88d1,
	kExit = 0x88d2,
	kSaveData = 0x88d3,
	kLoadData = 0x88d4,
	kGetSpriteSize = 0x88d5,
	kPrint = 0x88d6,
	kStartAnimation = 0x88d8,
	kInitString = 0x88d9,
	kTextOut = 0x88db,
	kSetFont = 0x88dc,
	kSetTextAlign = 0x88dd,
	kGetSysInfo = 0x88df,
	kSuspendAnimation = 0x88e3,
	kScrollScreen = 0x88e4,
	kTransformSprite = 0x88e5,
	kSetShadingTable = 0x88e6,
	kMoveAnimation = 0x88e7,
	kScaleAnimation = 0x88e8,
	kCreateSprite = 0x88e9,
	kMoveSprite = 0x88ea,
	kDebugTrace = 0x88eb,
	kSuspendTimer = 0x88ec,
	kEnableLibrary = 0x88ed,
	kSetSoundVolume = 0x88ee,
	kSetPaletteFragment = 0x88f0,
	kGetPaletteFragment = 0x88f1,
	kGetResourcePaletteFragment = 0x88f2,
	kSmackerStart = 0x88f4,
	kSmackerStop = 0x88f5,
	kSmackerSuspend = 0x88f6,
	kGetAnimInfo = 0x88fe,
	kGetButton = 0x88ff,
	kRenderSprite2Sprite = 0x8900,
	kRedrawSprite = 0x8901,
	kGetTimeDate = 0x8902,
	kSetTranslationTable = 0x8903,
	kRecolorSprite = 0x8904,
	kGetAnimList = 0x8905,
	kLoadSprite = 0x8912,
	kSaveSprite = 0x8913,
	kFlushScreen = 0x891c,
	kFWSetMap = 0x8ca3,
	kFWSetAnim = 0x8ca4,
	kFWSetStepSize = 0x8ca5,
	kFWSetMaxSlide = 0x8ca6,
	kFWStopWalk = 0x8ca7,
	kFWGetScale = 0x8ca8,
	kFWGetZLayer = 0x8ca9,
	kFWSetStartPoint = 0x8caa,
	kFWSetStartDirection = 0x8cab,
	kFWWalkTo = 0x8cac,
	kFWGetLocation = 0x8cae,
	kFWGetArea = 0x8caf,
	kFWSuspend = 0x8cb0,
	kFWSkipToEnd = 0x8cb1,
	kGetTableDimensions = 0x9089,
	kGetTableElement = 0x908a,
	kButtonSetZ = 0x9154,
	kUniTextOut = 0x927c,
	kUniSetFont = 0x927d,
	kUniSetColorTable = 0x927e,
	kUniSetTextColor = 0x9280,
	kUniSetupSlot = 0x9281,
	kUniSetTextDirection = 0x9282,
	kUniSetTextColor32 = 0x9283,
	kUniSetSlot32 = 0x9284,
	kUniGetFontHeight = 0x9285,
	kUniTextOutExWidthCalc = 0x9286,
	kUniTextOutExCharByPoint = 0x9287,
	kUniTextOutExRectByChar = 0x9288,
	// what does KA mean ?
	kKAGetIniString = 0x946f,
	kKAGetUser = 0x9664,
	kKACloseUser = 0x9665,
	kKACloseDatabase = 0x9666,
	kKAGetInt8Attr = 0x966e,
	kKAGetInt16Attr = 0x966f,
	kKAGetInt32Attr = 0x9670,
	kKAGetFloatAttr = 0x9671,
	kKAGetDateAttr = 0x9672,
	kKAGetStringAttr = 0x9673,
	kKASetInt8Attr = 0x9678,
	kKASetInt16Attr = 0x9679,
	kKASetInt32Attr = 0x967a,
	kKASetFloatAttr = 0x967b,
	kKASetDateAttr = 0x967c,
	kKASetStringAttr = 0x967d,
	kKAAttrExists = 0x9682,
	kKADeleteAttr = 0x9683,
	kKAGetProdInt8 = 0x968c,
	kKAGetProdInt16 = 0x968d,
	kKAGetProdInt32 = 0x968e,
	kKAGetProdFloat = 0x968f,
	kKAGetProdDate = 0x9690,
	kKAGetProdString = 0x9691,
	kKASetProdInt8 = 0x9696,
	kKASetProdInt16 = 0x9697,
	kKASetProdInt32 = 0x9698,
	kKASetProdFloat = 0x9699,
	kKASetProdDate = 0x969a,
	kKASetProdString = 0x969b,
	kKAProdDataExists = 0x96a0,
	kKADeleteProdData = 0x96a1,
	kKASkillAddSuccess = 0x96aa,
	kKASkillAddFailure = 0x96ab,
	kKASkillGetLevel = 0x96ac,
	kKASkillSetLevel = 0x96ad,
	kKASkillAutoChangeLevel = 0x96ae,
	kKASkillGetDateRange = 0x96af,
	kKASkillGetLevelRange = 0x96b0,
	kKASkillGetResult1 = 0x96b4,
	kKASkillGetResult2 = 0x96b5,
	kKASkillGetResult3 = 0x96b6,
	kKASkillGetResult4 = 0x96b7,
	kKASkillGetResult5 = 0x96b8,
	kKASkillGetResult6 = 0x96b9,
	kKAUserEnumeration = 0x96be,
	kKAUserAnyMoreElements = 0x96bf,
	kKAUserGetNextElement = 0x96c0,
	kKAUserDeleteUser = 0x96c1,
	kKAUserSetPath = 0x96c2,
	kKAUserExist = 0x96c3,
	kKAGetArrayInt8Attr = 0x96d2,
	kKAGetArrayInt16Attr = 0x96d3,
	kKAGetArrayInt32Attr = 0x96d4,
	kKAGetArrayFloatAttr = 0x96d5,
	kKAGetArrayStringAttr = 0x96d7,
	kKASetArrayInt8Attr = 0x96dc,
	kKASetArrayInt16Attr = 0x96dd,
	kKASetArrayInt32Attr = 0x96de,
	kKASetArrayFloatAttr = 0x96df,
	kKASetArrayStringAttr = 0x96e1,
	kSwitchScreen = 0xc3c2,
};

template<typename T>
class MemoryValue {
public:
	explicit MemoryValue(void *ptr)
		: _ptr(static_cast<T *>(ptr)) {
		assert(ptr);
	}

	MemoryValue<T> &operator=(T value) {
		writeValue(value);
		return *this;
	}

	MemoryValue<T> &operator=(MemoryValue<T> value) {
		writeValue(value);
		return *this;
	}

	void nextValue() {
		_ptr += 1;
	}

	operator T() {
		return readValue<>();
	}

private:
	template<size_t Size = sizeof(T)>
	T readValue();

	template<>
	T readValue<1>() {
		return *reinterpret_cast<T *>(_ptr);
	};

	template<>
	T readValue<2>() {
		return READ_UINT16(_ptr);
	};

	template<>
	T readValue<4>() {
		uint32 v = READ_UINT32(_ptr);
		return *reinterpret_cast<T *>(&v);
	};

	template<>
	T readValue<8>() {
		uint64 v = READ_UINT64(_ptr);
		return *reinterpret_cast<T *>(&v);
	};

	template<size_t Size = sizeof(T)>
	void writeValue(T val);

	template<>
	void writeValue<1>(T val) {
		*reinterpret_cast<T *>(_ptr) = val;
	}

	template<>
	void writeValue<2>(T val) {
		WRITE_UINT16(_ptr, *reinterpret_cast<uint16 *>(&val));
	}

	template<>
	void writeValue<4>(T val) {
		WRITE_UINT32(_ptr, *reinterpret_cast<uint32 *>(&val));
	}

	template<>
	void writeValue<8>(T val) {
		WRITE_UINT64(_ptr, *reinterpret_cast<uint64 *>(&val));
	}

	T *_ptr;
};

} // End of namespace Composer4

#endif
