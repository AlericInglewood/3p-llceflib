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
#include "llrenderhandler.h"

class LLCEFLibImpl;
class LLRenderHandler;

class LLBrowserClient :
	public CefClient,
	public CefLifeSpanHandler,
	public CefDisplayHandler,
	public CefLoadHandler,
	public CefRequestHandler
{
    public:
        LLBrowserClient(LLCEFLibImpl* parent, LLRenderHandler* render_handler);

        // point to our CefRenderHandler
		CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;

		// CefLifeSpanHandler overrides
		CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{ return this; }
#if (CEF_CURRENT_BRANCH >= CEF_BRANCH_2357)
		bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, 
			const CefString& target_url, const CefString& target_frame_name, 
			CefLifeSpanHandler::WindowOpenDisposition target_disposition, 
			bool user_gesture, const CefPopupFeatures& popupFeatures, 
			CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, 
			CefBrowserSettings& settings, bool* no_javascript_access) OVERRIDE;
#else
		bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, 
			const CefString& target_url, const CefString& target_frame_name, const 
			CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, 
			CefRefPtr<CefClient>& client, CefBrowserSettings& settings, 
			bool* no_javascript_access) OVERRIDE;
#endif
		void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		bool RunModal(CefRefPtr<CefBrowser> browser) OVERRIDE;
		bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

        // CefDisplayhandler overrides
		CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{ return this; }
		void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
        bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) OVERRIDE;
        void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) OVERRIDE;
        void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

		// CefLoadHandler overrides
		CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{ return this; }
		void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
		void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
		void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

		// CefRequestHandler overrides
		CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE{ return this; }
		bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect) OVERRIDE;
		bool GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback) OVERRIDE;

#if (CEF_CURRENT_BRANCH >= CEF_BRANCH_2357)
		bool OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url, int64 new_size, CefRefPtr<CefRequestCallback> callback) OVERRIDE;
#else
		bool OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url, int64 new_size, CefRefPtr<CefQuotaCallback> callback) OVERRIDE;
#endif

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
