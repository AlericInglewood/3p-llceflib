# -*- cmake -*-

project(llceflib)

cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0004 OLD)

set(ARCH "-m32" CACHE STRING "Achitecture (-m32 or -m64)")
if ("${ARCH}" STREQUAL "-m64")
	set(BITS "64")
	message("Building for 64 bits architecture")
else ("${ARCH}" STREQUAL "-m64")
	set(BITS "32")
	message("Building for 32 bits architecture")
endif ("${ARCH}" STREQUAL "-m64")

if (WIN32)
	set(CEF_SOURCES_DIR "cef_2526_WIN_${BITS}")
else (WIN32)
	set(CEF_SOURCES_DIR "cef_2526_LIN_${BITS}")
endif (WIN32)

# IMPORTANT: for Linux, packages/include and packages/lib/release must contain
# the files from the glib v2.32 pre-compiled library: the old gtk/atk/pango/
# glib (v2.28) libraries as provided by LL do not match CEF 3.2454 dependencies
# (depends on glib v2.32 or newer)
# For all OSes, the boost header files used to compile the viewer must also be
# present in packages/include.
set(PACKAGES_INCLUDE_DIR "packages/include")
set(PACKAGES_LIB_DIR "packages/lib/release")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_SOURCES_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../${PACKAGES_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	)

set(llceflib_SOURCE_FILES
	llbrowserclient.cpp
	llceflib.cpp
	llceflibimpl.cpp
	llrenderhandler.cpp
	llschemehandler.cpp
	)

if (WIN32)
	list(APPEND llceflib_SOURCE_FILES llceflibimpl_win.cpp)
else (WIN32)
	list(APPEND llceflib_SOURCE_FILES llceflibimpl_linux.cpp)
endif (WIN32)

set(llceflib_HEADER_FILES
	CMakeLists.txt

	llbrowserclient.h
	llceflib.h
	llceflibimpl.h
	llceflibplatform.h
	llcontexthandler.h
	llrenderhandler.h
	llschemehandler.h
	)

set_source_files_properties(${llceflib_HEADER_FILES}
							PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llceflib_SOURCE_FILES ${llceflib_HEADER_FILES})

if (WIN32)
	add_definitions(-DWINVER=0x0501 -D_WIN32_WINNT=0x501)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /D_NDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_HAS_ITERATOR_DEBUGGING=0")
else (WIN32)
	add_definitions(-D_REENTRANT)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCH} -D_GLIBCXX_USE_CXX11_ABI=0 -std=gnu++11 -fPIC -O2 -fomit-frame-pointer")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCH} -fPIC -O2 -fomit-frame-pointer")
endif (WIN32)

link_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_SOURCES_DIR}/Release
	${CMAKE_CURRENT_SOURCE_DIR}/../${CEF_SOURCES_DIR}/build/libcef_dll
	${CMAKE_CURRENT_SOURCE_DIR}/../${PACKAGES_LIB_DIR}
)

add_library (llceflib STATIC ${llceflib_SOURCE_FILES})
if (WIN32)
	target_link_libraries(llceflib
		libcef_dll_wrapper.lib
 		libcef.lib
		)
else (WIN32)
	target_link_libraries(llceflib
		cef_dll_wrapper
		cef
		)
endif (WIN32)

set(llceflib_host_SOURCE_FILES
	llceflib_host/llceflib_host.cpp
	)

add_executable(llceflib_host ${llceflib_host_SOURCE_FILES})
if (WIN32)
	target_link_libraries(llceflib_host
		libcef_dll_wrapper.lib
 		libcef.lib
		)
else (WIN32)
	target_link_libraries(llceflib_host
		cef_dll_wrapper
 		cef
		)
endif (WIN32)

if (WIN32)
	set_target_properties(llceflib PROPERTIES COMPILE_FLAGS " /MD")
	set_target_properties(llceflib PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
	set_target_properties(llceflib_host PROPERTIES COMPILE_FLAGS "/MT")
	set_target_properties(llceflib_host PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
	#set_target_properties(llceflib_host PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /MANIFEST:NO")
endif (WIN32)
