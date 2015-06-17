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

// Mac OS doesn't have the same keyboard support functions as Windows.
// We can remove this functionality once we use a non GLUT event loop using NSEvent
//
// NSEvent will allow us to fetch the the correct virtual key code. Until then, the
// code below will have to do.

#define GLUT_UNDEFINED          (-1)
#define GLUT_KEY_DELETE         (8)
#define GLUT_KEY_ENTER          (13)
#define GLUT_KEY_ESCAPE         (27)
#define GLUT_KEY_BACKSPACE      (127)

// See: const KeyCodeMap kKeyCodesMap[] in
// http://src.chromium.org/viewvc/chrome/trunk/src/ui/events/keycodes/keyboard_code_conversion_mac.mm
// for the Cnative key values
#define NATIVE_KEY_DELETE         (0x75)
#define NATIVE_KEY_ENTER          (0x0D)
#define NATIVE_KEY_ESCAPE         (0x1B)
#define NATIVE_KEY_BACKSPACE      (0x33)

// This namespace should be temporary, can probably be removed in final version
// of llceflib.
namespace LLCEFLibImplMacAssist
{
    int  _glutToNativeKey(int code)
    {
        switch(code)
        {
            case GLUT_KEY_DELETE:
                return NATIVE_KEY_DELETE;
                
            //case GLUT_KEY_ENTER:
            //    return NATIVE_KEY_ENTER;
                
            //case GLUT_KEY_ESCAPE:
            //    return NATIVE_KEY_ESCAPE;
                
            case GLUT_KEY_BACKSPACE:
                return NATIVE_KEY_BACKSPACE;
        }
        
        return GLUT_UNDEFINED;
    }
    
    bool _isSpecialGlutKey(int code)
    {
        switch(code)
        {
            case GLUT_KEY_DELETE:
            //case GLUT_KEY_ENTER:
            //case GLUT_KEY_ESCAPE:
            case GLUT_KEY_BACKSPACE:
                return true;
        }
        
        return false;
    }
}

/* LLCEFLibImpl Mac methods below */
void LLCEFLibImpl::keyPress(int code, bool is_down)
{
	if (mBrowser)
	{
		if (mBrowser->GetHost())
		{
            // Ignore sending the key when we're dealing with special
            // keys, such as backspace and delete
            if(LLCEFLibImplMacAssist::_isSpecialGlutKey(code))
            {
                int virtualCode = LLCEFLibImplMacAssist::_glutToNativeKey(code);
                if(virtualCode != GLUT_UNDEFINED && is_down)
                {
                    CefKeyEvent event;
                    event.character = 0;
                    event.unmodified_character = 0;
                    event.native_key_code = virtualCode;
                    event.modifiers = 0;
                    event.type = KEYEVENT_KEYDOWN;
                    
                    mBrowser->GetHost()->SendKeyEvent(event);
                }
                
                return;
            }

            CefKeyEvent event;
            event.is_system_key = false;
            event.modifiers = 0;
            event.character = code;
            
            if(is_down)
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
