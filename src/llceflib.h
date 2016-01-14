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

#ifndef _LLCEFLIB
#define _LLCEFLIB

#include <string>
#include <memory>
#include <vector>

// dependency on boost until we move to C++11 (std::function) on OS X
#include "boost/function.hpp"
#include "boost/move/unique_ptr.hpp"

class LLCEFLibImpl;

// version information
// version of this library
const std::string LLCEFLIB_BASE_VERSION = "1.5.1";

// version of CEF and the version of Chrome it represents on Windows
const std::string CEF_VERSION_WIN = "(CEF-WIN-3.2526.1347-32)";
const std::string CEF_CHROME_VERSION_WIN = "47.0.2526.16";

// version of CEF and the version of Chrome it represents on OS X
const std::string CEF_VERSION_OSX = "(CEF-OSX-3.2171.2069-32)";
const std::string CEF_CHROME_VERSION_OSX = "39.0.2171.95";

// composite version for display
#ifdef WIN32
const std::string LLCEFLIB_VERSION = LLCEFLIB_BASE_VERSION + "-" + CEF_VERSION_WIN;
#else
const std::string LLCEFLIB_VERSION = LLCEFLIB_BASE_VERSION + "-" + CEF_VERSION_OSX;
#endif

class LLCEFLib
{
    public:

        // used to pass in various settings during initialization
        struct LLCEFLibSettings
        {
            // initial dimensions of the browser window
            unsigned int initial_width = 512;
            unsigned int initial_height = 512;

            // substring inserted into existing user agent string
            std::string user_agent_substring = "";

            // enable/disable features
            bool javascript_enabled = true;
            bool plugins_enabled = true;
            bool media_stream_enabled = true;
            bool cookies_enabled = true;
            bool cache_enabled = true;

            // path to browser cache
            std::string cache_path = "";

            // path to cookie store
            std::string cookie_store_path = "";

            // list of language locale codes used to configure the Accept-Language HTTP header value
            // and change the default language of the browser
            std::string accept_language_list = "en-us";
        };

        // keyboard event types
        typedef enum e_key_event
        {
            KE_KEY_DOWN,
            KE_KEY_REPEAT,
            KE_KEY_UP
        } EKeyEvent;

        // mouse vent types
        typedef enum e_mouse_event
        {
            ME_MOUSE_MOVE,
            ME_MOUSE_DOWN,
            ME_MOUSE_UP,
            ME_MOUSE_DOUBLE_CLICK
        } EMouseEvent;

        // mouse buttons
        typedef enum e_mouse_button
        {
            MB_MOUSE_BUTTON_LEFT,
            MB_MOUSE_BUTTON_RIGHT,
            MB_MOUSE_BUTTON_MIDDLE
        } EMouseButton;

        // keyboard modifiers
        typedef enum e_keyboard_modifier
        {
            KM_MODIFIER_NONE = 0x00,
            KM_MODIFIER_SHIFT = 0x01,
            KM_MODIFIER_CONTROL = 0x02,
            KM_MODIFIER_ALT = 0x04,
            KM_MODIFIER_META = 0x08
        } EKeyboardModifier;

        // cursor types returned via setOnCursorChangedCallback
        typedef enum e_cursor_type
        {
            CT_POINTER = 0,
            CT_CROSS, CT_HAND, CT_IBEAM, CT_WAIT, CT_HELP,
            CT_EASTRESIZE, CT_NORTHRESIZE, CT_NORTHEASTRESIZE, CT_NORTHWESTRESIZE, CT_SOUTHRESIZE,
            CT_SOUTHEASTRESIZE, CT_SOUTHWESTRESIZE, CT_WESTRESIZE, CT_NORTHSOUTHRESIZE, CT_EASTWESTRESIZE,
            CT_NORTHEASTSOUTHWESTRESIZE, CT_NORTHWESTSOUTHEASTRESIZE, CT_COLUMNRESIZE, CT_ROWRESIZE,
            CT_MIDDLEPANNING, CT_EASTPANNING, CT_NORTHPANNING, CT_NORTHEASTPANNING, CT_NORTHWESTPANNING,
            CT_SOUTHPANNING, CT_SOUTHEASTPANNING, CT_SOUTHWESTPANNING, CT_WESTPANNING, CT_MOVE,
            CT_VERTICALTEXT, CT_CELL, CT_CONTEXTMENU, CT_ALIAS, CT_PROGRESS, CT_NODROP, CT_COPY,
            CT_NONE, CT_NOTALLOWED, CT_ZOOMIN, CT_ZOOMOUT, CT_GRAB, CT_GRABBING, CT_CUSTOM,
        } ECursorType;

        LLCEFLib();
        ~LLCEFLib();

        // initialize LLCEFLib - call before anything else
        bool init(LLCEFLib::LLCEFLibSettings& user_settings);

        // call regularly in your message loop
        void update();

        // set/get the size of the virtual browser
        void setSize(int width, int height);
        void getSize(int& width, int& height);

        // navigate to a new URL
        void navigate(std::string url);

        // utility function to post data to a URL
        void postData(std::string url, std::string data, std::string headers);

        // set a cookie in the CEF cookie store
        void setCookie(std::string url, std::string name, std::string value, std::string domain, std::string path, bool httponly, bool secure);

        // set page zoom factor (1.0 == normal)
        void setPageZoom(double zoom_val);

        // mouse button down/up and move and mouse wheel
        void mouseButton(EMouseButton mouse_button, EMouseEvent mouse_event, int x, int y);
        void mouseMove(int x, int y);
        void mouseWheel(int deltaX, int deltaY);

        // construct a Windows keyboard event from component data
        void keyboardEvent(
            EKeyEvent key_event,
            uint32_t key_code,
            const char* utf8_text,
            EKeyboardModifier modifiers,
            uint32_t native_scan_code,
            uint32_t native_virtual_key,
            uint32_t native_modifiers);

        // native keyboard event for Windows using messages/parameters
        void nativeKeyboardEvent(uint32_t msg, uint32_t wparam, uint64_t lparam);

        // construct an OS X keyboard event from first principles
        void keyboardEventOSX(uint32_t eventType, uint32_t modifiers, const char* characters,
                              const char* unmodCharacters, bool repeat, uint32_t keyCode);

        void injectUnicodeText(wchar_t unicodeChars, wchar_t unmodChars, uint32_t keyCode, uint32_t modifiers);

        // native keyboard event for OS X
        void nativeKeyboardEventOSX(void* nsEvent);

        // set/remove focus to/from the virtual browser
        void setFocus(bool focus);

        // request that the application exit - wait for a callback (see below)
        void requestExit();

        // once you get a callback, use this to shutdown LLCEFLib just before you exit the app
        void shutdown();

        // browsing controls - self explanatory
        void stop();
        void reload(bool ignore_cache);
        bool canGoBack();
        void goBack();
        bool canGoForward();
        void goForward();
        bool isLoading();

        // indicates if there is something available to be copy/cut/pasted
        // (for UI purposes) and if so, provides methods to do so
        bool editCanCopy();
        bool editCanCut();
        bool editCanPaste();
        void editCopy();
        void editCut();
        void editPaste();

        // set the scheme names which will intercepted and trigger a callback
        void setCustomSchemes(std::vector<std::string> custom_schemes);

        // show or hide the Chrome developer console
        void showDevTools(bool show);

        // construct a User Agent String that is "more compatible" with sites
        // e.g. is has the "Chrome/version" string in it
        std::string makeCompatibleUserAgentString(const std::string base);

        // callbacks you can hook up and monitor events
        // called when the contents of a page changes
        void setOnPageChangedCallback(boost::function<void(unsigned char*, int, int, int, int, bool)> callback);

        // called when a custome scheme URL is entered (see setCustomSchemes)
        void setOnCustomSchemeURLCallback(boost::function<void(std::string)> callback);

        // called when a JavaScript console message is to be displayed
        void setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback);

        // called when a browser status message is to be displayed
        void setOnStatusMessageCallback(boost::function<void(std::string)> callback);

        // called when the URL address changes
        void setOnAddressChangeCallback(boost::function<void(std::string)> callback);

        // called when the page title (<title>) changes
        void setOnTitleChangeCallback(boost::function<void(std::string)> callback);

        // called when a page starts to load
        void setOnLoadStartCallback(boost::function<void()> callback);

        // called when a page "finishes" loading
        void setOnLoadEndCallback(boost::function<void(int)> callback);

        // called after you call requestExit() and LLCEFLib is ready to close
        void setOnRequestExitCallback(boost::function<void()> callback);

        // called when the cursor changes
        void setOnCursorChangedCallback(boost::function<void(LLCEFLib::ECursorType type, unsigned int)> callback);

        // called when a URL is navigated to (and has a target name)
        void setOnNavigateURLCallback(boost::function<void(std::string url, std::string target)> callback);

        // called when an HTTP AUTH request is made
        void setOnHTTPAuthCallback(boost::function<bool(const std::string host, const std::string realm, std::string&, std::string&)> callback);

        // called when a file download request is made
        void setOnFileDownloadCallback(boost::function<void(const std::string filename)> callback);

    private:
        boost::movelib::unique_ptr <LLCEFLibImpl> mImpl;
};

#endif // _LLCEFLIB
