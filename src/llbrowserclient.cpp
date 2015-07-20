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

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"

#include "llrenderhandler.h"
#include "llbrowserclient.h"
#include "llbrowserevents.h"

#include "llceflibimpl.h"

LLBrowserClient::LLBrowserClient(LLCEFLibImpl* parent, LLRenderHandler *render_handler) :
    mParent(parent),
    mLLRenderHandler(render_handler),
	mIsBrowserClosing(false)
{
}

CefRefPtr<CefRenderHandler> LLBrowserClient::GetRenderHandler()
{
    return mLLRenderHandler;
}

bool LLBrowserClient::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
{
    CEF_REQUIRE_UI_THREAD();

    std::string message_str = message;
    std::string source_str = source;
    mParent->onConsoleMessage(message_str, source_str, line);

    return true;
}

void LLBrowserClient::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
    CEF_REQUIRE_UI_THREAD();

    std::string value_str = value;
    mParent->onStatusMessage(value_str);
}

void LLBrowserClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();

    std::string title_str = title;
    mParent->onTitleChange(title_str);
}

void LLBrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();
	if (frame->IsMain())
		mParent->onLoadStart();
}

void LLBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();
	if (frame->IsMain())
		mParent->onLoadEnd(httpStatusCode);
}

void LLBrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
}

bool LLBrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect)
{
	CEF_REQUIRE_UI_THREAD();
	std::string url = request->GetURL();

	mParent->onNavigateURL(url);

	// continue with navigation
	return false;
}

void LLBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	mIsBrowserClosing = true;
}

bool LLBrowserClient::isBrowserClosing()
{
	return mIsBrowserClosing;
}