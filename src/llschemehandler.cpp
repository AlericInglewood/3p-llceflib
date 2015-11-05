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

// IMPORTANT:
// removed for now since the scheme handler as implemented is broken in rev 2357
// now schemes are caught via onBeforeBrowse override - when 2357 is fixed the code
// should revert to using a scheme hander vs a URL parser

#include "llceflibplatform.h"

#include "llschemehandler.h"

#include <string>

#include "include/cef_resource_handler.h"
#include "include/cef_response.h"
#include "include/cef_request.h"
#include "include/cef_scheme.h"
#include "include/wrapper/cef_helpers.h"

#include "llceflibimpl.h"

namespace scheme_handler
{

class ClientSchemeHandler : public CefResourceHandler
{
    public:
        ClientSchemeHandler(LLCEFLibImpl* parent) :
            mParent(parent),
            offset_(0)
        {
        }

        virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) OVERRIDE
        {
            CEF_REQUIRE_IO_THREAD();

            std::string url = request->GetURL();
#ifdef LLCEFLIB_DEBUG
            std::cout << "ClientSchemeHandler - url is " << url << std::endl;
#endif
            mParent->onCustomSchemeURL(url);

            mime_type_ = "none/secondlife";
            callback->Continue();

            return true;
        }

        virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) OVERRIDE
        {
            CEF_REQUIRE_IO_THREAD();

            response->SetMimeType(mime_type_);
            response->SetStatus(200);
            response_length = 0;
        }

        virtual void Cancel() OVERRIDE
        {
            CEF_REQUIRE_IO_THREAD();
        }

        virtual bool ReadResponse(void* data_out,
                                  int bytes_to_read,
                                  int& bytes_read,
                                  CefRefPtr<CefCallback> callback) OVERRIDE
        {
            CEF_REQUIRE_IO_THREAD();
            return false;
        }

    private:
        LLCEFLibImpl* mParent;
        std::string data_;
        std::string mime_type_;
        size_t offset_;

        IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
};

class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory
{
    public:
        ClientSchemeHandlerFactory(LLCEFLibImpl* parent) :
            mParent(parent)
        {
        }

        virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const CefString& scheme_name,
                CefRefPtr<CefRequest> request) OVERRIDE
        {
            CEF_REQUIRE_IO_THREAD();

            return new ClientSchemeHandler(mParent);
        }

        IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);

    private:
        LLCEFLibImpl* mParent;
};

const CefString& schemeName("secondlife");  // scheme name we want to catch
const CefString& domainName("");            // domain name ignored for non-standard schemes

void RegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
    registrar->AddCustomScheme(schemeName, true, false, false);
}

void RegisterSchemeHandlers(LLCEFLibImpl* parent)
{
    CefRegisterSchemeHandlerFactory(schemeName, domainName,
                                    new scheme_handler::ClientSchemeHandlerFactory(parent));
}

} // scheme_handler
