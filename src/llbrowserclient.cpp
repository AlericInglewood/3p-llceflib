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

bool LLBrowserClient::GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, 
	const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback)
{
	CEF_REQUIRE_UI_THREAD();

	std::string host_str = host;
	std::string realm_str = realm;
	std::string scheme_str = scheme;

	std::string username="";
	std::string password="";
	bool proceed = mParent->onHTTPAuth(host_str, realm_str, username, password);

	CefRefPtr<LLCEFLibAuthCredentials> cred = new LLCEFLibAuthCredentials(isProxy, host, port, realm, scheme, callback);
	if (proceed)
	{
		cred->proceed(username.c_str(), password.c_str());
		return true; // continue with request
	}
	else
	{
		cred->cancel();
		return false; // cancel request
	}
}

void LLBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
#ifdef LLCEFLIB_DEBUG
	std::cout << "LLBrowserClient::OnBeforeClose - set mIsBrowserClosing = true; " << std::endl;
#endif
	mIsBrowserClosing = true;
}

bool LLBrowserClient::isBrowserClosing()
{
	return mIsBrowserClosing;
}
