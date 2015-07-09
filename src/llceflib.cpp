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

#include "llceflib.h"
#include "llceflibimpl.h"

LLCEFLib::LLCEFLib()
    :mImpl(new LLCEFLibImpl())
{
}

LLCEFLib::~LLCEFLib()
{

}

bool LLCEFLib::init(LLCEFLibSettings& user_settings)
{
    return mImpl->init(user_settings);
}

void LLCEFLib::update()
{
    mImpl->update();
}

void LLCEFLib::setSize(int width, int height)
{
    mImpl->setSize(width, height);
}

void LLCEFLib::getSize(int& width, int& height)
{
    mImpl->getSize(width, height);
}

void LLCEFLib::navigate(std::string url)
{
    mImpl->navigate(url);
}

void LLCEFLib::setPageZoom(double zoom_val)
{
	mImpl->setPageZoom(zoom_val);
}

void LLCEFLib::setPageChangedCallback(boost::function<void(unsigned char*, int, int)> callback)
{
    mImpl->setPageChangedCallback(callback);
}

void LLCEFLib::setOnCustomSchemeURLCallback(boost::function<void(std::string url)> callback)
{
    mImpl->setOnCustomSchemeURLCallback(callback);
}

void LLCEFLib::setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback)
{
    mImpl->setOnConsoleMessageCallback(callback);
}

void LLCEFLib::setOnStatusMessageCallback(boost::function<void(std::string value)> callback)
{
    mImpl->setOnStatusMessageCallback(callback);
}

void LLCEFLib::setOnTitleChangeCallback(boost::function<void(std::string title)> callback)
{
	mImpl->setOnTitleChangeCallback(callback);
}

void LLCEFLib::setOnLoadStartCallback(boost::function<void()> callback)
{
	mImpl->setOnLoadStartCallback(callback);
}

void LLCEFLib::setOnLoadEndCallback(boost::function<void(int)> callback)
{
	mImpl->setOnLoadEndCallback(callback);
}

void LLCEFLib::setOnNavigateURLCallback(boost::function<void(std::string title)> callback)
{
	mImpl->setOnNavigateURLCallback(callback);
}

void LLCEFLib::reset()
{
    mImpl->reset();
}

void LLCEFLib::mouseButton(EMouseButton mouse_button, EMouseEvent mouse_event, int x, int y)

{
    mImpl->mouseButton(mouse_button, mouse_event, x, y);
}

void LLCEFLib::mouseMove(int x, int y)
{
    mImpl->mouseMove(x, y);
}

void LLCEFLib::nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam) 
{
	mImpl->nativeKeyboardEvent(msg, wparam, lparam);
}

void LLCEFLib::keyboardEvent(
	EKeyEvent key_event,
	uint32_t key_code,
	const char *utf8_text,
	EKeyboardModifier modifiers,
	uint32_t native_scan_code,
	uint32_t native_virtual_key,
	uint32_t native_modifiers)
{
	mImpl->keyboardEvent(key_event, key_code, utf8_text, modifiers, native_scan_code, native_virtual_key, native_modifiers);
}

void LLCEFLib::mouseWheel(int deltaY)
{
    mImpl->mouseWheel(deltaY);
}

void LLCEFLib::setFocus(bool focus)
{
    mImpl->setFocus(focus);
}

void LLCEFLib::stop()
{
    mImpl->stop();
}

void LLCEFLib::reload(bool ignore_cache)
{
    mImpl->reload(ignore_cache);
}

bool LLCEFLib::canGoBack()
{
    return mImpl->canGoBack();
}

void LLCEFLib::goBack()
{
    mImpl->goBack();
}

bool LLCEFLib::canGoForward()
{
    return mImpl->canGoForward();
}

void LLCEFLib::goForward()
{
    mImpl->goForward();
}

bool LLCEFLib::isLoading()
{
    return mImpl->isLoading();
}
