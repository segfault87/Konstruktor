project(Konstruktor)

cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

if (MINGW OR UNIX)
  add_definitions(-fexceptions)
endif()

# 3rdparty must be included first to avoid using wrong opengl header
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)
include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(OpenGL REQUIRED)
find_package(Qt5Widgets)
find_package(Sqlite REQUIRED)

if (WIN32)
  add_definitions(-DWIN32)
endif()

if(DEBUG_PROFILE)
  add_definitions(-pg)
endif(DEBUG_PROFILE)

subdirs(src)

