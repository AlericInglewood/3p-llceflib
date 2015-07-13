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

#ifndef _LLCEFLIBIMPL
#define _LLCEFLIBIMPL

#include "include/cef_app.h"
#include "llbrowserclient.h"
#include "boost/function.hpp"

#include "llceflib.h"
#include "llceflibplatform.h"

class CefSchemeRegistrar;
struct LLCEFLibSettings;
class LLBrowserClient;

class LLCEFLibImpl :
    public CefApp
{
    public:         // TODO: divide up methods into those uin pImpl interface and not
        LLCEFLibImpl();
        virtual ~LLCEFLibImpl();

        bool init(LLCEFLibSettings& user_settings);
        void update();
        void setSize(int width, int height);
        void getSize(int& width, int& height);
        int getDepth();
        void navigate(std::string url);
		void setPageZoom(double zoom_val);
        void reset();

        void setPageChangedCallback(boost::function<void(unsigned char*, int, int)> callback);

        void pageChanged(unsigned char*, int, int);

        void setOnCustomSchemeURLCallback(boost::function<void(std::string)> callback);
        void onCustomSchemeURL(std::string url);

        void setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback);
        void onConsoleMessage(std::string message, std::string source, int line);

        void setOnStatusMessageCallback(boost::function<void(std::string)> callback);
        void onStatusMessage(std::string value);

        void setOnTitleChangeCallback(boost::function<void(std::string)> callback);
        void onTitleChange(std::string title);

		void setOnLoadStartCallback(boost::function<void()> callback);
		void onLoadStart();

		void setOnLoadEndCallback(boost::function<void(int)> callback);
		void onLoadEnd(int httpStatusCode);

		void setOnNavigateURLCallback(boost::function<void(std::string)> callback);
		void onNavigateURL(std::string url);

		void mouseButton(EMouseButton mouse_button, EMouseEvent mouse_event, int x, int y);
        void mouseMove(int x, int y);

		void nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam);
		void keyboardEvent(
			EKeyEvent key_event,
			uint32_t key_code,
			const char *utf8_text,
			EKeyboardModifier modifiers,
			uint32_t native_scan_code,
			uint32_t native_virtual_key,
			uint32_t native_modifiers);

        void mouseWheel(int deltaY);
        void setFocus(bool focus);

        void stop();
        void reload(bool ignore_cache);
        bool canGoBack();
        void goBack();
        bool canGoForward();
        void goForward();
        bool isLoading();

        /* virtual */
        void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;

    private:
        CefRefPtr<LLBrowserClient> mBrowserClient;
        CefRefPtr<CefBrowser> mBrowser;
        int mViewWidth;
        int mViewHeight;
        const int mViewDepth = 4;
        boost::function<void(unsigned char*, int, int)> mPageChangedCallbackFunc;
        boost::function<void(std::string)> mOnCustomSchemeURLCallbackFunc;
        boost::function<void(std::string, std::string, int line)> mOnConsoleMessageCallbackFunc;
        boost::function<void(std::string)> mOnStatusMessageCallbackFunc;
		boost::function<void(std::string)> mOnTitleChangeCallbackFunc;
		boost::function<void()> mOnLoadStartCallbackFunc;
		boost::function<void(int)> mOnLoadEndCallbackFunc;
		boost::function<void(std::string)> mOnNavigateURLCallbackFunc;

        IMPLEMENT_REFCOUNTING(LLCEFLibImpl);
};

#endif // _LLCEFLIBIMPL
