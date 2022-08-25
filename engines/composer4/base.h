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
	kUniFont = MKTAG('U', 'F', 'I', '1'),
	kMouthSync = MKTAG('A', 'A', 'A', '1')
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
// Unused opcodes in games are commented out
// x prefix means event
enum FunctionOpcode : uint16 {
	kXAnimationStart = 1,
	kXAnimationEnd = 2,
	kXLibraryLoad = 3,
	kXLibraryFree = 4,
	kXKeyDown = 5,
	kXKeyChar = 6,
	kXKeyUp = 7,
	kXQuitRequest = 8,
	kXAnimationTick = 9,
	kXLButtonDown = 10,
	kXLButtonUp = 11,
	kXRButtonDown = 12,
	kXRButtonUp = 13,
	kXMouseIn = 14,
	kXMouseOut = 15,
	kXMouseOver = 16,
	kXTimer = 17,
	kFWDone = 18,
	kFWSkipCustom = 19,
	kFWAreaChanged = 20,
	kXResourceLoad = 21,
	kXResourceRelease = 22,
	kXPrintPage = 23,
	kXPrintError = 24,
	kxLibLoadError = 25,
	kxSmackerStart = 30,
	kxSmackerEnd = 31,
	kxSmackerFrame = 32,
	kxKAUserError = 50,
	kxTimeToNextTick = 90,

	kStopAnimation = 35002,
	kStartTimer = 35004,
	kStopTimer = 35005,
	kSetCursor = 35006,
	kGetCursor = 35007,
	kShowCursor = 35008,
	kHideCursor = 35009,
	kActivateButton = 35011,
	kDeactivateButton = 35012,
	// kReloadLibrary = 35013,
	kLoadLibrary = 35014,
	kFreeLibrary = 35015,
	kSetPalette = 35016,
	// kSaveVars = 35017,
	// kRestoreVars = 35018,
	kSingleEvent = 35019,
	kGetCursorPos = 35020,
	kSetBackground = 35021,
	kSetDefBgColor = 35022,
	kClearAllSprites = 35023,
	kDrawSprite = 35024,
	kClearSprite = 35025,
	kExit = 35026,
	kSaveData = 35027,
	kLoadData = 35028,
	kGetSpriteSize = 35029,
	kPrint = 35030,
	kStartAnimation = 35032,
	kInitString = 35033,
	kTextOut = 35035,
	kSetFont = 35036,
	kSetTextAlign = 35037,
	// kGetSysInfo = 35039,
	kSuspendAnimation = 35043,
	kScrollScreen = 35044,
	kTransformSprite = 35045,
	kSetShadingTable = 35046,
	kMoveAnimation = 35047,
	kScaleAnimation = 35048,
	kCreateSprite = 35049,
	kMoveSprite = 35050,
	kDebugTrace = 35051,
	kSuspendTimer = 35052,
	kEnableLibrary = 35053,
	kSetSoundVolume = 35054,
	kSetPaletteFragment = 35056,
	kGetPaletteFragment = 35057,
	kGetResourcePaletteFragment = 35058,
	// kCalcRecolorTable = 35059,
	kSmackerStart = 35060,
	kSmackerStop = 35061,
	kSmackerSuspend = 35062,
	kGetAnimInfo = 35070,
	kGetButton = 35071,
	kRenderSprite2Sprite = 35072,
	kRedrawSprite = 35073,
	kGetTimeDate = 35074,
	kSetTranslationTable = 35075,
	kRecolorSprite = 35076,
	kGetAnimList = 35077,
	kLoadSprite = 35090,
	kSaveSprite = 35091,
	kFlushScreen = 35100,
	// kResourceLoad = 35321, V5
	// kResourceRelease = 35322, V5
	kFWSetMap = 36003,
	kFWSetAnim = 36004,
	kFWSetStepSize = 36005,
	kFWSetMaxSlide = 36006,
	kFWStopWalk = 36007,
	kFWGetScale = 36008,
	kFWGetZLayer = 36009,
	kFWSetStartPoint = 36010,
	kFWSetStartDirection = 36011,
	kFWWalkTo = 36012,
	kFWGetLocation = 36014,
	kFWGetArea = 36015,
	kFWSuspend = 36016,
	kFWSkipToEnd = 36017,
	kGetTableDimensions = 37001,
	kGetTableElement = 37002,
	// kGetTableElementType = 37003, // V5
	// kTableFindElement = 37004, // V5
	// kButtonSetZ = 37204, // V5
	kUniTextOut = 37500,
	kUniSetFont = 37501,
	kUniSetColorTable = 37502,
	kUniSetTextColor = 37504,
	kUniSetupSlot = 37505,
	kUniSetTextDirection = 37506,
	// kUniSetTextColor32 = 37507, // V5
	// kUniSetSlot32 = 37508, // V5
	// kUniGetFontHeight = 37509, // V5 
	// kUniTextOutExWidthCalc = 37510, // V5
	// kUniTextOutExCharByPoint = 37511, // V5
	// kUniTextOutExRectByChar = 37512, // V5
	kKAGetIniString = 37999,
	kGetPageSize = 38000,
	kPrintSprite = 38001,
	kKAGetUser = 38500,
	kKACloseUser = 38501,
	// kKACloseDatabase = 38502,
	// kKAGetInt8Attr = 38510,
	// kKAGetInt16Attr = 38511,
	kKAGetInt32Attr = 38512,
	// kKAGetFloatAttr = 38513,
	kKAGetDateAttr = 38514,
	// kKAGetStringAttr = 38515,
	// kKASetInt8Attr = 38520,
	// kKASetInt16Attr = 38521,
	kKASetInt32Attr = 38522,
	// kKASetFloatAttr = 38523,
	kKASetDateAttr = 38524,
	// kKASetStringAttr = 38525,
	kKAAttrExists = 38530,
	// kKADeleteAttr = 38531,
	// kKAGetProdInt8 = 38540,
	// kKAGetProdInt16 = 38541,
	// kKAGetProdInt32 = 38542,
	// kKAGetProdFloat = 38543,
	// kKAGetProdDate = 38544,
	// kKAGetProdString = 38545,
	// kKASetProdInt8 = 38550,
	// kKASetProdInt16 = 38551,
	// kKASetProdInt32 = 38552,
	// kKASetProdFloat = 38553,
	// kKASetProdDate = 38554,
	// kKASetProdString = 38555,
	// kKAProdDataExists = 38560,
	// kKADeleteProdData = 38561,
	kKASkillAddSuccess = 38570,
	kKASkillAddFailure = 38571,
	kKASkillGetLevel = 38572,
	kKASkillSetLevel = 38573,
	kKASkillAutoChangeLevel = 38574,
	kKASkillGetDateRange = 38575,
	// kKASkillGetLevelRange = 38576,
	kKASkillGetResult1 = 38580,
	// kKASkillGetResult2 = 38581,
	kKASkillGetResult3 = 38582,
	// kKASkillGetResult4 = 38583,
	kKASkillGetResult5 = 38584,
	kKASkillGetResult6 = 38585,
	kKAUserEnumeration = 38590,
	kKAUserAnyMoreElements = 38591,
	kKAUserGetNextElement = 38592,
	kKAUserDeleteUser = 38593,
	kKAUserSetPath = 38594,
	kKAUserExist = 38595,
	// kKAGetArrayInt8Attr = 38610,
	// kKAGetArrayInt16Attr = 38611,
	// kKAGetArrayInt32Attr = 38612,
	// kKAGetArrayFloatAttr = 38613,
	// kKAGetArrayStringAttr = 38615,
	// kKASetArrayInt8Attr = 38620,
	// kKASetArrayInt16Attr = 38621,
	// kKASetArrayInt32Attr = 38622,
	// kKASetArrayFloatAttr = 38623,
	// kKASetArrayStringAttr = 38625,
	// kSwitchScreen = 50114, // V5
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
