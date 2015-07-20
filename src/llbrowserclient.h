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

#ifndef _LLBROWSERCLIENT
#define _LLBROWSERCLIENT

#include "include/cef_client.h"

#include "llceflibplatform.h"
#include "llbrowserevents.h"

class LLCEFLibImpl;
class LLRenderHandler;
#include "llrenderhandler.h"

class LLBrowserClient :
	public LLBrowserEvents
{
    public:
        LLBrowserClient(LLCEFLibImpl* parent, LLRenderHandler* render_handler);

        /* virtual */
		CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;

        // LLBrowserEvents/CefDisplayhandler overrides
        bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) OVERRIDE;
        void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) OVERRIDE;
        void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

		// LLBrowserEvents/CefLoadHandler overrides
		void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
		void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
		void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

		// LLBrowserEvents/CefRequestHandler overrides
		bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect) OVERRIDE;

		// CefLifeSpanHandler overrides
		void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

		// utility methods
		bool isBrowserClosing();

    private:
        LLCEFLibImpl* mParent;
        CefRefPtr<CefRenderHandler> mLLRenderHandler;
		bool mIsBrowserClosing;

    public:
        IMPLEMENT_REFCOUNTING(LLBrowserClient);
};

#endif //_LLBROWSERCLIENT
