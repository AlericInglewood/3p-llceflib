/**
 * @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
 * @author Henri Beauchamp 2015
 *
 * $LicenseInfo:firstyear=2001&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2015, Linden Research, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "llceflibimpl.h"

#include "llceflib.h"
#include "llceflibplatform.h"

// See: http://src.chromium.org/viewvc/chrome/trunk/src/ui/events/keycodes/keyboard_code_conversion_x.cc

#define VK_BACK     0x16

#define VK_MULTIPLY 0x3F

#define VK_NUMPAD7  0x4F
#define VK_NUMPAD8  0x50
#define VK_NUMPAD9  0x51
#define VK_SUBTRACT 0x52
#define VK_NUMPAD4  0x53
#define VK_NUMPAD5  0x54
#define VK_NUMPAD6  0x55
#define VK_ADD      0x56
#define VK_NUMPAD1  0x57
#define VK_NUMPAD2  0x58
#define VK_NUMPAD3  0x59
#define VK_NUMPAD0  0x5A
#define VK_DECIMAL  0x5B

#define VK_ENTER    0x68

#define VK_DIVIDE   0x6A

#define VK_RETURN   0x6D

#define VK_HOME     0x6E
#define VK_UP       0x6F
#define VK_PRIOR    0x70    // PageUp
#define VK_LEFT     0x71
#define VK_RIGHT    0x72
#define VK_END      0x73
#define VK_DOWN     0x74
#define VK_NEXT     0x75    // PageDown
#define VK_INSERT   0x76
#define VK_DELETE   0x77

#define WK_BACK     0x08
#define WK_PRIOR    0x21
#define WK_NEXT     0x22
#define WK_END      0x23
#define WK_HOME     0x24
#define WK_LEFT     0x25
#define WK_UP       0x26
#define WK_RIGHT    0x27
#define WK_DOWN     0x28
#define WK_INSERT   0x2D
#define WK_DELETE   0x2E

// Not used...
void LLCEFLibImpl::nativeKeyboardEvent(uint32_t msg, uint32_t wparam,
                                       uint64_t lparam)
{
}

void LLCEFLibImpl::keyboardEvent(LLCEFLib::EKeyEvent key_event,
                                 uint32_t key_code,
                                 const char* utf8_text,
                                 LLCEFLib::EKeyboardModifier modifiers,
                                 uint32_t native_scan_code,
                                 uint32_t native_virtual_key,
                                 uint32_t native_modifiers)
{
    if (mBrowser && mBrowser->GetHost())
    {
        CefKeyEvent event;
        event.is_system_key = false;
        event.native_key_code = native_virtual_key;
        // Do not treat DEL and BACKSPACE as characters: they are processed via
        // the event.windows_key_code member
        if (native_virtual_key != 0x7f && native_virtual_key != 0x08)
        {
            event.character = native_virtual_key;
        }
        else
        {
            event.character = 0;
        }
        event.unmodified_character = native_virtual_key;

        event.modifiers = native_modifiers;
        if (native_modifiers & EVENTFLAG_ALT_DOWN)
        {
#if 0
            event.modifiers |= EVENTFLAG_IS_LEFT;
#else
            event.modifiers &= ~EVENTFLAG_ALT_DOWN;
            event.is_system_key = true;
#endif
        }
#if 0
        if ((native_modifiers & EVENTFLAG_SHIFT_DOWN) ||
            (native_modifiers & EVENTFLAG_CONTROL_DOWN))
        {
            event.modifiers |= EVENTFLAG_IS_LEFT;
        }
#endif
        if ((native_scan_code >= VK_NUMPAD7 &&
             native_scan_code <= VK_DECIMAL) ||
            native_scan_code == VK_MULTIPLY || native_scan_code == VK_DIVIDE ||
            native_scan_code == VK_ENTER || native_scan_code == VK_RETURN)
        {
            event.modifiers |= EVENTFLAG_IS_KEY_PAD;
        }

        switch (native_scan_code)
        {
            case VK_BACK:
                event.windows_key_code = WK_BACK;
                break;
            case VK_HOME:
                event.windows_key_code = WK_HOME;
                break;
            case VK_UP:
                event.windows_key_code = WK_UP;
                break;
            case VK_PRIOR:
                event.windows_key_code = WK_PRIOR;
                break;
            case VK_LEFT:
                event.windows_key_code = WK_LEFT;
                break;
            case VK_RIGHT:
                event.windows_key_code = WK_RIGHT;
                break;
            case VK_END:
                event.windows_key_code = WK_END;
                break;
            case VK_DOWN:
                event.windows_key_code = WK_DOWN;
                break;
            case VK_NEXT:
                event.windows_key_code = WK_NEXT;
                break;
            case VK_INSERT:
                event.windows_key_code = WK_INSERT;
                break;
            case VK_DELETE:
                event.windows_key_code = WK_DELETE;
                break;
            default:
                event.windows_key_code = native_scan_code;
        }

        if (key_event == LLCEFLib::KE_KEY_DOWN)
        {
            event.type = KEYEVENT_RAWKEYDOWN;
            mBrowser->GetHost()->SendKeyEvent(event);
        }
        else
        {
            if (key_event == LLCEFLib::KE_KEY_UP)
            {
                event.type = KEYEVENT_KEYUP;
                mBrowser->GetHost()->SendKeyEvent(event);
            }
            if (event.character)
            {
                event.type = KEYEVENT_CHAR;
                mBrowser->GetHost()->SendKeyEvent(event);
            }
        }
    }
}

void LLCEFLibImpl::injectUnicodeText(wchar_t, wchar_t, uint32_t, uint32_t)
{
    // not used for Linux
}

void LLCEFLibImpl::nativeKeyboardEventOSX(void*)
{
    // not used for Linux
}

void LLCEFLibImpl::keyboardEventOSX(unsigned int, unsigned int, char const*,
                                    char const*, bool, unsigned int)
{
    // not used on for Linux
}
