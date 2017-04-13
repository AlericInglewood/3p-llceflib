cmake_minimum_required(VERSION 3.1.0)
project(llceflib)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
include("macros")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_SKIP_RPATH TRUE)
endif()

if(MSVC)
  set(CEF_LIBS debug ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/build/libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib
               optimized ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/build/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib
               debug ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/Debug/libcef.lib
               optimized ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/Release/libcef.lib)
elseif(APPLE)
  set(CEF_LIBS debug ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/libcef_dll_wrapper/Debug/libcef_dll_wrapper.a
               optimized ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/libcef_dll_wrapper/Release/libcef_dll_wrapper.a
               debug "${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/Debug/Chromium Embedded Framework.framework"
               optimized "${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/Release/Chromium Embedded Framework.framework")
elseif(UNIX)
  set(CEF_LIBS debug ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/build_debug/libcef_dll_wrapper/libcef_dll_wrapper.a
               optimized ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/build_release/libcef_dll_wrapper/libcef_dll_wrapper.a
               debug ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/Debug/libcef.so
               optimized ${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE}/Release/libcef.so)
endif()

if(APPLE)
  set(CMAKE_CXX_COMPILER "clang++")
  set(CMAKE_CXX_FLAGS "-x objective-c++")
elseif(UNIX)
  add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_BASE})

set(llceflib_SOURCE_FILES
    llbrowserclient.cpp
    llceflib.cpp
    llceflibimpl.cpp
    llrenderhandler.cpp
    llschemehandler.cpp
)

if(WIN32)
  list(APPEND llceflib_SOURCE_FILES llceflibimpl_win.cpp)
elseif(APPLE)
  list(APPEND llceflib_SOURCE_FILES llceflibimpl_mac.mm)
elseif(UNIX)
  list(APPEND llceflib_SOURCE_FILES llceflibimpl_linux.cpp)
endif(WIN32)

set(llceflib_HEADER_FILES
    llbrowserclient.h
    llceflib.h
    llceflibimpl.h
    llceflibplatform.h
    llrenderhandler.h
    llschemehandler.h
)

if (UNIX AND NOT APPLE)
  add_definitions(-fPIC)
endif (UNIX AND NOT APPLE)

add_library(llceflib STATIC ${llceflib_SOURCE_FILES} ${llceflib_HEADER_FILES})

target_link_libraries(llceflib ${CEF_LIBS})

if (MSVC)
  set_target_properties(llceflib PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif (MSVC)

add_subdirectory(llceflib_host)
if (MSVC)
  add_subdirectory(tests/win32gl)
endif (MSVC)
