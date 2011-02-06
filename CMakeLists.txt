project(Konstruktor)

if (UNIX)
add_definitions(${CMAKE_CXX_FLAGS_DEBUG} -fexceptions)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(KDE4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Sqlite REQUIRED)

if (WIN32)
find_package(Kdewin REQUIRED)
endif()

include_directories(${KDE4_INCLUDES})

if (WIN32)
add_definitions(-DWIN32)
endif()

if(DEBUG_PROFILE)
add_definitions(-pg)
endif(DEBUG_PROFILE)

subdirs(src)

