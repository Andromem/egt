/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/keycode.h"
#include "detail/egtlog.h"

#ifdef HAVE_LINUX_INPUT_H
#include <linux/input.h>
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

static constexpr std::pair<int, KeyboardCode> keys[] =
{
#ifdef HAVE_LINUX_INPUT_H
    //{KEY_UNKNOWN, EKEY_UNKNOWN},
    {KEY_BACKSPACE, EKEY_BACKSPACE},
    {KEY_TAB, EKEY_TAB},
    {KEY_CLEAR, EKEY_CLEAR},
    {KEY_ENTER, EKEY_ENTER},
    {KEY_MENU, EKEY_MENU},
    {KEY_PAUSE, EKEY_PAUSE},
    {KEY_CAPSLOCK, EKEY_CAPSLOCK},
    {KEY_ESC, EKEY_ESCAPE},
    {KEY_NEXT, EKEY_NEXT},
    {KEY_SPACE, EKEY_SPACE},
    {KEY_PAGEUP, EKEY_PAGEUP},
    {KEY_PAGEDOWN, EKEY_PAGEDOWN},
    {KEY_END, EKEY_END},
    {KEY_HOME, EKEY_HOME},
    {KEY_LEFT, EKEY_LEFT},
    {KEY_UP, EKEY_UP},
    {KEY_RIGHT, EKEY_RIGHT},
    {KEY_DOWN, EKEY_DOWN},
    {KEY_SELECT, EKEY_SELECT},
    {KEY_PRINT, EKEY_PRINT},
    {KEY_SYSRQ, EKEY_SNAPSHOT},
    {KEY_INSERT, EKEY_INSERT},
    {KEY_DELETE, EKEY_DELETE},
    {KEY_HELP, EKEY_HELP},
    {KEY_0, EKEY_0},
    {KEY_1, EKEY_1},
    {KEY_2, EKEY_2},
    {KEY_3, EKEY_3},
    {KEY_4, EKEY_4},
    {KEY_5, EKEY_5},
    {KEY_6, EKEY_6},
    {KEY_7, EKEY_7},
    {KEY_8, EKEY_8},
    {KEY_9, EKEY_9},
    {KEY_A, EKEY_A},
    {KEY_B, EKEY_B},
    {KEY_C, EKEY_C},
    {KEY_D, EKEY_D},
    {KEY_E, EKEY_E},
    {KEY_F, EKEY_F},
    {KEY_G, EKEY_G},
    {KEY_H, EKEY_H},
    {KEY_I, EKEY_I},
    {KEY_J, EKEY_J},
    {KEY_K, EKEY_K},
    {KEY_L, EKEY_L},
    {KEY_M, EKEY_M},
    {KEY_N, EKEY_N},
    {KEY_O, EKEY_O},
    {KEY_P, EKEY_P},
    {KEY_Q, EKEY_Q},
    {KEY_R, EKEY_R},
    {KEY_S, EKEY_S},
    {KEY_T, EKEY_T},
    {KEY_U, EKEY_U},
    {KEY_V, EKEY_V},
    {KEY_W, EKEY_W},
    {KEY_X, EKEY_X},
    {KEY_Y, EKEY_Y},
    {KEY_Z, EKEY_Z},
    {KEY_SLEEP, EKEY_SLEEP},
    {KEY_KP0, EKEY_NUMPAD0},
    {KEY_KP1, EKEY_NUMPAD1},
    {KEY_KP2, EKEY_NUMPAD2},
    {KEY_KP3, EKEY_NUMPAD3},
    {KEY_KP4, EKEY_NUMPAD4},
    {KEY_KP5, EKEY_NUMPAD5},
    {KEY_KP6, EKEY_NUMPAD6},
    {KEY_KP7, EKEY_NUMPAD7},
    {KEY_KP8, EKEY_NUMPAD8},
    {KEY_KP9, EKEY_NUMPAD9},
    {KEY_KPASTERISK, EKEY_MULTIPLY},
    {KEY_KPPLUS, EKEY_ADD},
    {KEY_KPENTER, EKEY_KPENTER},
    {KEY_KPMINUS, EKEY_SUBTRACT},
    {KEY_KPDOT, EKEY_DECIMAL},
    {KEY_KPSLASH, EKEY_DIVIDE},
    {KEY_F1, EKEY_F1},
    {KEY_F2, EKEY_F2},
    {KEY_F3, EKEY_F3},
    {KEY_F4, EKEY_F4},
    {KEY_F5, EKEY_F5},
    {KEY_F6, EKEY_F6},
    {KEY_F7, EKEY_F7},
    {KEY_F8, EKEY_F8},
    {KEY_F9, EKEY_F9},
    {KEY_F10, EKEY_F10},
    {KEY_F11, EKEY_F11},
    {KEY_F12, EKEY_F12},
    {KEY_F13, EKEY_F13},
    {KEY_F14, EKEY_F14},
    {KEY_F15, EKEY_F15},
    {KEY_F16, EKEY_F16},
    {KEY_F17, EKEY_F17},
    {KEY_F18, EKEY_F18},
    {KEY_F19, EKEY_F19},
    {KEY_F20, EKEY_F20},
    {KEY_F21, EKEY_F21},
    {KEY_F22, EKEY_F22},
    {KEY_F23, EKEY_F23},
    {KEY_F24, EKEY_F24},
    {KEY_NUMLOCK, EKEY_NUMLOCK},
    {KEY_LEFTSHIFT, EKEY_LSHIFT},
    {KEY_RIGHTSHIFT, EKEY_RSHIFT},
    {KEY_LEFTCTRL, EKEY_LCONTROL},
    {KEY_RIGHTCTRL, EKEY_RCONTROL},
    {KEY_LEFTALT, EKEY_LEFTALT},
    {KEY_RIGHTALT, EKEY_RIGHTALT},
    {KEY_MUTE, EKEY_VOLUME_MUTE},
    {KEY_VOLUMEDOWN, EKEY_VOLUME_DOWN},
    {KEY_VOLUMEUP, EKEY_VOLUME_UP},
    {KEY_KPPLUS, EKEY_PLUS },
    {KEY_COMMA, EKEY_COMMA },
    {KEY_MINUS, EKEY_MINUS },
    {KEY_DOT, EKEY_DOT },
    {KEY_KPDOT, EKEY_NUMDOT  },
    {KEY_SLASH, EKEY_SLASH  },
    {KEY_SEMICOLON, EKEY_SEMICOLON },
    {KEY_APOSTROPHE, EKEY_APOSTROPHE },
    {KEY_LEFTBRACE, EKEY_LEFTBRACE },
    {KEY_RIGHTBRACE, EKEY_RIGHTBRACE },
    {KEY_EQUAL, EKEY_EQUAL },
    {KEY_GRAVE, EKEY_GRAVE },
    {KEY_BACKSLASH, EKEY_BACKSLASH},
    {KEY_PLAY, EKEY_PLAY},
    {KEY_ZOOM, EKEY_ZOOM},
    {KEY_WLAN, EKEY_WLAN},
    {KEY_POWER, EKEY_POWER},
    {KEY_COMPOSE, EKEY_COMPOSE},
#endif
    // non-standard key code, used by some DTS gpio_keys
    {0x104, EKEY_USER1},
};

KeyboardCode linux_to_ekey(int code)
{
    for (const auto& i : keys)
    {
        if (i.first == code)
        {
            EGTLOG_TRACE("key {} to keycode {}", code, i.second);
            return i.second;
        }
    }

    EGTLOG_DEBUG("unable to map key {} to keycode", code);

    return EKEY_UNKNOWN;
}

}
}
}
