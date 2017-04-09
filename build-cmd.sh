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

# hard code the linux64 folder name
CEF_SOURCE_DIR_LIN64="cef_2526_LIN_64"

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
            mkdir -p "$stage/resources"
            mkdir -p "$stage/LICENSES"

            # LLCefLib files
            cp "$LLCEFLIB_SOURCE_DIR/Release/llceflib.lib" "$stage/lib/release/"
            cp "$LLCEFLIB_SOURCE_DIR/llceflib.h" "$stage/include/cef/"
            cp "$LLCEFLIB_SOURCE_DIR/Release/llceflib_host.exe" "$stage/bin/release/"

            # CEF libraries
            cp "$CEF_SOURCE_DIR_WIN/lib/Release/libcef.lib" "$stage/lib/release"
            cp "$CEF_SOURCE_DIR_WIN/lib/Release/libcef_dll_wrapper.lib" "$stage/lib/release"

            # CEF run time binaries
            cp "$CEF_SOURCE_DIR_WIN/bin/release/"* "$stage/bin/release/"

            # CEF resources (common to deubg/release)
            cp -R "$CEF_SOURCE_DIR_WIN/resources/"* "$stage/resources/"

            # licenses
            cp -R "$LLCEFLIB_SOURCE_DIR/LICENSES" "$stage"
        ;;
        "darwin")
            # xcode project is set up to build in the llcef source folder
            xcodebuild -project llceflib.xcodeproj -scheme LLCefLib -configuration Release -derivedDataPath build_mac
            cd ..
            mkdir -p "$stage/include/cef"
            mkdir -p "$stage/lib/release"

            cp "$LLCEFLIB_SOURCE_DIR/build/Release/libLLCefLib.a" "$stage/lib/release/"
            cp "$LLCEFLIB_SOURCE_DIR/llceflib.h" "$stage/include/cef/"
            cp -R "$LLCEFLIB_SOURCE_DIR/llceflib_host/mac/build/Release/LLCefLib Helper EH.app" "$stage/lib/release"
            cp -R "$LLCEFLIB_SOURCE_DIR/llceflib_host/mac/build/Release/LLCefLib Helper.app" "$stage/lib/release"

            cp "$CEF_SOURCE_DIR_OSX/build/libcef_dll/Release/libcef_dll_wrapper.a" "$stage/lib/release"
            cp -R "$CEF_SOURCE_DIR_OSX/Release/Chromium Embedded Framework.framework" "$stage/lib/release"

            mkdir -p "$stage/LICENSES"
            cp -R "$LLCEFLIB_SOURCE_DIR/LICENSES" "$stage"
        ;;
        "linux64")
          cd ..

          # Build libcef_dll
          pushd "$CEF_SOURCE_DIR_LIN64"
          mkdir -p build
          pushd build
          cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
          make -j6 libcef_dll_wrapper
          popd
          popd

          # Build libllceflib.a and llceflib_host
          pushd "$LLCEFLIB_SOURCE_DIR"
          mkdir -p build
          pushd build
          cmake -G "Unix Makefiles" -DARCH:STRING="-m64" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
          make
          popd
          popd

          mkdir -p "$stage/include/cef"
          mkdir -p "$stage/bin/release"
          mkdir -p "$stage/lib/release"
          rm -rf "$stage/resources" "$stage/LICENSES"
          mkdir -p "$stage/resources"
          mkdir -p "$stage/LICENSES"

          cp $LLCEFLIB_SOURCE_DIR/build/llceflib_host $stage/bin/release
          cp $CEF_SOURCE_DIR_LIN64/Release/*_blob.bin $stage/bin/release
          cp $LLCEFLIB_SOURCE_DIR/llceflib.h $stage/include/cef
          cp $CEF_SOURCE_DIR_LIN64/build/libcef_dll/libcef_dll_wrapper.a $stage/lib/release
          cp $LLCEFLIB_SOURCE_DIR/build/libllceflib.a $stage/lib/release
          cp $CEF_SOURCE_DIR_LIN64/Release/libcef.so $stage/lib/release
          cp -R $CEF_SOURCE_DIR_LIN64/Resources/* $stage/resources
          cp -R $LLCEFLIB_SOURCE_DIR/LICENSES/* $stage/LICENSES
          mv $stage/LICENSES/LICENSE-source.txt $stage/LICENSES/llceflib.txt
          chmod 664 $stage/LICENSES/*

          VERSION_HEADER_FILE="$CEF_SOURCE_DIR_LIN64/include/cef_version.h"
          version=$(sed -n -r 's/#define CEF_VERSION "([[:alnum:].]+)"/\1/p' "${VERSION_HEADER_FILE}")
          echo "${version}" > "${stage}/VERSION.txt"
        ;;
    esac
popd

pass
