cmake_minimum_required(VERSION "3.20")

project("CGNBT")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)

if(MSVC)
    cmake_policy(SET CMP0091 NEW)
    add_compile_definitions(
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

option(CGNBT_HAS_ZLIBNG "Whether to not add zlib-ng dependency." OFF)

if (NOT CGNBT_HAS_ZLIBNG)
    set(ZLIB_ENABLE_TESTS   OFF CACHE BOOL "")
    set(ZLIBNG_ENABLE_TESTS OFF CACHE BOOL "")
    set(WITH_GTEST          OFF CACHE BOOL "")
    add_subdirectory("libs/zlib_ng")
endif()

option(CGNBT_BUILD_TESTS "Whether to build test programs." ON)

if(CGNBT_BUILD_TESTS)
    add_subdirectory("tests")
endif()

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
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} INTERFACE zlibstatic)