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

#ifndef _LLCONTEXTHANDLER_H
#define _LLCONTEXTHANDLER_H

#include "llceflibplatform.h"

#include "include/cef_app.h"
#include "include/cef_request_context_handler.h"
#include "include/cef_cookie.h"

class LLContextHandler: public CefRequestContextHandler
{
    public:
        LLContextHandler(std::string cookieStorageDirectory)
        {
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
            mCookieManager = CefCookieManager::CreateManager(CefString(cookieStorageDirectory), false, nullptr);
#else
            mCookieManager = CefCookieManager::CreateManager(CefString(cookieStorageDirectory), false);
#endif
        };

        virtual ~LLContextHandler()
        {
        };

        CefRefPtr<CefCookieManager> GetCookieManager() OVERRIDE
        {
            return mCookieManager;
        }

#if CEF_CURRENT_BRANCH >= CEF_BRANCH_2357
        bool OnBeforePluginLoad(const CefString& mime_type,
                                const CefString& plugin_url,
                                const CefString& top_origin_url,
                                CefRefPtr<CefWebPluginInfo> plugin_info,
                                PluginPolicy* plugin_policy) OVERRIDE
        {
            if (*plugin_policy != PLUGIN_POLICY_ALLOW &&
                    mime_type == "application/pdf")
            {
                *plugin_policy = PLUGIN_POLICY_ALLOW;
                return true;

            }

            return false;
        }
#endif

    private:
        CefRefPtr<CefCookieManager> mCookieManager;

        IMPLEMENT_REFCOUNTING(LLContextHandler);
};

#endif  // _LLCONTEXTHANDLER_H
