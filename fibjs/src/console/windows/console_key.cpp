/*
 * console_key.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: lion
 */

#include "object.h"
#include "ifs/console.h"

#ifdef _WIN32

namespace fibjs {

enum {
    K_NOT_A_KEY = 9999,
    K_BACKSPACE = VK_BACK,
    K_DELETE = VK_DELETE,
    K_RETURN = VK_RETURN,
    K_TAB = VK_TAB,
    K_ESCAPE = VK_ESCAPE,
    K_UP = VK_UP,
    K_DOWN = VK_DOWN,
    K_RIGHT = VK_RIGHT,
    K_LEFT = VK_LEFT,
    K_HOME = VK_HOME,
    K_END = VK_END,
    K_PAGEUP = VK_PRIOR,
    K_PAGEDOWN = VK_NEXT,
    K_F1 = VK_F1,
    K_F2 = VK_F2,
    K_F3 = VK_F3,
    K_F4 = VK_F4,
    K_F5 = VK_F5,
    K_F6 = VK_F6,
    K_F7 = VK_F7,
    K_F8 = VK_F8,
    K_F9 = VK_F9,
    K_F10 = VK_F10,
    K_F11 = VK_F11,
    K_F12 = VK_F12,
    K_F13 = VK_F13,
    K_F14 = VK_F14,
    K_F15 = VK_F15,
    K_F16 = VK_F16,
    K_F17 = VK_F17,
    K_F18 = VK_F18,
    K_F19 = VK_F19,
    K_F20 = VK_F20,
    K_F21 = VK_F21,
    K_F22 = VK_F22,
    K_F23 = VK_F23,
    K_F24 = VK_F24,
    K_META = VK_LWIN,
    K_CONTROL = VK_CONTROL,
    K_SHIFT = VK_SHIFT,
    K_RIGHTSHIFT = VK_RSHIFT,
    K_ALT = VK_MENU,
    K_CAPSLOCK = VK_CAPITAL,
    K_SPACE = VK_SPACE,
    K_PRINTSCREEN = VK_SNAPSHOT,
    K_INSERT = VK_INSERT,

    K_NUMPAD_0 = VK_NUMPAD0,
    K_NUMPAD_1 = VK_NUMPAD1,
    K_NUMPAD_2 = VK_NUMPAD2,
    K_NUMPAD_3 = VK_NUMPAD3,
    K_NUMPAD_4 = VK_NUMPAD4,
    K_NUMPAD_5 = VK_NUMPAD5,
    K_NUMPAD_6 = VK_NUMPAD6,
    K_NUMPAD_7 = VK_NUMPAD7,
    K_NUMPAD_8 = VK_NUMPAD8,
    K_NUMPAD_9 = VK_NUMPAD9,

    K_AUDIO_VOLUME_MUTE = VK_VOLUME_MUTE,
    K_AUDIO_VOLUME_DOWN = VK_VOLUME_DOWN,
    K_AUDIO_VOLUME_UP = VK_VOLUME_UP,
    K_AUDIO_PLAY = VK_MEDIA_PLAY_PAUSE,
    K_AUDIO_STOP = VK_MEDIA_STOP,
    K_AUDIO_PAUSE = VK_MEDIA_PLAY_PAUSE,
    K_AUDIO_PREV = VK_MEDIA_PREV_TRACK,
    K_AUDIO_NEXT = VK_MEDIA_NEXT_TRACK,
    K_AUDIO_REWIND = K_NOT_A_KEY,
    K_AUDIO_FORWARD = K_NOT_A_KEY,
    K_AUDIO_REPEAT = K_NOT_A_KEY,
    K_AUDIO_RANDOM = K_NOT_A_KEY,

    K_LIGHTS_MON_UP = K_NOT_A_KEY,
    K_LIGHTS_MON_DOWN = K_NOT_A_KEY,
    K_LIGHTS_KBD_TOGGLE = K_NOT_A_KEY,
    K_LIGHTS_KBD_UP = K_NOT_A_KEY,
    K_LIGHTS_KBD_DOWN = K_NOT_A_KEY
};

enum {
    MOD_NONE = 0,
    /* These are already defined by the Win32 API */
    /* MOD_ALT = 0,
	MOD_CONTROL = 0,
	MOD_SHIFT = 0, */
    MOD_META = MOD_WIN
};

struct KeyNames {
    const char* name;
    int32_t key;
};

static KeyNames key_names[] = {
    { "backspace", K_BACKSPACE },
    { "delete", K_DELETE },
    { "enter", K_RETURN },
    { "tab", K_TAB },
    { "escape", K_ESCAPE },
    { "up", K_UP },
    { "down", K_DOWN },
    { "right", K_RIGHT },
    { "left", K_LEFT },
    { "home", K_HOME },
    { "end", K_END },
    { "pageup", K_PAGEUP },
    { "pagedown", K_PAGEDOWN },
    { "f1", K_F1 },
    { "f2", K_F2 },
    { "f3", K_F3 },
    { "f4", K_F4 },
    { "f5", K_F5 },
    { "f6", K_F6 },
    { "f7", K_F7 },
    { "f8", K_F8 },
    { "f9", K_F9 },
    { "f10", K_F10 },
    { "f11", K_F11 },
    { "f12", K_F12 },
    { "f13", K_F13 },
    { "f14", K_F14 },
    { "f15", K_F15 },
    { "f16", K_F16 },
    { "f17", K_F17 },
    { "f18", K_F18 },
    { "f19", K_F19 },
    { "f20", K_F20 },
    { "f21", K_F21 },
    { "f22", K_F22 },
    { "f23", K_F23 },
    { "f24", K_F24 },
    { "command", K_META },
    { "alt", K_ALT },
    { "control", K_CONTROL },
    { "shift", K_SHIFT },
    { "right_shift", K_RIGHTSHIFT },
    { "space", K_SPACE },
    { "printscreen", K_PRINTSCREEN },
    { "insert", K_INSERT },

    { "audio_mute", K_AUDIO_VOLUME_MUTE },
    { "audio_vol_down", K_AUDIO_VOLUME_DOWN },
    { "audio_vol_up", K_AUDIO_VOLUME_UP },
    { "audio_play", K_AUDIO_PLAY },
    { "audio_stop", K_AUDIO_STOP },
    { "audio_pause", K_AUDIO_PAUSE },
    { "audio_prev", K_AUDIO_PREV },
    { "audio_next", K_AUDIO_NEXT },
    { "audio_rewind", K_AUDIO_REWIND },
    { "audio_forward", K_AUDIO_FORWARD },
    { "audio_repeat", K_AUDIO_REPEAT },
    { "audio_random", K_AUDIO_RANDOM },

    { "numpad_0", K_NUMPAD_0 },
    { "numpad_1", K_NUMPAD_1 },
    { "numpad_2", K_NUMPAD_2 },
    { "numpad_3", K_NUMPAD_3 },
    { "numpad_4", K_NUMPAD_4 },
    { "numpad_5", K_NUMPAD_5 },
    { "numpad_6", K_NUMPAD_6 },
    { "numpad_7", K_NUMPAD_7 },
    { "numpad_8", K_NUMPAD_8 },
    { "numpad_9", K_NUMPAD_9 },

    { "lights_mon_up", K_LIGHTS_MON_UP },
    { "lights_mon_down", K_LIGHTS_MON_DOWN },
    { "lights_kbd_toggle", K_LIGHTS_KBD_TOGGLE },
    { "lights_kbd_up", K_LIGHTS_KBD_UP },
    { "lights_kbd_down", K_LIGHTS_KBD_DOWN }
};

struct XSpecialCharacterMapping {
    char name;
    int32_t key;
};

struct XSpecialCharacterMapping XSpecialCharacterTable[] = {
    { '\n', K_RETURN }
};

result_t CheckKeyCodes(exlib::string k, int32_t* code, int32_t* flags)
{
    size_t i;

    if (k.length() == 1) {
        char c = k[0];
        for (i = 0; i < ARRAYSIZE(XSpecialCharacterTable); i++)
            if (c == XSpecialCharacterTable[i].name) {
                *code = XSpecialCharacterTable[i].key;
                return 0;
            }

        *code = VkKeyScan(c);
        int modifiers = *code >> 8;
        if ((modifiers & 1) != 0)
            *flags |= MOD_SHIFT;
        if ((modifiers & 2) != 0)
            *flags |= MOD_CONTROL;
        if ((modifiers & 4) != 0)
            *flags |= MOD_ALT;
        *code = *code & 0xff;
        return 0;
    }

    for (i = 0; i < ARRAYSIZE(key_names); i++)
        if (k == key_names[i].name) {
            *code = key_names[i].key;
            return 0;
        }

    return CALL_E_INVALIDARG;
}

void win32KeyEvent(int key, int32_t flags)
{
    int scan = MapVirtualKey(key & 0xff, MAPVK_VK_TO_VSC);

    /* Set the scan code for extended keys */
    switch (key) {
    case VK_RCONTROL:
    case VK_SNAPSHOT: /* Print Screen */
    case VK_RMENU: /* Right Alt / Alt Gr */
    case VK_PAUSE: /* Pause / Break */
    case VK_HOME:
    case VK_UP:
    case VK_PRIOR: /* Page up */
    case VK_LEFT:
    case VK_RIGHT:
    case VK_END:
    case VK_DOWN:
    case VK_NEXT: /* 'Page Down' */
    case VK_INSERT:
    case VK_DELETE:
    case VK_LWIN:
    case VK_RWIN:
    case VK_APPS: /* Application */
    case VK_VOLUME_MUTE:
    case VK_VOLUME_DOWN:
    case VK_VOLUME_UP:
    case VK_MEDIA_NEXT_TRACK:
    case VK_MEDIA_PREV_TRACK:
    case VK_MEDIA_STOP:
    case VK_MEDIA_PLAY_PAUSE:
    case VK_BROWSER_BACK:
    case VK_BROWSER_FORWARD:
    case VK_BROWSER_REFRESH:
    case VK_BROWSER_STOP:
    case VK_BROWSER_SEARCH:
    case VK_BROWSER_FAVORITES:
    case VK_BROWSER_HOME:
    case VK_LAUNCH_MAIL: {
        flags |= KEYEVENTF_EXTENDEDKEY;
        break;
    }
    }

    /* Set the scan code for keyup */
    if (flags & KEYEVENTF_KEYUP) {
        scan |= 0x80;
    }

    keybd_event(key, scan, flags, 0);
}

void toggleKeyCode(int32_t code, const bool down, int32_t flags)
{
    const DWORD dwFlags = down ? 0 : KEYEVENTF_KEYUP;

    /* Parse modifier keys. */
    if (flags & MOD_META)
        win32KeyEvent(K_META, dwFlags);
    if (flags & MOD_ALT)
        win32KeyEvent(K_ALT, dwFlags);
    if (flags & MOD_CONTROL)
        win32KeyEvent(K_CONTROL, dwFlags);
    if (flags & MOD_SHIFT)
        win32KeyEvent(K_SHIFT, dwFlags);

    win32KeyEvent(code, dwFlags);
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

    for (i = 0; i < len; i++) {
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

    for (i = 0; i < text.length(); i++) {
        hr = keyTap(text.substr(i, 1), "");
        if (hr < 0)
            return hr;
    }

    return 0;
}
}

#endif