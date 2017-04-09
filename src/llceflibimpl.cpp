/**
 * @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
 * @author Callum Prentice 2015
 * - Improvements and Linux support by Henri Beauchamp (<CV:HB>).
 * - Updated April 2017 to upstream version by Aleric (<SV:AI>).
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

#ifdef __linux__
//#include <cstdlib>
//#include <sys/types.h>
//#include <sys/stat.h>
#endif

#include "llceflibimpl.h"
#include "llceflib.h"           // Must be lowercase <SV:AI>

#include "llceflibplatform.h"

#include "llschemehandler.h"
#include "llrenderhandler.h"
#include "llbrowserclient.h"
#include "llcontexthandler.h"

#include "include/cef_runnable.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

// <CV:HB>
#include <iostream>             // Needed when mDebug is set.
// </CV:HB>

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif

LLCEFLibImpl::LLCEFLibImpl() :
    mViewWidth(0),
    mViewHeight(0),
    mBrowser(0),
	mRequestedZoom(0.0),
    mSystemFlashEnabled(false),
    mMediaStreamEnabled(false),
    mDebug(false)               // Added <CV:HB>
{
    // default is second life scheme
    std::vector<std::string> default_schemes;
    default_schemes.push_back("secondlife://"); // bah - clang doesn't like the explicit initialization form
    mCustomSchemes = default_schemes;

    mFlushStoreCallback = new FlushStoreCallback();
}

LLCEFLibImpl::~LLCEFLibImpl()
{
}

void LLCEFLibImpl::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    if (process_type.empty())
    {
        command_line->AppendSwitch("disable-surfaces");     // for PDF files

        if (mMediaStreamEnabled == true)                    // for webcam/media access
        {
            command_line->AppendSwitch("enable-media-stream");
        }

        if (mSystemFlashEnabled == true)                    // for Flash
        {
            command_line->AppendSwitch("enable-system-flash");
            // <CV:HB>
#ifdef __linux__
            if (getenv("LL_FLASH_PLUGIN") && getenv("LL_FLASH_VERSION"))
            {
                std::string flash_plugin = getenv("LL_FLASH_PLUGIN");
                std::string flash_version = getenv("LL_FLASH_VERSION");
                if (!flash_plugin.empty() && !flash_version.empty())
                {
                    command_line->AppendSwitchWithValue("ppapi-flash-path", flash_plugin);
                    command_line->AppendSwitchWithValue("ppapi-flash-version", flash_version);
                }
            }
#endif
            // </CV:HB>
        }

#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
        // <FS:ND> Add enable-begin-frame-scheduling to synchronize frame rate between all CEF child processes from Drake Arconis/Alchemy
        // speculative/hopefull fix for https://jira.secondlife.com/browse/BUG-11265
        command_line->AppendSwitch("enable-begin-frame-scheduling"); // Synchronize the frame rate between all processes.
        // </FS:ND>
#endif
    }
}

bool LLCEFLibImpl::init(LLCEFLib::LLCEFLibSettings& user_settings)
{
    // <CV:HB>
    mDebug = user_settings.debug;
    if (mDebug)
    {
        std::cerr << "LLCEFLibImpl::init() starting..." << std::endl;
    }
    // </CV:HB>

#ifdef WIN32
    CefMainArgs args(GetModuleHandle(NULL));
#else
    CefMainArgs args(0, NULL);
#endif

    CefSettings settings;

#if defined(WIN32)
    CefString(&settings.browser_subprocess_path) = "llceflib_host.exe";
#elif defined(__APPLE__)
    NSString* appBundlePath = [[NSBundle mainBundle] bundlePath];
    CefString(&settings.browser_subprocess_path) = [[NSString stringWithFormat: @"%@/Contents/Frameworks/LLCefLib Helper.app/Contents/MacOS/LLCefLib Helper", appBundlePath] UTF8String];
#elif defined(__linux__)
    CefString(&settings.browser_subprocess_path) = "llceflib_host";
#endif

    // <CV:HB>
    // Do not bother with the stupid sandbox which keeps failing under Windows
    // and which must be set UID root under Linux...
    settings.no_sandbox = true;
    // </CV:HB>

    // change settings based on what was passed in
    // Only change user agent if user wants to
    if (user_settings.user_agent_substring.length())
    {
        std::string user_agent(user_settings.user_agent_substring);
        cef_string_utf8_to_utf16(user_agent.c_str(), user_agent.size(), &settings.product_version);
    }

    // <CV:HB>
    std::string locale = user_settings.locale;
    cef_string_utf8_to_utf16(locale.c_str(), locale.size(), &settings.locale);
    // </CV:HB>

    // list of language locale codes used to configure the Accept-Language HTTP header value
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
    std::string accept_language_list(user_settings.accept_language_list);
    cef_string_utf8_to_utf16(accept_language_list.c_str(), accept_language_list.size(), &settings.accept_language_list);
#endif

    // set path to cache if enabled and set
    if (user_settings.cache_enabled && user_settings.cache_path.length())
    {
        CefString(&settings.cache_path) = user_settings.cache_path;
    }

    mSystemFlashEnabled = user_settings.plugins_enabled;
    mMediaStreamEnabled = user_settings.media_stream_enabled;

#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
    // Works for Windows 7 and newer, NOP for other OSes.
    CefEnableHighDPISupport();
#endif

    bool result = CefInitialize(args, settings, this, NULL);
    if (! result)
    {
        return false;
    }

    // removed for now since the scheme handler as implemented is broken in rev 2357
    // now schemes are caught via onBeforeBrowse override - when 2357 is fixed the code
    // should revert to using a scheme hander vs a URL parser
    //scheme_handler::RegisterSchemeHandlers(this);

    setSize(user_settings.initial_width, user_settings.initial_height);

    CefWindowInfo window_info;
    window_info.windowless_rendering_enabled = true;

    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 60; // 30 is default - hook into LL media scheduling system?
    browser_settings.webgl = STATE_ENABLED;

    // change settings based on what was passed in
    browser_settings.javascript = user_settings.javascript_enabled ? STATE_ENABLED : STATE_DISABLED;
    browser_settings.plugins = STATE_ENABLED;

	// set page zoom (won't be acted up until later but tha'ts okay)
    mRequestedZoom = user_settings.page_zoom_factor;

    // <CV:HB>
    if (!user_settings.preferred_font.empty())
    {
        CefString(&browser_settings.standard_font_family) = user_settings.preferred_font;
        CefString(&browser_settings.serif_font_family) = user_settings.preferred_font;
        CefString(&browser_settings.sans_serif_font_family) = user_settings.preferred_font;
    }

    if (user_settings.minimum_font_size)
    {
        browser_settings.minimum_logical_font_size = user_settings.minimum_font_size;
        browser_settings.minimum_font_size = user_settings.minimum_font_size;
    }
    browser_settings.remote_fonts = user_settings.remote_fonts ? STATE_ENABLED : STATE_DISABLED;
    // </CV:HB>

    // CEF handler classes
    LLRenderHandler* renderHandler = new LLRenderHandler(this);
    mBrowserClient = new LLBrowserClient(this, renderHandler);

    // if this is NULL for CreateBrowserSync, the global request context will be used
    CefRefPtr<CefRequestContext> rc = NULL;

    // Add a custom context handler that implements a
    // CookieManager so cookies will persist to disk.
    // (if cookies enabled)
    if (user_settings.cookies_enabled)
    {
#ifdef WIN32
        std::string cookiePath = ".\\cookies";
#else
        std::string cookiePath = "./cookies";
#endif
        if (user_settings.cookie_store_path.length())
        {
            cookiePath = std::string(user_settings.cookie_store_path);
        }

        mContextHandler = new LLContextHandler(cookiePath.c_str());

#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
        CefRequestContextSettings contextSettings;
        if (user_settings.cache_enabled && user_settings.cache_path.length())
        {
            CefString(&contextSettings.cache_path) = user_settings.cache_path;
        }
        rc = CefRequestContext::CreateContext(contextSettings, mContextHandler.get());
#else
        rc = CefRequestContext::CreateContext(mContextHandler);
#endif
    }

    // <CV:HB>
    if (mDebug)
    {
        std::cerr << "LLCEFLibImpl::init() creating browser..." << std::endl;
    }
    // </CV:HB>
    CefString url = "";
    mBrowser = CefBrowserHost::CreateBrowserSync(window_info, mBrowserClient.get(), url, browser_settings, rc);

    // <CV:HB>
    if (mDebug)
    {
        if (mBrowser.get() != nullptr)
        {
            std::cerr << "LLCEFLibImpl::init() success." << std::endl;
        }
        else
        {
            std::cerr << "LLCEFLibImpl::init() failure !." << std::endl;
        }
    }
    // </CV:HB>

    return mBrowser.get() != nullptr;   // Return false when LLCEFLibImpl::init failed <CV:HB>.
}

void LLCEFLibImpl::update()
{
    CefDoMessageLoopWork();
}

void LLCEFLibImpl::shutdown()
{
#if defined(WIN32) || defined(__linux__)
	CefShutdown();
#elif defined(__APPLE__)
	// CefShutdown(); 
	// remove for now - the very old version of CEF on OS X does 
	// not shut down cleanly with this in place. Once we switch to
	// 64 bit libs/viewer and update CEF to match Windows version, will enable.
#endif
}

void LLCEFLibImpl::setOnPageChangedCallback(boost::function<void(unsigned char*, int, int, int, int, bool)> callback)
{
    mOnPageChangedCallbackFunc = callback;
}

void LLCEFLibImpl::setOnCustomSchemeURLCallback(boost::function<void(std::string)> callback)
{
    mOnCustomSchemeURLCallbackFunc = callback;
}

void LLCEFLibImpl::setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback)
{
    mOnConsoleMessageCallbackFunc = callback;
}

void LLCEFLibImpl::setOnAddressChangeCallback(boost::function<void(std::string)> callback)
{
    mOnAddressChangeCallbackFunc = callback;
}

void LLCEFLibImpl::setOnStatusMessageCallback(boost::function<void(std::string)> callback)
{
    mOnStatusMessageCallbackFunc = callback;
}

void LLCEFLibImpl::setOnTitleChangeCallback(boost::function<void(std::string)> callback)
{
    mOnTitleChangeCallbackFunc = callback;
}

void LLCEFLibImpl::setOnLoadStartCallback(boost::function<void()> callback)
{
    mOnLoadStartCallbackFunc = callback;
}

void LLCEFLibImpl::setOnRequestExitCallback(boost::function<void()> callback)
{
    mOnRequestExitCallbackFunc = callback;
}

void LLCEFLibImpl::setOnCursorChangedCallback(boost::function<void(LLCEFLib::ECursorType type, unsigned int)> callback)
{
    mOnCursorChangedCallbackFunc = callback;
}

void LLCEFLibImpl::setOnLoadEndCallback(boost::function<void(int)> callback)
{
    mOnLoadEndCallbackFunc = callback;
}

void LLCEFLibImpl::setOnNavigateURLCallback(boost::function<void(std::string, std::string)> callback)
{
    mOnNavigateURLCallbackFunc = callback;
}

void LLCEFLibImpl::setOnHTTPAuthCallback(boost::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> callback)
{
    mOnHTTPAuthCallbackFunc = callback;
}

void LLCEFLibImpl::setOnFileDownloadCallback(boost::function<void(const std::string filename)> callback)
{
    mOnFileDownloadCallbackFunc = callback;
}

void LLCEFLibImpl::setOnFileDialogCallback(boost::function<const std::string()> callback)
{
    mOnFileDialogCallbackFunc = callback;
}

void LLCEFLibImpl::setSize(int width, int height)
{
    mViewWidth = width;
    mViewHeight = height;

    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->WasResized();
    }
}

void LLCEFLibImpl::getSize(int& width, int& height)
{
    width = mViewWidth;
    height = mViewHeight;
}

void LLCEFLibImpl::onPageChanged(unsigned char* pixels, int x, int y, int width, int height, bool is_popup)
{
    if (mOnPageChangedCallbackFunc)
    {
        mOnPageChangedCallbackFunc(pixels, x, y, width, height, is_popup);
    }
}

void LLCEFLibImpl::onCustomSchemeURL(std::string url)
{
    if (mOnCustomSchemeURLCallbackFunc)
    {
        mOnCustomSchemeURLCallbackFunc(url);
    }
}

void LLCEFLibImpl::onConsoleMessage(std::string message, std::string source, int line)
{
    if (mOnConsoleMessageCallbackFunc)
    {
        mOnConsoleMessageCallbackFunc(message, source, line);
    }
}

void LLCEFLibImpl::onAddressChange(std::string new_url)
{
    if (mOnAddressChangeCallbackFunc)
    {
        mOnAddressChangeCallbackFunc(new_url);
    }
}

void LLCEFLibImpl::onStatusMessage(std::string value)
{
    if (mOnStatusMessageCallbackFunc)
    {
        mOnStatusMessageCallbackFunc(value);
    }
}

void LLCEFLibImpl::onTitleChange(std::string title)
{
    if (mOnTitleChangeCallbackFunc)
    {
        mOnTitleChangeCallbackFunc(title);
    }
}

void LLCEFLibImpl::onLoadStart()
{
	if (mBrowser && mBrowser->GetHost())
	{
		double cur_zoom = convertZoomLevel(mBrowser->GetHost()->GetZoomLevel());
		if (fabs(convertZoomLevel(mRequestedZoom)) - fabs(cur_zoom) > 0.001)
		{
			mBrowser->GetHost()->SetZoomLevel(convertZoomLevel(mRequestedZoom));
		};
	}

    if (mOnLoadStartCallbackFunc)
    {
        mOnLoadStartCallbackFunc();
    }
}

void LLCEFLibImpl::onLoadEnd(int httpStatusCode)
{
    if (mOnLoadEndCallbackFunc)
    {
        mOnLoadEndCallbackFunc(httpStatusCode);
    }
}

void LLCEFLibImpl::onNavigateURL(std::string url, std::string target)
{
    if (mOnNavigateURLCallbackFunc)
    {
        mOnNavigateURLCallbackFunc(url, target);
    }
}

void LLCEFLibImpl::onRequestExit()
{
    if (mOnRequestExitCallbackFunc)
    {
        mOnRequestExitCallbackFunc();
    }
}

void LLCEFLibImpl::onCursorChanged(LLCEFLib::ECursorType type, unsigned int cursor)
{
    if (mOnCursorChangedCallbackFunc)
    {
        mOnCursorChangedCallbackFunc(type, cursor);
    }
}

bool LLCEFLibImpl::onHTTPAuth(const std::string host, const std::string realm, std::string& username, std::string& password)
{
    if (mOnHTTPAuthCallbackFunc)
    {
        return mOnHTTPAuthCallbackFunc(host, realm, username, password);
    }

    return false;
}


void LLCEFLibImpl::onFileDownload(const std::string filename)
{
    if (mOnFileDownloadCallbackFunc)
    {
        mOnFileDownloadCallbackFunc(filename);
    }
}

const std::string LLCEFLibImpl::onFileDialog()
{
    if (mOnFileDialogCallbackFunc)
    {
        return mOnFileDialogCallbackFunc();
    }

    return std::string();
}

int LLCEFLibImpl::getDepth()
{
    return mViewDepth;
}

void LLCEFLibImpl::navigate(std::string url)
{
    if (mBrowser && mBrowser->GetMainFrame() && url.length() > 0)
    {
        mBrowser->GetMainFrame()->LoadURL(url);
    }
}

void LLCEFLibImpl::postData(std::string url, std::string data, std::string headers)
{
    if (mBrowser)
    {
        if (mBrowser->GetMainFrame())
        {
            CefRefPtr<CefRequest> request = CefRequest::Create();

            request->SetURL(url);
            request->SetMethod("POST");

            // TODO - get this from the headers parameter
            CefRequest::HeaderMap headerMap;
            headerMap.insert(
                std::make_pair("Accept", "*/*"));
            headerMap.insert(
                std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
            request->SetHeaderMap(headerMap);

            const std::string& upload_data = data;
            CefRefPtr<CefPostData> postData = CefPostData::Create();
            CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
            element->SetToBytes(upload_data.size(), upload_data.c_str());
            postData->AddElement(element);
            request->SetPostData(postData);

            mBrowser->GetMainFrame()->LoadRequest(request);
        }
    }
}

void LLCEFLibImpl::setCookie(std::string url, std::string name, std::string value, std::string domain, std::string path)
{
#if CEF_CURRENT_BRANCH <= CEF_BRANCH_2171
    // CEF 2171 SetCookie() needs to run on IO thread
    if (! CefCurrentlyOn(TID_IO))
    {
        CefPostTask(TID_IO, base::Bind(&LLCEFLibImpl::setCookie, this, url, name, value, domain, path));
        return;
    }
#endif

    // <CV:HB>
    if (!mContextHandler) return;
    // </CV:HB>

    CefRefPtr<CefCookieManager> manager = mContextHandler->GetCookieManager();
    // <CV:HB>
    if (!manager) return;
    // </CV:HB>

    CefCookie cookie;
    CefString(&cookie.name) = name;
    CefString(&cookie.value) = value;
    CefString(&cookie.domain) = domain;
    CefString(&cookie.path) = path;
    cookie.httponly = true;     // IMPORTANT: these 2 fields are always set to true for now and do
    cookie.secure = true;       // do not use the parameters from llceflib.h because of limitation of CEF::Bind() # params

    // TODO set from input
    cookie.has_expires = true;
    cookie.expires.year = 2064;
    cookie.expires.month = 4;
    cookie.expires.day_of_week = 5;
    cookie.expires.day_of_month = 10;

#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
    bool result = manager->SetCookie(url, cookie, nullptr);
    manager->FlushStore(nullptr);
#else
    bool result = manager->SetCookie(url, cookie);
    manager->FlushStore(mFlushStoreCallback);
#endif
}

void LLCEFLibImpl::setPageZoom(double zoom_val)
{
    if (mBrowser && mBrowser->GetHost())
    {
		mBrowser->GetHost()->SetZoomLevel(convertZoomLevel(zoom_val));
		mRequestedZoom = zoom_val;
    }
}

void LLCEFLibImpl::mouseButton(LLCEFLib::EMouseButton mouse_button, LLCEFLib::EMouseEvent mouse_event, int x, int y)
{
	// modify coords based on rules (Y flipped, scaled etc.)
	convertInputCoords(x, y);

    // select click location
    CefMouseEvent cef_mouse_event;
    cef_mouse_event.x = x;
    cef_mouse_event.y = y;
    cef_mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

    // select button
    CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
    if (mouse_button == LLCEFLib::MB_MOUSE_BUTTON_RIGHT)
    {
        btnType = MBT_RIGHT;
        // <CV:HB>
        cef_mouse_event.modifiers = EVENTFLAG_RIGHT_MOUSE_BUTTON;
        // </CV:HB>
    }
    if (mouse_button == LLCEFLib::MB_MOUSE_BUTTON_MIDDLE)
    {
        btnType = MBT_MIDDLE;
        // <CV:HB>
        cef_mouse_event.modifiers = EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        // </CV:HB>
    }

    // TODO: set this properly
    int last_click_count = 1;

    // action TODO: extend to include "move" although this might be enough
    bool is_down = false;
    if (mouse_event == LLCEFLib::ME_MOUSE_DOWN)
    {
        is_down = true;
    }
    else if (mouse_event == LLCEFLib::ME_MOUSE_UP)
    {
        is_down = false;
    }
    else if (mouse_event == LLCEFLib::ME_MOUSE_DOUBLE_CLICK)
    {
        is_down = true;
        last_click_count = 2;
    }

    // send to CEF
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->SendMouseClickEvent(cef_mouse_event, btnType, is_down ? false : true, last_click_count);
    }
};

void LLCEFLibImpl::mouseMove(int x, int y)
{
	// modify coords based on rules (Y flipped, scaled etc.)
	convertInputCoords(x, y);

    CefMouseEvent mouse_event;
    mouse_event.x = x;
    mouse_event.y = y;
    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->SendMouseMoveEvent(mouse_event, false);
    }
};

void LLCEFLibImpl::mouseWheel(int deltaX, int deltaY)
{
    if (mBrowser && mBrowser->GetHost())
    {
        CefMouseEvent mouse_event;
        mouse_event.modifiers = 0;
        mBrowser->GetHost()->SendMouseWheelEvent(mouse_event, deltaX, deltaY);
    }
}

void LLCEFLibImpl::setFocus(bool focus)
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GetHost()->SendFocusEvent(focus);
    }
}

void LLCEFLibImpl::requestExit()
{
    if (mContextHandler && mContextHandler->GetCookieManager())
    {
        mContextHandler->GetCookieManager()->FlushStore(mFlushStoreCallback);
    }

    if (mBrowser && mBrowser->GetHost())
    {
        bool force_close = false;
        mBrowser->GetHost()->CloseBrowser(force_close);
    }
}

void LLCEFLibImpl::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
    // removed for now since the scheme handler as implemented is broken in rev 2357
    // now schemes are caught via onBeforeBrowse override - when 2357 is fixed the code
    // should revert to using a scheme hander vs a URL parser
    //scheme_handler::RegisterCustomSchemes(registrar);
}

void LLCEFLibImpl::setCustomSchemes(std::vector<std::string> custom_schemes)
{
    mCustomSchemes = custom_schemes;
}

std::vector<std::string>& LLCEFLibImpl::getCustomSchemes()
{
    return mCustomSchemes;
}

void LLCEFLibImpl::stop()
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->StopLoad();
    }
}

void LLCEFLibImpl::reload(bool ignore_cache)
{
    if (mBrowser && mBrowser->GetHost())
    {
        if (ignore_cache)
        {
            mBrowser->ReloadIgnoreCache();
        }
        else
        {
            mBrowser->Reload();
        }
    }
}

bool LLCEFLibImpl::canGoBack()
{
    if (mBrowser && mBrowser->GetHost())
    {
        return mBrowser->CanGoBack();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

void LLCEFLibImpl::goBack()
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GoBack();
    }
}

bool LLCEFLibImpl::canGoForward()
{
    if (mBrowser && mBrowser->GetHost())
    {
        return mBrowser->CanGoForward();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

void LLCEFLibImpl::goForward()
{
    if (mBrowser && mBrowser->GetHost())
    {
        mBrowser->GoForward();
    }
}

bool LLCEFLibImpl::isLoading()
{
    if (mBrowser && mBrowser->GetHost())
    {
        return mBrowser->IsLoading();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

bool LLCEFLibImpl::editCanCopy()
{
    // TODO: ask CEF if we can do this
    return true;
}

bool LLCEFLibImpl::editCanCut()
{
    // TODO: ask CEF if we can do this
    return true;
}

bool LLCEFLibImpl::editCanPaste()
{
    // TODO: ask CEF if we can do this
    return true;
}

void LLCEFLibImpl::editCopy()
{
    if (mBrowser && mBrowser->GetFocusedFrame())
    {
        mBrowser->GetFocusedFrame()->Copy();
    }
}

void LLCEFLibImpl::editCut()
{
    if (mBrowser && mBrowser->GetFocusedFrame())
    {
        mBrowser->GetFocusedFrame()->Cut();
    }
}

void LLCEFLibImpl::editPaste()
{
    if (mBrowser && mBrowser->GetFocusedFrame())
    {
        mBrowser->GetFocusedFrame()->Paste();
    }
}

void LLCEFLibImpl::showDevTools(bool show)
{
    if (mBrowser.get() && mBrowser->GetHost())
    {
        if (show)
        {
            CefWindowInfo window_info;
            window_info.x = 0;
            window_info.y = 0;
            window_info.width = 400;
            window_info.height = 400;
#ifdef WIN32
            window_info.SetAsPopup(NULL, "LLCEFLib Dev Tools");
#endif
            CefRefPtr<CefClient> client = mBrowserClient;
            CefBrowserSettings browser_settings;
            CefPoint inspect_element_at;
            mBrowser->GetHost()->ShowDevTools(window_info, client, browser_settings, inspect_element_at);
        }
        else
        {
            mBrowser->GetHost()->CloseDevTools();
        }
    }
}

CefRefPtr<CefBrowser> LLCEFLibImpl::getBrowser()
{
    return mBrowser;
}

void LLCEFLibImpl::setBrowser(CefRefPtr<CefBrowser> browser)
{
    mBrowser = browser;
}

std::string LLCEFLibImpl::makeCompatibleUserAgentString(const std::string base)
{
    std::string frag = "(" + base + ")" + " Chrome/";
#if defined(WIN32)
    frag += CEF_CHROME_VERSION_WIN;
#elif defined(__APPLE__)
    frag += CEF_CHROME_VERSION_OSX;
#elif defined(__linux__)
    frag += CEF_CHROME_VERSION_LIN;
#endif
    return frag;
}

void LLCEFLibImpl::convertInputCoords(int& x, int& y)
{
#ifdef FLIP_OUTPUT_Y
	y = mViewHeight - y;
#endif
}

// convert linear zoom (1.0, 2.0, 3.0 etc.) to log based zoom CEF uses
// where 0.0 is 100%, 1.0 is 120%, 2.0 is 144% etc. (each 1.0 == 20% more)
double LLCEFLibImpl::convertZoomLevel(double linear_zoom)
{
	if (linear_zoom == 0)
	{
		return 0.0;
	}

	double cef_zoom = log(linear_zoom) / log(1.2);

	return cef_zoom;
}
