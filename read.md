## Update the version of CEF that LLCEFLib builds against

#### Note: This is mostly a manual process currently. As part of the switch to 64 bit viewers, this process will be significantly more automated.

#### Windows
* Visit http://cefbuilds.com and download the most recent (non-development branch) version of CEF (WIN 32) you can find.
  * The files are compressed using the  format ".7z"
  * You can find tools to extract this format here: http://www.7-zip.org/
  * Extract to a folder
* Open a command prompt in the folder where you extracted the files and enter these commands:
  ``` 
    mkdir build
    cd build
    cmake -G "Visual Studio 12 2013" ..
    start cef.sln
  ```
* Set `libcef_dll_wrapper` to be the StartUp Project
  * In `libcef_dll_wrapper -> Properties -> C/C++ -> Code Generation` change the `Runtime Library` to `Multi-threaded Debug DLL` for Debug configurations and `Multi-threaded DLL` for Release configurations
* Build the Debug and Release configurations in the normal way
* Edit the `tools\make_autobuild_pkg.bat` batch file
  * Change the value for `SRC_DIR` to point to the CEF binary folder you downloaded
  * Change the value for `DST_DIR` to point to a folder along side existing CEF folder in the 3p-llceflib repo - e.g. `cef_2704_WIN_32`
  * Run the batch file
* From a command prompt in the LLCEFLib folder run  `autobuild install` to install the LLCEFLib dependencies via autobuild (Boost)
* Open the `src\llceflib.sln` solution file
* Set the StartUp Project to be `win32gl` (the test app)
* Update the `CEF_VERSION_WIN` and `CEF_CHROME_VERSION_WIN` to reflect the new version numbers. Follow the pattern - you can get the values from http://cefbuilds.com
* Build Release and Debug configurations.
* Important: Is it likely that there will be errors - the CEF interface changes frequently. Fix up the errors.
* Once there are no errors, try running the test app - you should see the web page of test links. Try a few and confirm most work. One to try for sure it the `Browser User Agent String` link. This page should display the same version of Chrome that you set in `CEF_CHROME_VERSION_WIN`
* Push out the new CEF folder and any changes you made to the source code to the repository
* Build it in Team City
* Update the `autobuild.xml` for a viewer to point to it in the usual fashion
* Build the viewer and confirm the embedded browser still functions as expected (login, search, profiles etc.)
* Open the media test floater (web content floater) and visit http://www.useragentstring.com/ - confirm that the Chrome version number matches what you set for `CEF_CHROME_VERSION_WIN` in LLCEFLib
* Using the media test floater, confirm that the reasons you did the update (CEF out of date, exploit fixes, bug fixes etc.) have been addressed.
* Declare victory!

## OS X
CEF is not supported on 32 bit OS X systems anymore so the version is frozen at the 2171 branch until we switch to 64 bit versions.

## Linux
TBD