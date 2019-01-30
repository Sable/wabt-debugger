cmake_minimum_required(VERSION 3.5)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(wdb)
set(WDB ${PROJECT_NAME})

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compile with -std=c++11
add_compile_options(-std=c++11)

# Inlucde header files
set(WDB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${WDB_INCLUDE_DIRS})

# Add wabt project
set(BUILD_TESTS OFF CACHE BOOL "Disable gtest in wabt" FORCE)
add_subdirectory(lib/wabt)

# Add wabt header files
set(WABT_INCLUDE_DIRS ${WABT_SOURCE_DIR}/../ ${WABT_BINARY_DIR}/../ ${WABT_SOURCE_DIR} ${WABT_BINARY_DIR})
include_directories(${WABT_INCLUDE_DIRS})  # Include <src/...> as required by wabt, but for clarity only also include <wabt/src/...>

# Expose include dirs for projects using wdb
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
set(WDB_INCLUDE_DIRS ${WDB_INCLUDE_DIRS} ${WABT_INCLUDE_DIRS} PARENT_SCOPE)
endif()

# Generate executable
file(GLOB_RECURSE PROJECT_SOURCE_FILES src/*.cpp src/*/*.cpp)

# Add wabt dependency
add_library(${WDB} ${PROJECT_SOURCE_FILES})

# Link libraries to the
target_link_libraries(${WDB} wabt)
