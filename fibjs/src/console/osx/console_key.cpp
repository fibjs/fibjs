/*
 * console_key.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: lion
 */

#include "object.h"
#include "ifs/console.h"

#ifdef Darwin

#include <Carbon/Carbon.h>
#import <ApplicationServices/ApplicationServices.h>
#import <IOKit/hidsystem/IOHIDLib.h>
#import <IOKit/hidsystem/ev_keymap.h>

namespace fibjs
{

enum _MMKeyCode {
	K_NOT_A_KEY = 9999,
	K_BACKSPACE = kVK_Delete,
	K_DELETE = kVK_ForwardDelete,
	K_RETURN = kVK_Return,
	K_TAB = kVK_Tab,
	K_ESCAPE = kVK_Escape,
	K_UP = kVK_UpArrow,
	K_DOWN = kVK_DownArrow,
	K_RIGHT = kVK_RightArrow,
	K_LEFT = kVK_LeftArrow,
	K_HOME = kVK_Home,
	K_END = kVK_End,
	K_PAGEUP = kVK_PageUp,
	K_PAGEDOWN = kVK_PageDown,
	K_F1 = kVK_F1,
	K_F2 = kVK_F2,
	K_F3 = kVK_F3,
	K_F4 = kVK_F4,
	K_F5 = kVK_F5,
	K_F6 = kVK_F6,
	K_F7 = kVK_F7,
	K_F8 = kVK_F8,
	K_F9 = kVK_F9,
	K_F10 = kVK_F10,
	K_F11 = kVK_F11,
	K_F12 = kVK_F12,
	K_F13 = kVK_F13,
	K_F14 = kVK_F14,
	K_F15 = kVK_F15,
	K_F16 = kVK_F16,
	K_F17 = kVK_F17,
	K_F18 = kVK_F18,
	K_F19 = kVK_F19,
	K_F20 = kVK_F20,
	K_F21 = K_NOT_A_KEY,
	K_F22 = K_NOT_A_KEY,
	K_F23 = K_NOT_A_KEY,
	K_F24 = K_NOT_A_KEY,
	K_META = kVK_Command,
	K_ALT = kVK_Option,
	K_CONTROL = kVK_Control,
	K_SHIFT = kVK_Shift,
	K_RIGHTSHIFT = kVK_RightShift,
	K_CAPSLOCK = kVK_CapsLock,
	K_SPACE = kVK_Space,
	K_INSERT = K_NOT_A_KEY,
	K_PRINTSCREEN = K_NOT_A_KEY,

	K_NUMPAD_0 = kVK_ANSI_Keypad0,
	K_NUMPAD_1 = kVK_ANSI_Keypad1,
	K_NUMPAD_2 = kVK_ANSI_Keypad2,
	K_NUMPAD_3 = kVK_ANSI_Keypad3,
	K_NUMPAD_4 = kVK_ANSI_Keypad4,
	K_NUMPAD_5 = kVK_ANSI_Keypad5,
	K_NUMPAD_6 = kVK_ANSI_Keypad6,
	K_NUMPAD_7 = kVK_ANSI_Keypad7,
	K_NUMPAD_8 = kVK_ANSI_Keypad8,
	K_NUMPAD_9 = kVK_ANSI_Keypad9,

	K_AUDIO_VOLUME_MUTE = 1007,
	K_AUDIO_VOLUME_DOWN = 1001,
	K_AUDIO_VOLUME_UP = 1000,
	K_AUDIO_PLAY = 1016,
	K_AUDIO_STOP = K_NOT_A_KEY,
	K_AUDIO_PAUSE = 1016,
	K_AUDIO_PREV = 1018,
	K_AUDIO_NEXT = 1017,
	K_AUDIO_REWIND = K_NOT_A_KEY,
	K_AUDIO_FORWARD = K_NOT_A_KEY,
	K_AUDIO_REPEAT = K_NOT_A_KEY,
	K_AUDIO_RANDOM = K_NOT_A_KEY,

	K_LIGHTS_MON_UP = 1002,
	K_LIGHTS_MON_DOWN = 1003,
	K_LIGHTS_KBD_TOGGLE = 1023,
	K_LIGHTS_KBD_UP = 1021,
	K_LIGHTS_KBD_DOWN = 1022
};

enum  {
	MOD_NONE = 0,
	MOD_META = kCGEventFlagMaskCommand,
	MOD_ALT = kCGEventFlagMaskAlternate,
	MOD_CONTROL = kCGEventFlagMaskControl,
	MOD_SHIFT = kCGEventFlagMaskShift
};

struct KeyNames
{
	const char* name;
	int32_t key;
};

static KeyNames key_names[] =
{
	{ "backspace",      K_BACKSPACE },
	{ "delete",         K_DELETE },
	{ "enter",          K_RETURN },
	{ "tab",            K_TAB },
	{ "escape",         K_ESCAPE },
	{ "up",             K_UP },
	{ "down",           K_DOWN },
	{ "right",          K_RIGHT },
	{ "left",           K_LEFT },
	{ "home",           K_HOME },
	{ "end",            K_END },
	{ "pageup",         K_PAGEUP },
	{ "pagedown",       K_PAGEDOWN },
	{ "f1",             K_F1 },
	{ "f2",             K_F2 },
	{ "f3",             K_F3 },
	{ "f4",             K_F4 },
	{ "f5",             K_F5 },
	{ "f6",             K_F6 },
	{ "f7",             K_F7 },
	{ "f8",             K_F8 },
	{ "f9",             K_F9 },
	{ "f10",            K_F10 },
	{ "f11",            K_F11 },
	{ "f12",            K_F12 },
	{ "f13",            K_F13 },
	{ "f14",            K_F14 },
	{ "f15",            K_F15 },
	{ "f16",            K_F16 },
	{ "f17",            K_F17 },
	{ "f18",            K_F18 },
	{ "f19",            K_F19 },
	{ "f20",            K_F20 },
	{ "f21",            K_F21 },
	{ "f22",            K_F22 },
	{ "f23",            K_F23 },
	{ "f24",            K_F24 },
	{ "command",        K_META },
	{ "alt",            K_ALT },
	{ "control",        K_CONTROL },
	{ "shift",          K_SHIFT },
	{ "right_shift",    K_RIGHTSHIFT },
	{ "space",          K_SPACE },
	{ "printscreen",    K_PRINTSCREEN },
	{ "insert",         K_INSERT },

	{ "audio_mute",     K_AUDIO_VOLUME_MUTE },
	{ "audio_vol_down", K_AUDIO_VOLUME_DOWN },
	{ "audio_vol_up",   K_AUDIO_VOLUME_UP },
	{ "audio_play",     K_AUDIO_PLAY },
	{ "audio_stop",     K_AUDIO_STOP },
	{ "audio_pause",    K_AUDIO_PAUSE },
	{ "audio_prev",     K_AUDIO_PREV },
	{ "audio_next",     K_AUDIO_NEXT },
	{ "audio_rewind",   K_AUDIO_REWIND },
	{ "audio_forward",  K_AUDIO_FORWARD },
	{ "audio_repeat",   K_AUDIO_REPEAT },
	{ "audio_random",   K_AUDIO_RANDOM },

	{ "numpad_0",		K_NUMPAD_0 },
	{ "numpad_1",		K_NUMPAD_1 },
	{ "numpad_2",		K_NUMPAD_2 },
	{ "numpad_3",		K_NUMPAD_3 },
	{ "numpad_4",		K_NUMPAD_4 },
	{ "numpad_5",		K_NUMPAD_5 },
	{ "numpad_6",		K_NUMPAD_6 },
	{ "numpad_7",		K_NUMPAD_7 },
	{ "numpad_8",		K_NUMPAD_8 },
	{ "numpad_9",		K_NUMPAD_9 },

	{ "lights_mon_up",    K_LIGHTS_MON_UP },
	{ "lights_mon_down",  K_LIGHTS_MON_DOWN },
	{ "lights_kbd_toggle", K_LIGHTS_KBD_TOGGLE },
	{ "lights_kbd_up",    K_LIGHTS_KBD_UP },
	{ "lights_kbd_down",  K_LIGHTS_KBD_DOWN }
};

struct XSpecialCharacterMapping {
	char name;
	int32_t key;
};

struct XSpecialCharacterMapping XSpecialCharacterTable[] = {
	{'\n', K_RETURN}
};

struct XSpecialCharacterMapping XShiftCharacterTable[] = {
	{'!', '1'},
	{'\"', '\''},
	{'#', '3'},
	{'$', '4'},
	{'%', '5'},
	{'&', '7'},
	{'(', '9'},
	{')', '0'},
	{':', ';'},
	{'<', ','},
	{'>', '.'},
	{'?', '/'},
	{'@', '2'},
	{'A', 'a'},
	{'B', 'b'},
	{'C', 'c'},
	{'D', 'd'},
	{'E', 'e'},
	{'F', 'f'},
	{'G', 'g'},
	{'H', 'h'},
	{'I', 'i'},
	{'J', 'j'},
	{'K', 'k'},
	{'L', 'l'},
	{'M', 'm'},
	{'N', 'n'},
	{'O', 'o'},
	{'P', 'p'},
	{'Q', 'q'},
	{'R', 'r'},
	{'S', 's'},
	{'T', 't'},
	{'U', 'u'},
	{'V', 'v'},
	{'W', 'w'},
	{'X', 'x'},
	{'Y', 'y'},
	{'Z', 'z'},
	{'^', '6'},
	{'_', '-'},
	{'{', '['},
	{'|', '\\'},
	{'}', ']'},
	{'~', '`'}
};

CFStringRef createStringForKey(CGKeyCode keyCode)
{
	TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
	CFDataRef layoutData = (CFDataRef)TISGetInputSourceProperty(currentKeyboard,
	                       kTISPropertyUnicodeKeyLayoutData);
	const UCKeyboardLayout *keyboardLayout =
	    (const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);

	UInt32 keysDown = 0;
	UniChar chars[4];
	UniCharCount realLength;

	UCKeyTranslate(keyboardLayout,
	               keyCode,
	               kUCKeyActionDisplay,
	               0,
	               LMGetKbdType(),
	               kUCKeyTranslateNoDeadKeysBit,
	               &keysDown,
	               sizeof(chars) / sizeof(chars[0]),
	               &realLength,
	               chars);
	CFRelease(currentKeyboard);

	return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
}

result_t CheckKeyCodes(exlib::string k, int32_t *code, int32_t *flags)
{
	size_t i;

	if (k.length() == 1)
	{
		char c = k[0];

		for (i = 0; i < ARRAYSIZE(XSpecialCharacterTable); i ++)
			if (c == XSpecialCharacterTable[i].name)
			{
				*code = XSpecialCharacterTable[i].key;
				return 0;
			}

		for (i = 0; i < ARRAYSIZE(XShiftCharacterTable); i ++)
			if (c == XShiftCharacterTable[i].name)
			{
				c = XShiftCharacterTable[i].key;
				*flags |= MOD_SHIFT;
				break;
			}

		static CFMutableDictionaryRef charToCodeDict = NULL;
		CGKeyCode key_code;
		UniChar character = c;
		CFStringRef charStr = NULL;

		/* Generate table of keycodes and characters. */
		if (charToCodeDict == NULL) {
			size_t i;
			charToCodeDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
			                 128,
			                 &kCFCopyStringDictionaryKeyCallBacks,
			                 NULL);
			if (charToCodeDict == NULL) return UINT16_MAX;

			/* Loop through every keycode (0 - 127) to find its current mapping. */
			for (i = 0; i < 128; ++i) {
				CFStringRef string = createStringForKey((CGKeyCode)i);
				if (string != NULL) {
					CFDictionaryAddValue(charToCodeDict, string, (const void *)i);
					CFRelease(string);
				}
			}
		}

		charStr = CFStringCreateWithCharacters(kCFAllocatorDefault, &character, 1);

		/* Our values may be NULL (0), so we need to use this function. */
		if (!CFDictionaryGetValueIfPresent(charToCodeDict, charStr,
		                                   (const void **)&key_code)) {
			key_code = UINT16_MAX; /* Error */
		}

		CFRelease(charStr);
		*code = key_code & 0xff;
		return 0;
	}

	for (i = 0; i < ARRAYSIZE(key_names); i ++)
		if (k == key_names[i].name)
		{
			*code = key_names[i].key;
			return 0;
		}

	return CALL_E_INVALIDARG;
}

static io_connect_t _getAuxiliaryKeyDriver(void)
{
	static mach_port_t sEventDrvrRef = 0;
	mach_port_t masterPort, service, iter;
	kern_return_t kr;

	if (!sEventDrvrRef) {
		kr = IOMasterPort( bootstrap_port, &masterPort );
		assert(KERN_SUCCESS == kr);
		kr = IOServiceGetMatchingServices(masterPort, IOServiceMatching( kIOHIDSystemClass), &iter );
		assert(KERN_SUCCESS == kr);
		service = IOIteratorNext( iter );
		assert(service);
		kr = IOServiceOpen(service, mach_task_self(), kIOHIDParamConnectType, &sEventDrvrRef );
		assert(KERN_SUCCESS == kr);
		IOObjectRelease(service);
		IOObjectRelease(iter);
	}
	return sEventDrvrRef;
}

void toggleKeyCode(int32_t code, const bool down, int32_t flags)
{
	/* The media keys all have 1000 added to them to help us detect them. */
	if (code >= 1000) {
		code = code - 1000; /* Get the real keycode. */
		NXEventData   event;
		kern_return_t kr;
		IOGPoint loc = { 0, 0 };
		UInt32 evtInfo = code << 16 | (down ? NX_KEYDOWN : NX_KEYUP) << 8;
		bzero(&event, sizeof(NXEventData));
		event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
		event.compound.misc.L[0] = evtInfo;
		kr = IOHIDPostEvent( _getAuxiliaryKeyDriver(), NX_SYSDEFINED, loc, &event, kNXEventDataVersion, 0, FALSE );
		assert( KERN_SUCCESS == kr );
	} else {
		CGEventRef keyEvent = CGEventCreateKeyboardEvent(NULL,
		                      (CGKeyCode)code, down);
		assert(keyEvent != NULL);

		CGEventSetType(keyEvent, down ? kCGEventKeyDown : kCGEventKeyUp);
		CGEventSetFlags(keyEvent, CGEventFlags(flags));
		CGEventPost(kCGSessionEventTap, keyEvent);
		CFRelease(keyEvent);
	}
}

result_t CheckKeyFlags(exlib::string f, int32_t* flags)
{
	if (f == "alt")
		*flags |= MOD_ALT;
	else if (f == "command")
		*flags |= MOD_META;
	else if (f == "control")
		*flags |= MOD_CONTROL;
	else if (f == "shift")
		*flags |= MOD_SHIFT;
	else if (f != "")
		return CALL_E_INVALIDARG;

	return 0;
}

result_t CheckKeyFlags(v8::Local<v8::Array> modifier, int32_t* flags)
{
	result_t hr;
	int32_t i, len;
	len = modifier->Length();

	for (i = 0; i < len; i++)
	{
		exlib::string f;
		v8::Local<v8::Value> v = modifier->Get(i);
		hr = GetArgumentValue(v, f);
		if (hr < 0)
			return hr;

		hr = CheckKeyFlags(f, flags);
		if (hr < 0)
			return hr;
	}

	return 0;
}

result_t console_base::keyDown(exlib::string key, exlib::string modifier)
{
	result_t hr;
	int32_t code;
	int32_t flags = MOD_NONE;

	hr = CheckKeyCodes(key, &code, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	hr = CheckKeyFlags(modifier, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	toggleKeyCode(code, true, flags);
	return 0;
}

result_t console_base::keyDown(exlib::string key, v8::Local<v8::Array> modifier)
{
	result_t hr;
	int32_t code;
	int32_t flags = MOD_NONE;

	hr = CheckKeyCodes(key, &code, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	hr = CheckKeyFlags(modifier, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	toggleKeyCode(code, true, flags);
	return 0;
}

result_t console_base::keyUp(exlib::string key, exlib::string modifier)
{
	result_t hr;
	int32_t code;
	int32_t flags = MOD_NONE;

	hr = CheckKeyCodes(key, &code, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	hr = CheckKeyFlags(modifier, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	toggleKeyCode(code, false, flags);
	return 0;
}

result_t console_base::keyUp(exlib::string key, v8::Local<v8::Array> modifier)
{
	result_t hr;
	int32_t code;
	int32_t flags = MOD_NONE;

	hr = CheckKeyCodes(key, &code, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	hr = CheckKeyFlags(modifier, &flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	toggleKeyCode(code, false, flags);
	return 0;
}

result_t console_base::keyTap(exlib::string key, exlib::string modifier)
{
	result_t hr;

	hr = keyDown(key, modifier);
	if (hr < 0)
		return hr;

	hr = keyUp(key, modifier);
	if (hr < 0)
		return hr;

	return 0;
}

result_t console_base::keyTap(exlib::string key, v8::Local<v8::Array> modifier)
{
	result_t hr;

	hr = keyDown(key, modifier);
	if (hr < 0)
		return hr;

	hr = keyUp(key, modifier);
	if (hr < 0)
		return hr;

	return 0;
}

result_t console_base::typeString(exlib::string text)
{
	result_t hr;
	size_t i;

	for (i = 0; i < text.length(); i ++)
	{
		hr = keyTap(text.substr(i, 1), "");
		if (hr < 0)
			return hr;
	}

	return 0;
}

}

#endif