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

#ifndef _LLRENDERHANDLER
#define _LLRENDERHANDLER

#include "include/cef_render_handler.h"

#include "llceflibplatform.h"

class LLCEFLibImpl;

class LLRenderHandler :
    public CefRenderHandler
{
    public:
        LLRenderHandler(LLCEFLibImpl* parent);
        ~LLRenderHandler();

        // CefRenderHandler interface
    public:
#if CEF_CURRENT_BRANCH >= CEF_BRANCH_3578
        void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
#else
        bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
#endif
        void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) OVERRIDE;
        void OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info) OVERRIDE;
        void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
        void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;

        // utility functions
        void setPopupLocation(const CefRect& rect);
        const CefRect& getPopupLocation();

        // CefBase interface
    public:
        IMPLEMENT_REFCOUNTING(LLRenderHandler);

    private:
        void resizeFlipBuffer(int width, int height);

        LLCEFLibImpl* mParent;
        unsigned char* flipBuffer;
        int flipBufferWidth;
        int flipBufferHeight;
        int flipBufferDepth;
        CefRect mPopupRect;
};

#endif // _LLRENDERHANDLER
