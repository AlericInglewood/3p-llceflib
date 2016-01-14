/**
 * @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
 * @author Callum Prentice 2015
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
    : mImpl(new LLCEFLibImpl())
{
}

LLCEFLib::~LLCEFLib()
{
}

bool LLCEFLib::init(LLCEFLib::LLCEFLibSettings& user_settings)
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
    if (url.length() > 0)
    {
        mImpl->navigate(url);
    }
}

void LLCEFLib::postData(std::string url, std::string data, std::string headers)
{
    if (url.length() > 0)
    {
        mImpl->postData(url, data, headers);
    }
}

bool LLCEFLib::setCookie(std::string url, std::string name, std::string value, std::string domain, std::string path, bool httponly, bool secure)
{
    if (url.length() > 0)
    {
        return mImpl->setCookie(url, name, value, domain, path);
    }

    return false;
}

void LLCEFLib::setPageZoom(double zoom_val)
{
    mImpl->setPageZoom(zoom_val);
}

void LLCEFLib::setOnPageChangedCallback(boost::function<void(unsigned char*, int, int, int, int, bool)> callback)
{
    mImpl->setOnPageChangedCallback(callback);
}

void LLCEFLib::setOnCustomSchemeURLCallback(boost::function<void(std::string url)> callback)
{
    mImpl->setOnCustomSchemeURLCallback(callback);
}

void LLCEFLib::setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback)
{
    mImpl->setOnConsoleMessageCallback(callback);
}

void LLCEFLib::setOnAddressChangeCallback(boost::function<void(std::string value)> callback)
{
    mImpl->setOnAddressChangeCallback(callback);
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

void LLCEFLib::setOnRequestExitCallback(boost::function<void()> callback)
{
    mImpl->setOnRequestExitCallback(callback);
}

void LLCEFLib::setOnCursorChangedCallback(boost::function<void(LLCEFLib::ECursorType type, unsigned int)> callback)
{
    mImpl->setOnCursorChangedCallback(callback);
}

void LLCEFLib::setOnLoadEndCallback(boost::function<void(int)> callback)
{
    mImpl->setOnLoadEndCallback(callback);
}

void LLCEFLib::setOnNavigateURLCallback(boost::function<void(std::string url, std::string target)> callback)
{
    mImpl->setOnNavigateURLCallback(callback);
}

void LLCEFLib::setOnHTTPAuthCallback(boost::function<bool(const std::string host, const std::string realm, std::string& username, std::string& password)> callback)
{
    mImpl->setOnHTTPAuthCallback(callback);
}

void LLCEFLib::setOnFileDownloadCallback(boost::function<void(const std::string filename)> callback)
{
    mImpl->setOnFileDownloadCallback(callback);
}

void LLCEFLib::setCustomSchemes(std::vector<std::string> custom_schemes)
{
    mImpl->setCustomSchemes(custom_schemes);
}

void LLCEFLib::requestExit()
{
    mImpl->requestExit();
}

void LLCEFLib::shutdown()
{
    mImpl->shutdown();
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

void LLCEFLib::keyboardEventOSX(uint32_t eventType, uint32_t modifiers, const char* characters,
                                const char* unmodCharacters, bool repeat, uint32_t keyCode)
{
    mImpl->keyboardEventOSX(eventType, modifiers, characters, unmodCharacters, repeat, keyCode);
}

void LLCEFLib::injectUnicodeText(wchar_t unicodeChars, wchar_t unmodChars, uint32_t keyCode, uint32_t modifiers)
{
    mImpl->injectUnicodeText(unmodChars, unmodChars, keyCode, modifiers);
}

void LLCEFLib::nativeKeyboardEventOSX(void* nsEvent)
{
    mImpl->nativeKeyboardEventOSX(nsEvent);
}

void LLCEFLib::keyboardEvent(
    EKeyEvent key_event,
    uint32_t key_code,
    const char* utf8_text,
    EKeyboardModifier modifiers,
    uint32_t native_scan_code,
    uint32_t native_virtual_key,
    uint32_t native_modifiers)
{
    mImpl->keyboardEvent(key_event, key_code, utf8_text, modifiers, native_scan_code, native_virtual_key, native_modifiers);
}

void LLCEFLib::mouseWheel(int deltaX, int deltaY)
{
    mImpl->mouseWheel(deltaX, deltaY);
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

bool LLCEFLib::editCanCopy()
{
    return mImpl->editCanCopy();
}

bool LLCEFLib::editCanCut()
{
    return mImpl->editCanCut();
}

bool LLCEFLib::editCanPaste()
{
    return mImpl->editCanPaste();
}

void LLCEFLib::editCopy()
{
    mImpl->editCopy();
}

void LLCEFLib::editCut()
{
    mImpl->editCut();
}

void LLCEFLib::editPaste()
{
    mImpl->editPaste();
}

void LLCEFLib::showDevTools(bool show)
{
    mImpl->showDevTools(show);
}

std::string LLCEFLib::makeCompatibleUserAgentString(const std::string base)
{
    return mImpl->makeCompatibleUserAgentString(base);
}
