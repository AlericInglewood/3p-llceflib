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

#include "include/cef_app.h"

#ifdef WIN32
#include <windows.h>

// <CV:HB>
// Nasty hack to stop flash from displaying a popup with "NO SANDBOX".
// Flashplayer tries to spawn a cmd.exe and echo this message into it, we use a
// process group to limit the number of processes allowed to 1, thus preventing
// popup.
// Using the sandbox would fix this problem, but for using the sandbox the same
// executable must be used for browser and all sub processes (see
// cef_sandbox_win.h); but the viewer uses slplugin.exe and llceflib_host.exe.

void enablePPAPIFlashHack(LPSTR lpCmdLine)
{
   if (!lpCmdLine) return;

   std::string strCmdLine = lpCmdLine;
   std::string strType = "--type=ppapi";
   std::string::size_type i = strCmdLine.find(strType);
   if (i == std::string::npos) return;

   HANDLE hJob = CreateJobObject(nullptr, nullptr);
   HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE,
						      ::GetCurrentProcessId());
   AssignProcessToJobObject(hJob, hProc);

   JOBOBJECT_BASIC_LIMIT_INFORMATION baseLimits = {};
   baseLimits.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
   baseLimits.ActiveProcessLimit = 1;
   SetInformationJobObject(hJob, JobObjectBasicLimitInformation, &baseLimits,
						   sizeof(baseLimits));
}
// </CV:HB>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CefMainArgs args(GetModuleHandle(NULL));

    enablePPAPIFlashHack(lpCmdLine);
    return CefExecuteProcess(args, nullptr, nullptr);
}
#endif

// OS X Helper executable, we can probably share this between linux & Mac
#if defined(__APPLE__) || defined(__linux__)

// Entry point function for sub-processes.
int main(int argc, char* argv[])
{
    // Provide CEF with command-line arguments.
    CefMainArgs main_args(argc, argv);

    // Execute the sub-process.
    return CefExecuteProcess(main_args, nullptr, nullptr);
}
#endif
