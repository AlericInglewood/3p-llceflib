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

    case "$AUTOBUILD_PLATFORM" in
        "windows")
            pushd "$LLCEFLIB_SOURCE_DIR"
                load_vsvars
                build_sln "llceflib.sln" "Release|Win32"
            popd

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
            pushd "$LLCEFLIB_SOURCE_DIR"
                # xcode project is set up to build in the llcef source folder
                xcodebuild -project llceflib.xcodeproj -scheme LLCefLib -configuration Release -derivedDataPath build_mac
            popd

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
          pushd "$CEF_SOURCE_DIR_LIN64"
              # Build debug version of libcef_dll
              mkdir -p build_debug
              pushd build_debug
                  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
                  make -j8 libcef_dll_wrapper
              popd

              # Build release version of libcef_dll
              mkdir -p build_release
              pushd build_release
                  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
                  make -j8 libcef_dll_wrapper
              popd
          popd

          # Build debug version of libllceflib.a and llceflib_host
          pushd "$LLCEFLIB_SOURCE_DIR"
              mkdir -p build_debug
              pushd build_debug
                  cmake -G "Unix Makefiles" -DARCH:STRING="-m64" -DCEF_BASE=$CEF_SOURCE_DIR_LIN64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
                  make -j8
              popd
          popd
          # Build release version of libllceflib.a and llceflib_host
          pushd "$LLCEFLIB_SOURCE_DIR"
              mkdir -p build_release
              pushd build_release
                  cmake -G "Unix Makefiles" -DARCH:STRING="-m64" -DCEF_BASE=$CEF_SOURCE_DIR_LIN64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
                  make -j8
              popd
          popd

          mkdir -p "$stage/include/cef"
          mkdir -p "$stage/bin/release"
          mkdir -p "$stage/bin/debug"
          mkdir -p "$stage/lib/release"
          mkdir -p "$stage/lib/debug"
          rm -rf "$stage/resources" "$stage/LICENSES"
          mkdir -p "$stage/resources"
          mkdir -p "$stage/LICENSES"

          cp $LLCEFLIB_SOURCE_DIR/build_release/bin/llceflib_host $stage/bin/release
          cp $LLCEFLIB_SOURCE_DIR/build_debug/bin/llceflib_host $stage/bin/debug
          cp $CEF_SOURCE_DIR_LIN64/Release/*_blob.bin $stage/bin/release
          cp $CEF_SOURCE_DIR_LIN64/Debug/*_blob.bin $stage/bin/debug
          cp $LLCEFLIB_SOURCE_DIR/llceflib.h $stage/include/cef
          cp $CEF_SOURCE_DIR_LIN64/build_release/libcef_dll/libcef_dll_wrapper.a $stage/lib/release
          cp $CEF_SOURCE_DIR_LIN64/build_debug/libcef_dll/libcef_dll_wrapper.a $stage/lib/debug
          cp $LLCEFLIB_SOURCE_DIR/build_release/lib/libllceflib.a $stage/lib/release
          cp $LLCEFLIB_SOURCE_DIR/build_debug/lib/libllceflib.a $stage/lib/debug
          cp $CEF_SOURCE_DIR_LIN64/Release/libcef.so $stage/lib/release
          cp $CEF_SOURCE_DIR_LIN64/Debug/libcef.so $stage/lib/debug
          cp -R $CEF_SOURCE_DIR_LIN64/Resources/* $stage/resources
          cp -R $LLCEFLIB_SOURCE_DIR/LICENSES/* $stage/LICENSES
          mv $stage/LICENSES/LICENSE-source.txt $stage/LICENSES/llceflib.txt
          chmod 664 $stage/LICENSES/*

          # Get version for packaging.
          VERSION_HEADER_FILE="$CEF_SOURCE_DIR_LIN64/include/cef_version.h"
          version=$(sed -n -r 's/#define CEF_VERSION "([[:alnum:].]+)"/\1/p' "${VERSION_HEADER_FILE}")
          echo "${version}" > "${stage}/VERSION.txt"
        ;;
    esac

pass
