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

#if (CEF_CURRENT_BRANCH >= CEF_BRANCH_2357)
bool LLBrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	const CefString& target_frame_name,
	CefLifeSpanHandler::WindowOpenDisposition target_disposition,
	bool user_gesture,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	bool* no_javascript_access)
#else
bool LLBrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	const CefString& target_frame_name,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	bool* no_javascript_access)
#endif
{
	CEF_REQUIRE_IO_THREAD();

#ifdef LLCEFLIB_DEBUG
	std::cout << "LLBrowserClient::OnBeforePopup" << std::endl;
	std::cout << "Target frame is " << std::string(target_frame_name) << std::endl;
#endif

	// links with a target will not be browsed to - up to caller to kick off 
	// another navigate event if they want to visit that URL
	std::string url = std::string(target_url);
	std::string target = std::string(target_frame_name);
	if (target.length())
	{
		mParent->onNavigateURL(url, target);
		return true;
	}

	browser->GetMainFrame()->LoadURL(target_url);

	return true;
}

void LLBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

#ifdef LLCEFLIB_DEBUG
	std::cout << "LLBrowserClient::OnAfterCreated" << std::endl;
#endif
}

bool LLBrowserClient::RunModal(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

#ifdef LLCEFLIB_DEBUG
	std::cout << "LLBrowserClient::RunModal" << std::endl;
#endif

	return false;
}

bool LLBrowserClient::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

#ifdef LLCEFLIB_DEBUG
	std::cout << "LLBrowserClient::DoClose" << std::endl;
#endif
	return false;
}

bool LLBrowserClient::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
{
    CEF_REQUIRE_UI_THREAD();

    std::string message_str = message;
    std::string source_str = source;
    mParent->onConsoleMessage(message_str, source_str, line);

    return true;
}

void LLBrowserClient::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	CEF_REQUIRE_UI_THREAD();

	std::string new_url = url;
	mParent->onAddressChange(new_url);
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

	// for conmparison
	std::transform(url.begin(), url.end(), url.begin(), ::tolower);

	std::vector<std::string>::iterator iter = mParent->getCustomSchemes().begin();
	while (iter != mParent->getCustomSchemes().end())
	{
		if (url.substr(0, (*iter).length()) == (*iter))
		{
			// get URL again since we lower cased it for comparison
			url = request->GetURL();
			mParent->onCustomSchemeURL(url);

			// don't continute with navigation
			return true;
		}

		++iter;
	}

	// might think this is the right approach to trigger a callback to say we're navigating 
	// but this causes a catastophic loop in SL where it sends a navigate request when it
	// gets this callback.  For the moment, until i can unravel all that, I'm just not sending it here.
	// It is send for links with a target.
	//std::string link_target("");
	//mParent->onNavigateURL(url, link_target);

	// continue with navigation
	return false;
}

bool LLBrowserClient::GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, 
	const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback)
{
	CEF_REQUIRE_IO_THREAD();

	std::string host_str = host;
	std::string realm_str = realm;
	std::string scheme_str = scheme;

	std::string username="";
	std::string password="";
	bool proceed = mParent->onHTTPAuth(host_str, realm_str, username, password);

	if (proceed)
	{
		callback->Continue(username.c_str(), password.c_str());
		return true; // continue with request
	}
	else
	{
		callback->Cancel();
		return false; // cancel request
	}
}

bool LLBrowserClient::OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url,
	int64 new_size, CefRefPtr<CefRequestCallback> callback)
{
	CEF_REQUIRE_IO_THREAD();

	static const int64 max_size = 1024 * 1024 * 5;  // 5mb.

	callback->Continue(new_size <= max_size);
	return true;
}

void LLBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

#ifdef LLCEFLIB_DEBUG
	std::cout << "LLBrowserClient::OnBeforeClose - set mIsBrowserClosing = true; " << std::endl;
#endif
	mIsBrowserClosing = true;
}

bool LLBrowserClient::isBrowserClosing()
{
	return mIsBrowserClosing;
}
