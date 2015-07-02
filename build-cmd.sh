#!/bin/bash

cd "$(dirname "$0")"

# turn on verbose debugging output for parabuild logs.
set -x
# make errors fatal
set -e

LLCEFLIB_VERSION="1.0.0"
LLCEFLIB_SOURCE_DIR="src"
CEF_SOURCE_DIR_OSX="cef_2171_OSX_32"
CEF_SOURCE_DIR_WIN="cef_2272_WIN_32"

CEF_VERSION_WIN="CEF-WIN-3.2272.gbda8dc7-32"
CEF_VERSION_OSX="CEF-OSX-3.2171.2069-32"

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

pushd "$LLCEFLIB_SOURCE_DIR"
    case "$AUTOBUILD_PLATFORM" in
        "windows")
            # version number combines LLCefLib version & CEF version/bit width
            echo "${LLCEFLIB_VERSION}.${CEF_VERSION_WIN}.${build}" > "${stage}/VERSION.txt"

            load_vsvars
            build_sln "llceflib.sln" "Release|Win32"

            cd ..
            mkdir -p "$stage/include/cef"
            mkdir -p "$stage/lib/release"
            mkdir -p "$stage/bin/release"
            mkdir -p "$stage/Resources"

            cp "$LLCEFLIB_SOURCE_DIR/Release/llceflib.lib" "$stage/lib/release/"
            cp "$LLCEFLIB_SOURCE_DIR/llceflib.h" "$stage/include/cef/"

            cp "$CEF_SOURCE_DIR_WIN/Release/libcef.lib" "$stage/lib/release"
            cp "$CEF_SOURCE_DIR_WIN/build/libcef_dll/Release/libcef_dll_wrapper.lib" "$stage/lib/release"
            cp -R "$CEF_SOURCE_DIR_WIN/Resources/" "$stage/"

            cp "$CEF_SOURCE_DIR_WIN/Release/d3dcompiler_43.dll" "$stage/bin/release"
            cp "$CEF_SOURCE_DIR_WIN/Release/d3dcompiler_47.dll" "$stage/bin/release"
            cp "$CEF_SOURCE_DIR_WIN/Release/ffmpegsumo.dll" "$stage/bin/release"
            cp "$CEF_SOURCE_DIR_WIN/Release/libcef.dll" "$stage/bin/release"
            cp "$CEF_SOURCE_DIR_WIN/Release/libEGL.dll" "$stage/bin/release"
            cp "$CEF_SOURCE_DIR_WIN/Release/libGLESv2.dll" "$stage/bin/release"
            cp "$CEF_SOURCE_DIR_WIN/Release/pdf.dll" "$stage/bin/release"
            cp "$CEF_SOURCE_DIR_WIN/Release/wow_helper.exe" "$stage/bin/release"
            cp "$LLCEFLIB_SOURCE_DIR/Release/llceflib_host.exe" "$stage/bin/release/"

            mkdir -p "$stage/LICENSES"
            cp -R "$LLCEFLIB_SOURCE_DIR/LICENSES" "$stage"
        ;;
        "darwin")
            # version number combines LLCefLib version & CEF version/bit width
            echo "${LLCEFLIB_VERSION}.${CEF_VERSION_OSX}.${build}" > "${stage}/VERSION.txt"

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
