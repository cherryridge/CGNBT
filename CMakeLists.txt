cmake_minimum_required(VERSION "4.0")

project("CGNBT")

#------------Basic-----------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "" FORCE)
#----------------------------------

#----------Before Tweaks-----------
add_library(ScopedFlags_CGNBT INTERFACE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    message("Using MSVC / clang-cl")
    target_compile_options(ScopedFlags_CGNBT INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>
        $<$<COMPILE_LANGUAGE:CXX>:/Zc:preprocessor>
        $<$<COMPILE_LANGUAGE:CXX>:/bigobj>
        $<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # I got a linker error about "__setusermatherr is replaced" when I link using lld and I have no idea why. This is a black magic workaround that seems to work: use MSVC.
    set(CMAKE_LINKER_TYPE "MSVC" CACHE STRING "" FORCE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message("Using clang / clang-cl")
    add_compile_options(
        # I hate clang
        # —Yes I know about `-Wno-error` but I don't want to disable all warnings as errors.
        # If you run into some warnings that does not exist in this list, please add it here via PR.
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-command-line-argument>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unqualified-std-cast-call>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-macro-redefined>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-missing-field-initializers>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-invalid-offsetof>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-function>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-private-field>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-but-set-variable>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-missing-braces>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-macro-redefined>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-deprecated-declarations>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-microsoft-void-pseudo-dtor>
    )
endif()

if(WIN32)
    # Override everything to Windows 10 SDK
    add_compile_options(
        $<$<COMPILE_LANGUAGE:C,CXX>:/U_WIN32_WINNT>
        $<$<COMPILE_LANGUAGE:C,CXX>:/UWINVER>
        $<$<COMPILE_LANGUAGE:C,CXX>:/UNTDDI_VERSION>
    )
    add_compile_definitions(
        $<$<COMPILE_LANGUAGE:C,CXX>:WIN32_LEAN_AND_MEAN>
        $<$<COMPILE_LANGUAGE:C,CXX>:NOMINMAX>
        $<$<COMPILE_LANGUAGE:C,CXX>:_WIN32_WINNT=0x0A00>
        $<$<COMPILE_LANGUAGE:C,CXX>:WINVER=0x0A00>
        $<$<COMPILE_LANGUAGE:C,CXX>:NTDDI_VERSION=0x0A000000>
    )
endif()
#----------------------------------

#------------Clangd----------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
#----------------------------------

#-------------Boost----------------
option(CGNBT_HAS_BOOST "Whether to not add Boost dependency privately." OFF)

if (NOT CGNBT_HAS_BOOST)
    set(CGNBT_BOOST_DIRECTORY "libs/boost" CACHE STRING "")
endif()
#----------------------------------

#------------Physfs----------------
option(CGNBT_HAS_PHYSFS "Whether to not add Physfs dependency privately." OFF)

if(NOT CGNBT_HAS_PHYSFS)
    set(CGNBT_PHYSFS_DIRECTORY "libs/physfs" CACHE STRING "")

    set(PHYSFS_ARCHIVE_ZIP         ON  CACHE BOOL "")
    set(PHYSFS_ARCHIVE_7Z          ON  CACHE BOOL "")
    set(PHYSFS_ARCHIVE_GRP         OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_WAD         OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_CSM         OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_HOG         OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_MVL         OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_QPAK        OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_SL          OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_ISO9660     OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_VDF         OFF CACHE BOOL "")
    set(PHYSFS_ARCHIVE_LECARCHIVES OFF CACHE BOOL "")
    set(PHYSFS_BUILD_STATIC        ON  CACHE BOOL "")
    set(PHYSFS_BUILD_SHARED        OFF CACHE BOOL "")
    set(PHYSFS_BUILD_TEST          OFF CACHE BOOL "")
    set(PHYSFS_DISABLE_INSTALL     ON  CACHE BOOL "")
    set(PHYSFS_BUILD_DOCS          OFF CACHE BOOL "")
    if(UNIX)
        set(PHYSFS_TARGETNAME_DIST "physfs-dist-please-do-not-collide-with-anything-ok" CACHE STRING "")
    endif()
    add_subdirectory(${CGNBT_PHYSFS_DIRECTORY})
endif()
#----------------------------------

#--------------Zstd----------------
option(CGNBT_HAS_ZSTD "Whether to not add Zstd dependency privately." OFF)

if (NOT CGNBT_HAS_ZSTD)
    set(CGNBT_ZSTD_DIRECTORY "libs/zstd" CACHE STRING "")

    set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "")
    set(BUILD_TESTING       OFF CACHE BOOL "")
    set(ZSTD_BUILD_STATIC   ON  CACHE BOOL "")
    set(ZSTD_BUILD_SHARED   OFF CACHE BOOL "")
    if(MSVC)
        set(ZSTD_USE_STATIC_RUNTIME ON CACHE BOOL "")
    endif()
    add_subdirectory("${CGNBT_ZSTD_DIRECTORY}/build/cmake")
endif()
#----------------------------------

#--------------Tests---------------
option(CGNBT_BUILD_TESTS "Whether to build test programs." ON)

if(CGNBT_BUILD_TESTS)
    add_subdirectory("tests")
endif()
#----------------------------------

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
    "${CGNBT_BOOST_DIRECTORY}/assert/include"
    "${CGNBT_BOOST_DIRECTORY}/config/include"
    "${CGNBT_BOOST_DIRECTORY}/container_hash/include"
    "${CGNBT_BOOST_DIRECTORY}/core/include"
    "${CGNBT_BOOST_DIRECTORY}/describe/include"
    "${CGNBT_BOOST_DIRECTORY}/mp11/include"
    "${CGNBT_BOOST_DIRECTORY}/predef/include"
    "${CGNBT_BOOST_DIRECTORY}/static_assert/include"
    "${CGNBT_BOOST_DIRECTORY}/throw_exception/include"
    "${CGNBT_BOOST_DIRECTORY}/unordered/include"

    "${CGNBT_PHYSFS_DIRECTORY}/src"
    "${CGNBT_ZSTD_DIRECTORY}/lib"

    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} INTERFACE
    ScopedFlags_CGNBT

    physfs-static
    libzstd_static
)