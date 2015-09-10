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

#include "llceflibimpl.h"
#include "llCEFLib.h"

#include "llceflibplatform.h"

#include "llschemehandler.h"
#include "llrenderhandler.h"
#include "llbrowserclient.h"
#include "llcontexthandler.h"

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif

LLCEFLibImpl::LLCEFLibImpl() :
    mViewWidth(0),
    mViewHeight(0),
    mBrowser(0)
{
	// default is second life scheme
	mCustomSchemes = std::vector<std::string> { "secondlife://" };
}

LLCEFLibImpl::~LLCEFLibImpl()
{
}

bool LLCEFLibImpl::init(LLCEFLibSettings& user_settings)
{
#ifdef LLCEFLIB_DEBUG
    std::cout << "Starting.." << std::endl;
#endif

#ifdef WIN32
	CefMainArgs args(GetModuleHandle(NULL));
#elif __APPLE__
	CefMainArgs args(0, NULL);
#endif


    CefSettings settings;

#ifdef WIN32
    CefString(&settings.browser_subprocess_path) = "llceflib_host.exe";
#elif __APPLE__
    NSString* appBundlePath = [[NSBundle mainBundle] bundlePath];
    CefString(&settings.browser_subprocess_path) = [[NSString stringWithFormat: @"%@/Contents/Frameworks/LLCefLib Helper.app/Contents/MacOS/LLCefLib Helper", appBundlePath] UTF8String];
#endif

	// change settings based on what was passed in
	// Only change user agent if user wants to
	if (user_settings.user_agent_substring.length())
	{
		std::string user_agent(user_settings.user_agent_substring);
		cef_string_utf8_to_utf16(user_agent.c_str(), user_agent.size(), &settings.product_version);
	}

	// list of language locale codes used to configure the Accept-Language HTTP header value
#ifdef WIN32
	std::string accept_language_list(user_settings.accept_language_list);
	cef_string_utf8_to_utf16(accept_language_list.c_str(), accept_language_list.size(), &settings.accept_language_list);
#elif __APPLE__
	// feature not supported on revision of OS X CEF we are locked to in 32 bit land
#endif

	// set path to cache if enabled and set
	if (user_settings.cache_enabled && user_settings.cache_path.length())
	{
		CefString(&settings.cache_path) = user_settings.cache_path;
	}

    bool result = CefInitialize(args, settings, this, NULL);
    if (! result)
    {
        return false;
    }
    
	// removed for now since the scheme handler as implemented is broken in rev 2357
	// now schemes are caught via onBeforeBrowse override - when 2357 is fixed the code
	// should revert to using a scheme hander vs a URL parser
	//scheme_handler::RegisterSchemeHandlers(this);
    
    setSize(user_settings.inital_width, user_settings.inital_height);
        
    CefWindowInfo window_info;
    window_info.windowless_rendering_enabled = true;
    
    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 60; // 30 is default - hook into LL media scheduling system?
    browser_settings.java = STATE_DISABLED;
    browser_settings.webgl = STATE_ENABLED;
    
    // change settings based on what was passed in
    browser_settings.javascript = user_settings.javascript_enabled ? STATE_ENABLED : STATE_DISABLED;
    browser_settings.plugins = user_settings.plugins_enabled ? STATE_ENABLED : STATE_DISABLED;

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
		if (user_settings.cookie_store_path.length())
		{
			cookiePath = std::string(user_settings.cookie_store_path);
		}
#elif __APPLE__
		// TODO: pass on cookie path to OS X version too
		NSString* appDataDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
		std::string cookiePath = [[NSString stringWithFormat: @"%@/%@", appDataDirectory, @"llceflib_cookies"] UTF8String];
#endif

		// CEF changed interfaces between these two branches
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
		CefRequestContextSettings contextSettings;
		mContextHandler = new LLContextHandler(cookiePath.c_str());
		rc = CefRequestContext::CreateContext(contextSettings, mContextHandler.get());
#else // CEF_BRANCH_2272
		rc = CefRequestContext::CreateContext(new LLContextHandler(cookiePath.c_str()));
#endif
	}

    CefString url = "";
    mBrowser = CefBrowserHost::CreateBrowserSync(window_info, mBrowserClient.get(), url, browser_settings, rc);
    
    return true;
}

void LLCEFLibImpl::update()
{
	if (mBrowserClient)
	{
		CefDoMessageLoopWork();

		if (mBrowserClient->isBrowserClosing())
		{
			CefQuitMessageLoop();
#ifdef LLCEFLIB_DEBUG
			std::cout << "Update loop told to close, call CefShutdown() then call exit callback" << std::endl;
#endif
			CefShutdown();

			mBrowserClient = 0;

			// tell the app counsuming us it's okay to exit now
			if (mOnRequestExitCallbackFunc)
				mOnRequestExitCallbackFunc();
		}
	}
}

void LLCEFLibImpl::setOnPageChangedCallback(boost::function<void(unsigned char*, int, int)> callback)
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

void LLCEFLibImpl::setSize(int width, int height)
{
    mViewWidth = width;
    mViewHeight = height;

    if(mBrowser && mBrowser->GetHost())
	{
		mBrowser->GetHost()->WasResized();
    }
}

void LLCEFLibImpl::getSize(int& width, int& height)
{
    width = mViewWidth;
    height = mViewHeight;
}

void LLCEFLibImpl::onPageChanged(unsigned char* pixels, int width, int height)
{
    if(mOnPageChangedCallbackFunc)
        mOnPageChangedCallbackFunc(pixels, width, height);
}

void LLCEFLibImpl::onCustomSchemeURL(std::string url)
{
    if(mOnCustomSchemeURLCallbackFunc)
        mOnCustomSchemeURLCallbackFunc(url);
}

void LLCEFLibImpl::onConsoleMessage(std::string message, std::string source, int line)
{
    if(mOnConsoleMessageCallbackFunc)
        mOnConsoleMessageCallbackFunc(message, source, line);
}

void LLCEFLibImpl::onAddressChange(std::string new_url)
{
	if (mOnAddressChangeCallbackFunc)
		mOnAddressChangeCallbackFunc(new_url);
}

void LLCEFLibImpl::onStatusMessage(std::string value)
{
	if (mOnStatusMessageCallbackFunc)
		mOnStatusMessageCallbackFunc(value);
}

void LLCEFLibImpl::onTitleChange(std::string title)
{
	if (mOnTitleChangeCallbackFunc)
		mOnTitleChangeCallbackFunc(title);
}

void LLCEFLibImpl::onLoadStart()
{
	if (mOnLoadStartCallbackFunc)
		mOnLoadStartCallbackFunc();
}

void LLCEFLibImpl::onLoadEnd(int httpStatusCode)
{
	if (mOnLoadEndCallbackFunc)
		mOnLoadEndCallbackFunc(httpStatusCode);
}

void LLCEFLibImpl::onNavigateURL(std::string url, std::string target)
{
	if (mOnNavigateURLCallbackFunc)
		mOnNavigateURLCallbackFunc(url, target);
}

bool LLCEFLibImpl::onHTTPAuth(const std::string host, const std::string realm, std::string& username, std::string& password)
{
	if (mOnHTTPAuthCallbackFunc)
		return mOnHTTPAuthCallbackFunc(host, realm, username, password);

	return false;
}

int LLCEFLibImpl::getDepth()
{
    return mViewDepth;
}

void LLCEFLibImpl::navigate(std::string url)
{
    if(mBrowser)
    {
        if(mBrowser->GetMainFrame())
        {
            mBrowser->GetMainFrame()->LoadURL(url);
        }
    }
}

void LLCEFLibImpl::setPageZoom(double zoom_val)
{
	if (mBrowser && mBrowser->GetHost())
	{
		mBrowser->GetHost()->SetZoomLevel(zoom_val);
	}
}

void LLCEFLibImpl::mouseButton(EMouseButton mouse_button, EMouseEvent mouse_event, int x, int y)
{
	// select click location
    CefMouseEvent cef_mouse_event;
    cef_mouse_event.x = x;
    cef_mouse_event.y = y;
	cef_mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

	// select button
	CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
	if (mouse_button == MB_MOUSE_BUTTON_RIGHT) btnType = MBT_RIGHT;
	if (mouse_button == MB_MOUSE_BUTTON_MIDDLE) btnType = MBT_MIDDLE;

	// TODO: set this properly
    int last_click_count = 1;

	// action TODO: extend to include "move" although this might be enough
	bool is_down = false;
	if (mouse_event == ME_MOUSE_DOWN) is_down = true;
	if (mouse_event == ME_MOUSE_UP) is_down = false;

	// send to CEF
	if (mBrowser && mBrowser->GetHost())
	{
		mBrowser->GetHost()->SendMouseClickEvent(cef_mouse_event, btnType, is_down ? false : true, last_click_count);
	}
};

void LLCEFLibImpl::mouseMove(int x, int y)
{
    CefMouseEvent mouse_event;
    mouse_event.x = x;
    mouse_event.y = y;
	mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

	if (mBrowser && mBrowser->GetHost())
	{
		mBrowser->GetHost()->SendMouseMoveEvent(mouse_event, false);
	}
};

void LLCEFLibImpl::mouseWheel(int deltaY)
{
    if(mBrowser && mBrowser->GetHost())
	{
		CefMouseEvent mouse_event;
		mouse_event.modifiers = 0;
		mBrowser->GetHost()->SendMouseWheelEvent(mouse_event, 0, deltaY);
    }
}

void LLCEFLibImpl::setFocus(bool focus)
{
    if(mBrowser && mBrowser->GetHost())
        mBrowser->GetHost()->SendFocusEvent(focus);
}

void LLCEFLibImpl::reset()
{
#ifdef LLCEFLIB_DEBUG
	std::cout << "Closing browser and flushing things" << std::endl;
#endif

	if (mContextHandler && mContextHandler->GetCookieManager())
		mContextHandler->GetCookieManager()->FlushStore(NULL);

	bool force_close = false;
	if (mBrowser && mBrowser->GetHost())
		mBrowser->GetHost()->CloseBrowser(force_close);
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
    if(mBrowser && mBrowser->GetHost())
    {
        mBrowser->StopLoad();
    }
}

void LLCEFLibImpl::reload(bool ignore_cache)
{
    if(mBrowser && mBrowser->GetHost())
    {
        if(ignore_cache)
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
    if(mBrowser && mBrowser->GetHost())
    {
        return mBrowser->CanGoBack();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

void LLCEFLibImpl::goBack()
{
    if(mBrowser && mBrowser->GetHost())
    {
        mBrowser->GoBack();
    }
}

bool LLCEFLibImpl::canGoForward()
{
    if(mBrowser && mBrowser->GetHost())
    {
        return mBrowser->CanGoForward();
    }

    // default to allow so if UI is relies on it, this is still sensible
    return true;
}

void LLCEFLibImpl::goForward()
{
    if(mBrowser && mBrowser->GetHost())
    {
        mBrowser->GoForward();
    }
}

bool LLCEFLibImpl::isLoading()
{
    if(mBrowser && mBrowser->GetHost())
    {
        mBrowser->IsLoading();
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
