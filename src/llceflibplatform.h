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

#ifndef _LLCEFLIBPLATFORM
#define _LLCEFLIBPLATFORM

//#define LLCEFLIB_DEBUG
#ifdef LLCEFLIB_DEBUG
#include <iostream>
#endif

#if 0 // <CV:HB>
// Latest version of CEF in use on Windows platform has
// different interfaces from 2171 in use on OS X. This
// flag is used to switch between the two.
#ifdef WIN32
#define LATEST_CEF_VERSION
#elif __APPLE__
#undef  LATEST_CEF_VERSION
#endif
#endif // </CV:HB>

// flip the output in the Y direction so it's compatible with SL
#define FLIP_OUTPUT_Y

#if 1 // <CV:HB>
#define CEF_BRANCH_3029     (5)
#define CEF_BRANCH_2526     (4)
#define CEF_BRANCH_2454     (3)
#define CEF_BRANCH_2357     (2)
#define CEF_BRANCH_2272     (1)
#define CEF_BRANCH_2171     (0)

#if defined(__APPLE__)
#define CEF_CURRENT_BRANCH  CEF_BRANCH_2171
#elif defined(__linux__)
#define CEF_CURRENT_BRANCH  CEF_BRANCH_3029
#else
#define CEF_CURRENT_BRANCH  CEF_BRANCH_2526
#endif
#endif // </CV:HB>

#endif // _LLCEFLIBPLATFORM
