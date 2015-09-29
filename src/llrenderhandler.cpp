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

bool LLRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    int width, height;
    mParent->getSize(width, height);

#ifdef FLIP_OUTPUT_Y
	// change flip buffer size only if it changed
	resizeFlipBuffer(width, height);
#endif

    rect = CefRect(0, 0, width, height);

	return true;
}

void LLRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
#ifdef FLIP_OUTPUT_Y
	// change flip buffer size only if it changed
	resizeFlipBuffer(width, height);

	for (int y = 0; y < height; ++y)
	{
		memcpy(flipBuffer + y * width * 4, (unsigned char*)buffer + (height - y - 1) * width * 4, width * 4);
	}

	mParent->onPageChanged(flipBuffer, width, height);
#elif
	mParent->onPageChanged((unsigned char*)(buffer), width, height);
#endif
}

void LLRenderHandler::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info)
{
	mParent->onCursorChanged((unsigned int)cursor);
}
