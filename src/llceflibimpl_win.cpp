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

bool isKeyDown(WPARAM wparam)
{
	return (GetKeyState(wparam) & 0x8000) != 0;
}

int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam)
{
	int modifiers = 0;
	if (isKeyDown(VK_SHIFT))
		modifiers |= EVENTFLAG_SHIFT_DOWN;
	if (isKeyDown(VK_CONTROL))
		modifiers |= EVENTFLAG_CONTROL_DOWN;
	if (isKeyDown(VK_MENU))
		modifiers |= EVENTFLAG_ALT_DOWN;

	// Low bit set from GetKeyState indicates "toggled".
	if (::GetKeyState(VK_NUMLOCK) & 1)
		modifiers |= EVENTFLAG_NUM_LOCK_ON;
	if (::GetKeyState(VK_CAPITAL) & 1)
		modifiers |= EVENTFLAG_CAPS_LOCK_ON;

	switch (wparam) {
	case VK_RETURN:
		if ((lparam >> 16) & KF_EXTENDED)
			modifiers |= EVENTFLAG_IS_KEY_PAD;
		break;
	case VK_INSERT:
	case VK_DELETE:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		if (!((lparam >> 16) & KF_EXTENDED))
			modifiers |= EVENTFLAG_IS_KEY_PAD;
		break;
	case VK_NUMLOCK:
	case VK_NUMPAD0:
	case VK_NUMPAD1:
	case VK_NUMPAD2:
	case VK_NUMPAD3:
	case VK_NUMPAD4:
	case VK_NUMPAD5:
	case VK_NUMPAD6:
	case VK_NUMPAD7:
	case VK_NUMPAD8:
	case VK_NUMPAD9:
	case VK_DIVIDE:
	case VK_MULTIPLY:
	case VK_SUBTRACT:
	case VK_ADD:
	case VK_DECIMAL:
	case VK_CLEAR:
		modifiers |= EVENTFLAG_IS_KEY_PAD;
		break;
	case VK_SHIFT:
		if (isKeyDown(VK_LSHIFT))
			modifiers |= EVENTFLAG_IS_LEFT;
		else if (isKeyDown(VK_RSHIFT))
			modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	case VK_CONTROL:
		if (isKeyDown(VK_LCONTROL))
			modifiers |= EVENTFLAG_IS_LEFT;
		else if (isKeyDown(VK_RCONTROL))
			modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	case VK_MENU:
		if (isKeyDown(VK_LMENU))
			modifiers |= EVENTFLAG_IS_LEFT;
		else if (isKeyDown(VK_RMENU))
			modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	case VK_LWIN:
		modifiers |= EVENTFLAG_IS_LEFT;
		break;
	case VK_RWIN:
		modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	}
	return modifiers;
}

void LLCEFLibImpl::nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam)
{
	if (mBrowser && mBrowser->GetHost())
	{
		CefKeyEvent event;
		event.windows_key_code = wparam;
		event.native_key_code = (int)lparam;
		event.is_system_key = msg == WM_SYSCHAR || msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP;

		if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
			event.type = KEYEVENT_RAWKEYDOWN;
		else if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
			event.type = KEYEVENT_KEYUP;
		else
			event.type = KEYEVENT_CHAR;

		event.modifiers = GetCefKeyboardModifiers((WPARAM)wparam, (LPARAM)lparam);

		mBrowser->GetHost()->SendKeyEvent(event);
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
	if (mBrowser && mBrowser->GetHost())
	{
		CefKeyEvent event;
		event.modifiers = native_modifiers;
		event.native_key_code = native_virtual_key;
		event.windows_key_code = native_virtual_key;
		event.unmodified_character = native_virtual_key;

		if (key_event == KE_KEY_DOWN)
		{
			event.type = KEYEVENT_RAWKEYDOWN;
			mBrowser->GetHost()->SendKeyEvent(event);

			event.windows_key_code = key_code;
			event.type = KEYEVENT_CHAR;
			mBrowser->GetHost()->SendKeyEvent(event);
		}
		else
		if (key_event == KE_KEY_UP)
		{
			event.type = KEYEVENT_KEYUP;
			mBrowser->GetHost()->SendKeyEvent(event);
		}
	}
}

