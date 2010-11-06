project(Konstruktor)

add_definitions(${CMAKE_CXX_FLAGS_DEBUG} -fexceptions)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(KDE4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Sqlite REQUIRED)

include_directories(${KDE4_INCLUDES})

if(DEBUG_PROFILE)
add_definitions(-pg)
endif(DEBUG_PROFILE)

subdirs(src)

