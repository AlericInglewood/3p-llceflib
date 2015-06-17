/**
 * @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
 *                   Test app loader
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

#include <iostream>

#ifdef WIN32
#include <windows.h>

typedef int(__cdecl *f_testfunc)();
#elif __APPLE__
#include <dlfcn.h>

//typedef void func_helloWorld();
typedef int  func_startTestGL();

// This file is compiled as Objective-C++ on OS X
#import <Cocoa/Cocoa.h>
#endif

int main(int argc, char* argv[])
{
#if WIN32
    HINSTANCE hGetProcIDDLL = LoadLibraryA("testgldll.dll");

    if ( ! hGetProcIDDLL)
    {
        std::cout << "could not load the dynamic library" << std::endl;
        return -1;
    }

    f_testfunc testfunc = (f_testfunc)GetProcAddress(hGetProcIDDLL, "startTestGL");
    if (! testfunc)
    {
        std::cout << "could not locate the function" << std::endl;
        return -1;
    }

    std::cout << "startTestGL() returned " << testfunc() << std::endl;
#elif __APPLE__
    // Construct dylib path, stored at app_bundle/Contents/Frameworks/lib_name.dylib
    NSString* appBundlePath = [[NSBundle mainBundle] bundlePath];
    NSString* dylibPath = [NSString stringWithFormat: @"%@/Contents/Frameworks/%@", appBundlePath, @"libtestgldll.dylib"];
    
    // Open Library
    void* hDylib = dlopen([dylibPath UTF8String], RTLD_LAZY);
    
    if ( ! hDylib)
    {
        std::cout << "could not load the dynamic library, reason: " << dlerror() << std::endl;
        return -1;
    }
    
    func_startTestGL* startTestGL = (func_startTestGL*) dlsym(hDylib, "startTestGL");
    if ( ! startTestGL)
    {
        std::cout << "could not locate the function, reason: " << dlerror() << std::endl;
        return -1;
    }
    
    int rt = startTestGL();
    std::cout << "startTestGL() returned " << rt << std::endl;

#if 0
    // Void function
    func_helloWorld* helloWorld;
    helloWorld = (func_helloWorld*) dlsym(hDylib, "helloWorld");
    
    if( ! helloWorld )
    {
        std::cout << "could not locate the function, reason: " << dlerror() << std::endl;
        return -1;
    }
    
    helloWorld();
#endif
#endif
    
    return 0;
}