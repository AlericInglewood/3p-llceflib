#!/bin/bash

cd "$(dirname "$0")"

# turn on verbose debugging output for parabuild logs.
set -x
# make errors fatal
set -e

LLCEFLIB_SOURCE_DIR="src"

# hard code the OS X CEF folder name for now (impossible to update so this is okay)
CEF_SOURCE_DIR_OSX="cef_2171_OSX_32"

# get the Windows CEF folder name from the MSVC property panel file
MSVC_PROPS_FILE="./${LLCEFLIB_SOURCE_DIR}/cef.props"
CEF_SOURCE_DIR_WIN=$(sed -n 's:.*<CEF_DIR>\(.*\)</CEF_DIR>.*:\1:p' "${MSVC_PROPS_FILE}")

if [ -z "$AUTOBUILD" ] ; then 
    fail
fi

if [ "$OSTYPE" = "cygwin" ] ; then
    export AUTOBUILD="$(cygpath -u $AUTOBUILD)"
fi

# load autobuild provided shell functions and variables
set +x
eval "$("$AUTOBUILD" source_environment)"
set -x

stage="$(pwd)/stage"

build=${AUTOBUILD_BUILD_ID:=0}

VERSION_HEADER_FILE="$LLCEFLIB_SOURCE_DIR/llceflib.h"
version=$(sed -n -E 's/const std::string LLCEFLIB_BASE_VERSION = "([0-9\.]+)";/\1/p' "${VERSION_HEADER_FILE}")
build=${AUTOBUILD_BUILD_ID:=0}
echo "${version}.${build}" > "${stage}/VERSION.txt"

pushd "$LLCEFLIB_SOURCE_DIR"
    case "$AUTOBUILD_PLATFORM" in
        "windows")
            load_vsvars
            build_sln "llceflib.sln" "Release|Win32"

            cd ..
            mkdir -p "$stage/include/cef"
            mkdir -p "$stage/lib/release"
            mkdir -p "$stage/bin/release"

            cp "$LLCEFLIB_SOURCE_DIR/Release/llceflib.lib" "$stage/lib/release/"
            cp "$LLCEFLIB_SOURCE_DIR/llceflib.h" "$stage/include/cef/"

            cp "$CEF_SOURCE_DIR_WIN/lib/Release/libcef.lib" "$stage/lib/release"
            cp "$CEF_SOURCE_DIR_WIN/lib/Release/libcef_dll_wrapper.lib" "$stage/lib/release"

            cp -R "$CEF_SOURCE_DIR_WIN/Resources/"* "$stage/bin/release/"

            cp "$LLCEFLIB_SOURCE_DIR/Release/llceflib_host.exe" "$stage/bin/release/"

            mkdir -p "$stage/LICENSES"
            cp -R "$LLCEFLIB_SOURCE_DIR/LICENSES" "$stage"
        ;;
        "darwin")
            # xcode project is set up to build in the llcef source folder
            xcodebuild -workspace llceflib.xcworkspace -scheme LLCefLib -configuration Release -derivedDataPath build_mac
            cd ..
            mkdir -p "$stage/include/cef"
            mkdir -p "$stage/lib/release"

            cp "$LLCEFLIB_SOURCE_DIR/build_mac/Build/Products/Release/libLLCefLib.a" "$stage/lib/release/"
            cp "$LLCEFLIB_SOURCE_DIR/llceflib.h" "$stage/include/cef/"
            cp -R "$LLCEFLIB_SOURCE_DIR/build_mac/Build/Products/Release/LLCefLib Helper EH.app" "$stage/lib/release"
            cp -R "$LLCEFLIB_SOURCE_DIR/build_mac/Build/Products/Release/LLCefLib Helper.app" "$stage/lib/release"

            cp "$CEF_SOURCE_DIR_OSX/build/libcef_dll/Release/libcef_dll_wrapper.a" "$stage/lib/release"
            cp -R "$CEF_SOURCE_DIR_OSX/Release/Chromium Embedded Framework.framework" "$stage/lib/release"

            mkdir -p "$stage/LICENSES"
            cp -R "$LLCEFLIB_SOURCE_DIR/LICENSES" "$stage"
        ;;
        "linux")
        ;;
    esac
popd

pass
