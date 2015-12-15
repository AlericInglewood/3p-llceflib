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
class LLContextHandler;


class FlushStoreCallback:
    public CefCompletionCallback
{
    public:
        void OnComplete() OVERRIDE
        {
        }

        IMPLEMENT_REFCOUNTING(FlushStoreCallback);
};

class LLCEFLibImpl :
    public CefApp
{
    public:
        LLCEFLibImpl();
        virtual ~LLCEFLibImpl();

        // CefApp overrides
        virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;

        bool init(LLCEFLib::LLCEFLibSettings& user_settings);
        void update();
        void setSize(int width, int height);
        void getSize(int& width, int& height);
        int getDepth();
        void navigate(std::string url);
        void postData(std::string url, std::string data, std::string headers);
        void setCookie(std::string url, std::string name, std::string value, std::string domain, std::string path);
        void setPageZoom(double zoom_val);
        void requestExit();
        void shutdown();

        void setOnPageChangedCallback(boost::function<void(unsigned char*, int, int, int, int, bool)> callback);
        void onPageChanged(unsigned char*, int, int, int, int, bool);

        void setOnCustomSchemeURLCallback(boost::function<void(std::string)> callback);
        void onCustomSchemeURL(std::string url);

        void setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback);
        void onConsoleMessage(std::string message, std::string source, int line);

        void setOnAddressChangeCallback(boost::function<void(std::string)> callback);
        void onAddressChange(std::string new_url);

        void setOnStatusMessageCallback(boost::function<void(std::string)> callback);
        void onStatusMessage(std::string value);

        void setOnTitleChangeCallback(boost::function<void(std::string)> callback);
        void onTitleChange(std::string title);

        void OnBeforeClose(CefRefPtr<CefBrowser> browser);

        void setOnLoadStartCallback(boost::function<void()> callback);
        void onLoadStart();

        void setOnLoadEndCallback(boost::function<void(int)> callback);
        void onLoadEnd(int httpStatusCode);

        void setOnNavigateURLCallback(boost::function<void(std::string, std::string)> callback);
        void onNavigateURL(std::string url, std::string target);

		void setOnHTTPAuthCallback(boost::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> callback);
		bool onHTTPAuth(const std::string host, const std::string realm, std::string& username, std::string& password);

		void setOnFileDownloadCallback(boost::function<void(const std::string filename)> callback);
		void onFileDownload(const std::string filename);

		void setOnRequestExitCallback(boost::function<void()> callback);
        void onRequestExit();

        void setOnCursorChangedCallback(boost::function<void(LLCEFLib::ECursorType type, unsigned int cursor)> callback);
        void onCursorChanged(LLCEFLib::ECursorType type, unsigned int cursor);

        void mouseButton(LLCEFLib::EMouseButton mouse_button, LLCEFLib::EMouseEvent mouse_event, int x, int y);
        void mouseMove(int x, int y);
        void nativeMouseEvent(uint32_t msg, uint32_t wparam, uint64_t lparam);

        void keyboardEventOSX(uint32_t eventType, uint32_t modifiers, const char* characters, const char* unmodChars, bool repeat, uint32_t keyCode);

        void nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam);
        void injectUnicodeText(wchar_t unicodeChars, wchar_t unmodChars, uint32_t keyCode, uint32_t modifiers);

        void nativeKeyboardEventOSX(void* nsEvent);
        void keyboardEvent(
            LLCEFLib::EKeyEvent key_event,
            uint32_t key_code,
            const char* utf8_text,
            LLCEFLib::EKeyboardModifier modifiers,
            uint32_t native_scan_code,
            uint32_t native_virtual_key,
            uint32_t native_modifiers);

        void mouseWheel(int deltaX, int deltaY);
        void setFocus(bool focus);

        void stop();
        void reload(bool ignore_cache);
        bool canGoBack();
        void goBack();
        bool canGoForward();
        void goForward();
        bool isLoading();

        bool editCanCopy();
        bool editCanCut();
        bool editCanPaste();
        void editCopy();
        void editCut();
        void editPaste();

        void setCustomSchemes(std::vector<std::string> custom_schemes);
        std::vector<std::string>& getCustomSchemes();

        void showDevTools(bool show);

        /* virtual */
        void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;

        CefRefPtr<CefBrowser> getBrowser();
        void setBrowser(CefRefPtr<CefBrowser> browser);

        std::string makeCompatibleUserAgentString(const std::string base);

    private:
        CefRefPtr<LLBrowserClient> mBrowserClient;
        CefRefPtr<CefBrowser> mBrowser;
        CefRefPtr<LLContextHandler> mContextHandler;
        int mViewWidth;
        int mViewHeight;
        const int mViewDepth = 4;
        bool mSystemFlashEnabled;
        std::vector<std::string> mCustomSchemes;
        boost::function<void(unsigned char*, int, int, int, int, bool)> mOnPageChangedCallbackFunc;
        boost::function<void(std::string)> mOnCustomSchemeURLCallbackFunc;
        boost::function<void(std::string, std::string, int line)> mOnConsoleMessageCallbackFunc;
        boost::function<void(std::string)> mOnAddressChangeCallbackFunc;
        boost::function<void(std::string)> mOnStatusMessageCallbackFunc;
        boost::function<void(std::string)> mOnTitleChangeCallbackFunc;
        boost::function<void()> mOnLoadStartCallbackFunc;
        boost::function<void(int)> mOnLoadEndCallbackFunc;
        boost::function<void(std::string, std::string)> mOnNavigateURLCallbackFunc;
		boost::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> mOnHTTPAuthCallbackFunc;
		boost::function<void(const std::string filename)> mOnFileDownloadCallbackFunc;
        boost::function<void()> mOnRequestExitCallbackFunc;
        boost::function<void(LLCEFLib::ECursorType type, unsigned int cursor)> mOnCursorChangedCallbackFunc;

        CefRefPtr<FlushStoreCallback> mFlushStoreCallback;

        IMPLEMENT_REFCOUNTING(LLCEFLibImpl);
};

#endif // _LLCEFLIBIMPL
