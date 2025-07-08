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

#--------------zstd----------------
option(CGNBT_HAS_ZSTD "Whether to not add zstd dependency privately." OFF)

if (NOT CGNBT_HAS_ZSTD)
    set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "")
    set(BUILD_TESTING       OFF CACHE BOOL "")
    set(ZSTD_BUILD_STATIC   ON  CACHE BOOL "")
    set(ZSTD_BUILD_SHARED   OFF CACHE BOOL "")
    if(MSVC)
        set(ZSTD_USE_STATIC_RUNTIME ON CACHE BOOL "")
    endif()
    add_subdirectory("libs/zstd/build/cmake")
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
    "libs/boost/assert/include"
    "libs/boost/config/include"
    "libs/boost/container_hash/include"
    "libs/boost/core/include"
    "libs/boost/describe/include"
    "libs/boost/mp11/include"
    "libs/boost/predef/include"
    "libs/boost/static_assert/include"
    "libs/boost/throw_exception/include"
    "libs/boost/unordered/include"

    "libs/zstd/lib"

    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} INTERFACE libzstd_static)