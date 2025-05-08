cmake_minimum_required(VERSION "3.20")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0091 NEW)
if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(GLOB_RECURSE TEST_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable("test" ${TEST_SOURCES})

target_link_libraries("test" PRIVATE CGNBT)

if(MSVC)
	target_compile_options("test" PUBLIC "/ZI")
	target_link_options("test" PUBLIC "/INCREMENTAL")
endif()