Building CEF from source
=========================

The version in this folder has been built already. It is the most recent 32bit version
of CEF for OSX and there will be no more official releases. If you want to build it yourself
these notes might help you:

* Grab the Mac 32bit revison 2171 build of CEF from http://cefbuilds.com 
  * Note: do NOT use the more recent "Dev channel (trunk)" 32bit release - this is 64bit
* Unpack and rename folder to 'cef_2171_OSX_32' along side llcef "src folder
* Download and install cmake if not installed, version 3.2.2 or greater
* cd cef_2171_OSX_32
* mkdir build
* cd build
* cmake -G "Xcode" -DPROJECT_ARCH="i386" ..
* open cef.xcodeproj
* In the project navigator, select libcef_dll_wrapper as a target and modify:
  1) OS X Deployment target 10.6 -> 10.7
  2) C++ standard library -> libstdc++ (GNU C++ standard library)
  3) C++ Language Dialect -> Compiler Default
  4) Other C flags: -mmacosx-version-min=10.6 -> -mmacosx-version-min=10.7
  5) Other C++ flags: -mmacosx-version-min=10.6 -> -mmacosx-version-min=10.7 
* Set libcef_dll_wrapper as the build project
* Build libcef_dll_wrapper Release configurations

Additional note: './Debug' folder was removed because it's huge (~500MB) and 
we not used because we don't build a debug version. It is available in the 
package from http://cefbuilds.com
