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

#include "llbrowserevents.h"
#include "include/wrapper/cef_helpers.h"

/* virtual */
#if (CEF_CURRENT_BRANCH >= CEF_BRANCH_2357)
bool LLBrowserEvents::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
bool LLBrowserEvents::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
    std::cout << "LLBrowserEvents::OnBeforePopup" << std::endl;
#endif
    browser->GetMainFrame()->LoadURL(target_url);

    return true;
}

/* virtual */
void LLBrowserEvents::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

#ifdef LLCEFLIB_DEBUG
    std::cout << "LLBrowserEvents::OnAfterCreated" << std::endl;
#endif
}

/* virtual */
bool LLBrowserEvents::RunModal(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

#ifdef LLCEFLIB_DEBUG
    std::cout << "LLBrowserEvents::RunModal" << std::endl;
#endif

    return false;
}

/* virtual */
bool LLBrowserEvents::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

#ifdef LLCEFLIB_DEBUG
    std::cout << "LLBrowserEvents::DoClose" << std::endl;
#endif
    return false;
}

/* virtual */
void LLBrowserEvents::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
#ifdef LLCEFLIB_DEBUG
	std::cout << "LLBrowserEvents::OnBeforeClose" << std::endl;
#endif
}

bool LLBrowserEvents::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
{
    CEF_REQUIRE_UI_THREAD();
    return true;
}

void LLBrowserEvents::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
    CEF_REQUIRE_UI_THREAD();
}

void LLBrowserEvents::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();
}

bool LLBrowserEvents::OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text)
{
    CEF_REQUIRE_UI_THREAD();
    return true;
}

void LLBrowserEvents::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
}

void LLBrowserEvents::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
}

void LLBrowserEvents::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
}

bool LLBrowserEvents::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect)
{
	CEF_REQUIRE_UI_THREAD();
	return true;
}

bool LLBrowserEvents::GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback)
{
	CEF_REQUIRE_IO_THREAD();
	return true;
}
