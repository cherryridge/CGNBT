cmake_minimum_required(VERSION "3.20")

project("CGNBT")

#------------Basic-----------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "" FORCE)
#----------------------------------

#----------Before Tweaks-----------
if(MSVC)
    add_link_options(/INCREMENTAL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
if(WIN32)
    set(CMAKE_SYSTEM_VERSION 10.0)
    add_compile_definitions(
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
        NTDDI_VERSION=0x0A000000
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
    physfs-static
    libzstd_static
)