project(Konstruktor)

if (MSVC)
message(STATUS ${CMAKE_CXX_FLAGS})
endif()
if (MINGW)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
endif()

if (UNIX)
add_definitions(${CMAKE_CXX_FLAGS_DEBUG} -fexceptions)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(OpenGL REQUIRED)
find_package(Qt4 4.5.0 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)
find_package(KDE4)
find_package(Sqlite REQUIRED)

if (KDE4_FOUND AND WIN32)
find_package(Kdewin REQUIRED)
endif()

if(KDE4_FOUND)
include_directories(${KDE4_INCLUDES})
else(KDE4_FOUND)
set(NO_KDE)
add_definitions(-DKONSTRUKTOR_NO_KDE)
endif(KDE4_FOUND)

if (WIN32)
add_definitions(-DWIN32)
endif()

if(DEBUG_PROFILE)
add_definitions(-pg)
endif(DEBUG_PROFILE)

subdirs(src)

