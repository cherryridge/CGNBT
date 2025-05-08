cmake_minimum_required(VERSION "3.20")

project("CGNBT")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0091 NEW)
if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (NOT DEFINED ZLIBNG_ROOT)
	set(ZLIBNG_ROOT "libs/zlib-ng" CACHE PATH "Folder for zlib-ng dependency.")
endif()

set(ZLIB_ENABLE_TESTS   OFF CACHE BOOL "")
set(ZLIBNG_ENABLE_TESTS OFF CACHE BOOL "")
set(WITH_GTEST          OFF CACHE BOOL "")

add_subdirectory(${ZLIBNG_ROOT})

add_subdirectory("tests")

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME} INTERFACE zlibstatic)