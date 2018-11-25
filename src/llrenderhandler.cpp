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

#include "llrenderhandler.h"

#include "llceflibimpl.h"

LLRenderHandler::LLRenderHandler(LLCEFLibImpl* parent) :
    mParent(parent)
{
#ifdef FLIP_OUTPUT_Y
    flipBufferWidth = 0;
    flipBufferHeight = 0;
    flipBufferDepth = parent->getDepth();
    flipBuffer = 0;
#endif
}

LLRenderHandler::~LLRenderHandler()
{
#ifdef FLIP_OUTPUT_Y
    delete[]  flipBuffer;
#endif
}

void LLRenderHandler::resizeFlipBuffer(int width, int height)
{
#ifdef FLIP_OUTPUT_Y
    if (flipBufferWidth != width || flipBufferHeight != height)
    {
        delete[]  flipBuffer;
        flipBufferWidth = width;
        flipBufferHeight = height;
        flipBuffer = new unsigned char[flipBufferWidth * flipBufferHeight * flipBufferDepth];
        memset(flipBuffer, 0, flipBufferWidth * flipBufferHeight * flipBufferDepth);
    }
#endif
}

#if CEF_CURRENT_BRANCH >= CEF_BRANCH_3578
void LLRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
#else
bool LLRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
#endif
{
    int width, height;
    mParent->getSize(width, height);

#ifdef FLIP_OUTPUT_Y
    // change flip buffer size only if it changed
    resizeFlipBuffer(width, height);
#endif

    rect = CefRect(0, 0, width, height);

#if CEF_CURRENT_BRANCH < CEF_BRANCH_3538
    return true;
#endif
}

void LLRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
{
    int x = 0;
    int y = 0;
    bool is_popup = type == PET_POPUP ? true : false;

    if (type == PET_POPUP)
    {
        x = mPopupRect.x;
        y = mPopupRect.y;
    }

#ifdef FLIP_OUTPUT_Y

    // change flip buffer size only if it changed
    resizeFlipBuffer(width, height);

    for (int y = 0; y < height; ++y)
    {
        memcpy(flipBuffer + y * width * 4, (unsigned char*)buffer + (height - y - 1) * width * 4, width * 4);
    }

    mParent->onPageChanged(flipBuffer, x, y, width, height, is_popup);
#else
    mParent->onPageChanged((unsigned char*)(buffer), x, y, width, height, is_popup);
#endif
}

void LLRenderHandler::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info)
{
    mParent->onCursorChanged((LLCEFLib::ECursorType)type, (unsigned int)(size_t)cursor);
}

void LLRenderHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    if (!show)
    {
        mPopupRect.Set(0, 0, 0, 0);

        mParent->getBrowser()->GetHost()->Invalidate(PET_VIEW);
        mParent->onPageChanged( NULL, 0, 0, 0, 0, true); // ND: Clear the hosts popup buffer
    }
}

void LLRenderHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    setPopupLocation(rect);
}

void LLRenderHandler::setPopupLocation(const CefRect& rect)
{
    mPopupRect = rect;
}

const CefRect& LLRenderHandler::getPopupLocation()
{
    return mPopupRect;
}
