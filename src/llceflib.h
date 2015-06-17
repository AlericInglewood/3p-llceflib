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

#ifndef _LLCEFLIB
#define _LLCEFLIB

#include <string>
#include <memory>
#include "boost/function.hpp"   // dependency on boost until we move to C++11 (std::function)

class LLCEFLibImpl;

// used to pass in various settings during initialization
struct LLCEFLibSettings
{
    // initial dimensions of the browser window
    unsigned int inital_width = 512;
    unsigned int inital_height = 512;

    // substring inserted into existing user agent string
    std::string user_agent_substring = "LLCEFLib";

    // enable/disable features
    bool javascript_enabled = true;
    bool plugins_enabled = true;
    bool cookies_enabled = true;

    // active locale
    std::string locale = "en-US";
};

class LLCEFLib
{
    public:
        LLCEFLib();
        ~LLCEFLib();

        bool init(LLCEFLibSettings& user_settings);

        void update();
        void setSize(int width, int height);
        void getSize(int& width, int& height);
        void navigate(std::string url);
        void mouseButton(int button, bool is_down, int x, int y);
        void mouseMove(int x, int y);

        void keyPress(int code, bool is_down);
        void setFocus(bool focus);
        void mouseWheel(int deltaY);

        void reset();

        void stop();
        void reload(bool ignore_cache);
        bool canGoBack();
        void goBack();
        bool canGoForward();
        void goForward();
        bool isLoading();

        void setPageChangedCallback(boost::function<void(unsigned char*, int, int)> callback);
        void setOnCustomSchemeURLCallback(boost::function<void(std::string)> callback);
        void setOnConsoleMessageCallback(boost::function<void(std::string, std::string, int)> callback);
        void setOnStatusMessageCallback(boost::function<void(std::string)> callback);
        void setOnTitleChangeCallback(boost::function<void(std::string)> callback);

    private:
        std::auto_ptr <LLCEFLibImpl> mImpl;
};

#endif // _LLCEFLIB
