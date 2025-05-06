cmake_minimum_required(VERSION "3.20")

project("CGNBT")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0091 NEW)
if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(GLOB_RECURSE CGNBT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${CGNBT_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")

option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
	add_subdirectory("tests")
endif()

if(MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
	target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()