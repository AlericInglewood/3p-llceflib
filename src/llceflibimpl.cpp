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
    std::string user_agent(user_settings.user_agent_substring);
    cef_string_utf8_to_utf16(user_agent.c_str(), user_agent.size(), &settings.product_version);
    std::string locale(user_settings.locale);
    cef_string_utf8_to_utf16(locale.c_str(), locale.size(), &settings.locale);
    
    bool result = CefInitialize(args, settings, NULL, NULL);
    if (! result)
    {
        return false;
    }
    
    scheme_handler::RegisterSchemeHandlers(this);
    
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
    
//#ifdef WIN32
//#warning Can the block below be removed on Windows?
//#endif
    // Does setting windowless_rendering_enabled to true (line 84) work on Windows,
    // or do we still need the code below?
    
    //HWND windowHandle = GetDesktopWindow();
    //window_info.SetAsWindowless(windowHandle, false);
    
    // CEF handler classes
    LLRenderHandler* renderHandler = new LLRenderHandler(this);
    mBrowserClient = new LLBrowserClient(this, renderHandler);
    
    // Add a custom context handler that implements a
    // CookieManager so cookies will persist to disk.
    
#ifdef WIN32
	std::string cookiePath = ".\\cookies";
#elif __APPLE__
    NSString* appDataDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
    std::string cookiePath = [[NSString stringWithFormat: @"%@/%@", appDataDirectory, @"llceflib_cookies"] UTF8String];
#endif
    
    // CEF changed interfaces between these two branches
    // Can be removed once we decide on a release CEF version
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2378
    CefRequestContextSettings contextSettings;
    
    CefRefPtr<LLContextHandler> contextHandler = new LLContextHandler(cookiePath.c_str());
    CefRefPtr<CefRequestContext> rc = CefRequestContext::CreateContext(contextSettings, contextHandler.get());
#else // CEF_BRANCH_2272
    CefRefPtr<CefRequestContext> rc = CefRequestContext::CreateContext( new LLContextHandler(cookiePath.c_str()) );
#endif
    
    CefString url = "about:blank";
    mBrowser = CefBrowserHost::CreateBrowserSync(window_info, mBrowserClient.get(), url, browser_settings, rc);
    
    return true;
}

void LLCEFLibImpl::update()
{
    CefDoMessageLoopWork();
}

void LLCEFLibImpl::setPageChangedCallback(boost::function<void(unsigned char*, int, int)> callback)
{
    mPageChangedCallbackFunc = callback;
}

void LLCEFLibImpl::setOnCustomSchemeURLCallback(boost::function<void(std::string)> callback)
{
    mOnCustomSchemeURLCallbackFunc = callback;
}

void LLCEFLibImpl::setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback)
{
    mOnConsoleMessageCallbackFunc = callback;
}

void LLCEFLibImpl::setOnStatusMessageCallback(boost::function<void(std::string)> callback)
{
    mOnStatusMessageCallbackFunc = callback;
}

void LLCEFLibImpl::setOnTitleChangeCallback(boost::function<void(std::string)> callback)
{
    mOnTitleChangeCallbackFunc = callback;
}

void LLCEFLibImpl::setSize(int width, int height)
{
    mViewWidth = width;
    mViewHeight = height;

    if(mBrowser)
    {
        if(mBrowser->GetHost())
        {
            mBrowser->GetHost()->WasResized();
        }
    }
}

void LLCEFLibImpl::getSize(int& width, int& height)
{
    width = mViewWidth;
    height = mViewHeight;
}

void LLCEFLibImpl::pageChanged(unsigned char* pixels, int width, int height)
{
    if(mPageChangedCallbackFunc)
        mPageChangedCallbackFunc(pixels, width, height);
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

void LLCEFLibImpl::onStatusMessage(std::string value)
{
    if(mOnStatusMessageCallbackFunc)
        mOnStatusMessageCallbackFunc(value);
}

void LLCEFLibImpl::onTitleChange(std::string title)
{
    if(mOnTitleChangeCallbackFunc)
        mOnTitleChangeCallbackFunc(title);
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

void LLCEFLibImpl::mouseButton(int button, bool is_down, int x, int y)
{
    CefMouseEvent mouse_event;
    mouse_event.x = x;
    mouse_event.y = y;

    CefBrowserHost::MouseButtonType btnType = MBT_LEFT; // TODO - generalize to all buttons
    int last_click_count = 1;
    mBrowser->GetHost()->SendMouseClickEvent(mouse_event, btnType, is_down ? false : true, last_click_count);
    if(is_down)
    {
#ifdef LLCEFLIB_DEBUG
        std::cout << "Setting focus" << std::endl;
#endif
        mBrowser->GetHost()->SetFocus(true);
    }
};

void LLCEFLibImpl::mouseMove(int x, int y)
{
    CefMouseEvent mouse_event;
    mouse_event.x = x;
    mouse_event.y = y;

    mBrowser->GetHost()->SendMouseMoveEvent(mouse_event, false);
};

void LLCEFLibImpl::mouseWheel(int deltaY)
{
    if(mBrowser)
    {
        if(mBrowser->GetHost())
        {
            CefMouseEvent mouse_event;
            //const CefRect& expected_rect = GetExpectedRect(0);
            //mouse_event.x = MiddleX(expected_rect);
            //mouse_event.y = MiddleY(expected_rect);
            mouse_event.modifiers = 0;
            mBrowser->GetHost()->SendMouseWheelEvent(mouse_event, 0, deltaY);
        }
    }
}

void LLCEFLibImpl::setFocus(bool focus)
{
    if(mBrowser && mBrowser->GetHost())
        mBrowser->GetHost()->SendFocusEvent(focus);
}

void LLCEFLibImpl::reset()
{
    bool force_close = true;
    mBrowser->GetHost()->CloseBrowser(force_close);
    CefShutdown();
}

void LLCEFLibImpl::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
    // called when registering custom schemes
    // TODO: what else do we need to do here?
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