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

#ifndef _LLCEFLIBPLATFORM
#define _LLCEFLIBPLATFORM

#define LLCEFLIB_DEBUG

#ifdef LLCEFLIB_DEBUG
#include <iostream>
#endif

// flip the output in the Y direction so it's compatible with SL
#define FLIP_OUTPUT_Y

#define CEF_BRANCH_2378     (2)
#define CEF_BRANCH_2357		(1)
#define CEF_BRANCH_2272     (0) /* Base branch, we originally started from this version */

#ifdef WIN32
#define CEF_CURRENT_BRANCH  CEF_BRANCH_2357
#elif __APPLE__
#define CEF_CURRENT_BRANCH  CEF_BRANCH_2272
#endif

#endif // _LLCEFLIBPLATFORM