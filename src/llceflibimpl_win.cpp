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

void LLCEFLibImpl::keyPress(int code, bool is_down)
{
	if (mBrowser)
	{
		if (mBrowser->GetHost())
		{
			CefKeyEvent event;
			event.is_system_key = false;
			event.modifiers = 0;
            
			unsigned char VkCode = (unsigned char)(VkKeyScanA(code));
			unsigned int scanCode = MapVirtualKey(VkCode, MAPVK_VK_TO_VSC);
			event.native_key_code = (scanCode << 16) |  // key scan code
				1;  // key repeat count
			event.windows_key_code = VkCode;

			if (is_down)
			{

				event.type = KEYEVENT_RAWKEYDOWN;
				mBrowser->GetHost()->SendKeyEvent(event);

				event.windows_key_code = code;
				event.type = KEYEVENT_CHAR;
				mBrowser->GetHost()->SendKeyEvent(event);
			}
			else
			{
				// bits 30 and 31 should be always 1 for WM_KEYUP
				event.native_key_code |= 0xC0000000;

				event.type = KEYEVENT_KEYUP;
				mBrowser->GetHost()->SendKeyEvent(event);
			}
		}
	}
}