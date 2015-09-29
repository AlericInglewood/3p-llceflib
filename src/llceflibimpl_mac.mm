/**
 * @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
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
#include "llCEFLib.h"

#include "llceflibplatform.h"
#include "llschemehandler.h"

// See: const KeyCodeMap kKeyCodesMap[] in
// http://src.chromium.org/viewvc/chrome/trunk/src/ui/events/keycodes/keyboard_code_conversion_mac.mm
// for the Cnative key values
#define NATIVE_KEY_BACKSPACE      (0x33)

#define ASCII_KEY_BACKSPACE       (0x7F)
#define ASCII_KEY_ENTER           (0x0D)
#define ASCII_KEY_UNDEFINED       (-1)

#define SL_KEY_BACKSPACE          (0x87)
#define SL_KEY_ENTER              (0x81)

namespace LLCEFLibImplMacAssist
{
    int asciiToNativeKey(int code)
    {
        switch(code)
        {
            case ASCII_KEY_BACKSPACE:
                return NATIVE_KEY_BACKSPACE;
        }

        return ASCII_KEY_UNDEFINED;
    }

    bool isSpecialKey(int code)
    {
        switch(code)
        {
            case ASCII_KEY_BACKSPACE:
                return true;
        }

        return false;
    }

    int slToASCIIKey(int sl_code)
    {
        switch(sl_code)
        {
            case SL_KEY_ENTER:
                return ASCII_KEY_ENTER;

            case SL_KEY_BACKSPACE:
                return ASCII_KEY_BACKSPACE;
        }

        return sl_code;
    }
}

void LLCEFLibImpl::keyboardEvent(
    EKeyEvent key_event,
    uint32_t key_code,
    const char *utf8_text,
    EKeyboardModifier modifiers,
    uint32_t native_scan_code,
    uint32_t native_virtual_key,
    uint32_t native_modifiers)
{
    if (mBrowser)
    {
        if (mBrowser->GetHost())
        {
            key_code = LLCEFLibImplMacAssist::slToASCIIKey(key_code);

            if(LLCEFLibImplMacAssist::isSpecialKey(key_code))
            {
                int native_key_code = LLCEFLibImplMacAssist::asciiToNativeKey(key_code);
                if(native_key_code != ASCII_KEY_UNDEFINED && key_event == KE_KEY_DOWN)
                {
                    CefKeyEvent event;
                    event.character = 0;
                    event.unmodified_character = 0;
                    event.native_key_code = native_key_code;
                    event.modifiers = 0;
                    event.type = KEYEVENT_KEYDOWN;

                    mBrowser->GetHost()->SendKeyEvent(event);
                }

                return;
            }

            CefKeyEvent event;
            event.is_system_key = false;
            event.modifiers = 0;
            event.character = key_code;

            if(key_event == KE_KEY_DOWN)
            {
                event.type = KEYEVENT_KEYDOWN;
                mBrowser->GetHost()->SendKeyEvent(event);

                event.type = KEYEVENT_CHAR;
                mBrowser->GetHost()->SendKeyEvent(event);
            }
            else
            {
                event.type = KEYEVENT_KEYUP;
                mBrowser->GetHost()->SendKeyEvent(event);
            }
        }
    }
}


void LLCEFLibImpl::setPlatformCursor(CefCursorHandle cursor_handle)
{
	// not implemented for OS X yet
}


void LLCEFLibImpl::nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam)
{
    // not implemented for OS X yet - may only be useful for Windows version
}
