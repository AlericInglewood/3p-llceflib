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

#ifndef _LLBROWSERCLIENT
#define _LLBROWSERCLIENT

#include "include/cef_client.h"

#include <list>

#include "llceflibplatform.h"
#include "llrenderhandler.h"

class LLCEFLibImpl;
class LLRenderHandler;

class LLBrowserClient :
    public CefClient,
    public CefLifeSpanHandler,
    public CefDisplayHandler,
    public CefLoadHandler,
    public CefRequestHandler,
    public CefDownloadHandler,
    public CefDialogHandler
{
    public:
        LLBrowserClient(LLCEFLibImpl* parent, LLRenderHandler* render_handler);

        // point to our CefRenderHandler
        CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;

        // CefLifeSpanHandler overrides
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{ return this; }
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
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
        bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
        void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

        // CefDisplayhandler overrides
        CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{ return this; }
        void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_3538
        bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source, int line) OVERRIDE;
#else
        bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) OVERRIDE;
#endif
        void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) OVERRIDE;
        void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

        // CefLoadHandler overrides
        CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{ return this; }
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_3029   // Actually in cef/188326bbc406c2b7997c723db5ba4f4897a553a0
        void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) OVERRIDE;
#else
        void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
#endif
        void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
        void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

        // CefRequestHandler overrides
        CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE{ return this; }
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_3538
        bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) OVERRIDE;
#else
        bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect) OVERRIDE;
#endif
        bool GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback) OVERRIDE;

#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
        bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                            const CefString& origin_url,
                            int64 new_size,
                            CefRefPtr<CefRequestCallback> callback) OVERRIDE;
#else
        bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                            const CefString& origin_url,
                            int64 new_size,
                            CefRefPtr<CefQuotaCallback> callback) OVERRIDE;
#endif
        // CefDownloadHandler overrides
        CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE { return this; }
        void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefDownloadItem> download_item,
                              const CefString& suggested_name,
                              CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;

        // CefDialogHandler orerrides
        CefRefPtr<CefDialogHandler> GetDialogHandler() OVERRIDE { return this; }
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
        bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                            FileDialogMode mode,
                            const CefString& title,
                            const CefString& default_file_path,
                            const std::vector<CefString>& accept_filters,
                            int selected_accept_filter,
                            CefRefPtr<CefFileDialogCallback> callback) OVERRIDE;
#else
        bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                            FileDialogMode mode,
                            const CefString& title,
                            const CefString& default_file_name,
                            const std::vector<CefString>& accept_types,
                            CefRefPtr<CefFileDialogCallback> callback) OVERRIDE;
#endif

    private:
        LLCEFLibImpl* mParent;
        CefRefPtr<CefRenderHandler> mLLRenderHandler;
        typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
        BrowserList mBrowserList;

    public:
        IMPLEMENT_REFCOUNTING(LLBrowserClient);
};

#endif //_LLBROWSERCLIENT
