cmake_minimum_required(VERSION 3.15)
project(JavaQuest CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use an installed raylib (>= 5.0) if there is one, otherwise download it.
find_package(raylib 5.0 QUIET)
if (NOT raylib_FOUND)
  include(FetchContent)
  FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW 1)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(raylib)
endif()

add_executable(javaquest main.cpp)
target_link_libraries(javaquest PRIVATE raylib)
if (WIN32)
  target_link_libraries(javaquest PRIVATE advapi32)
endif()
