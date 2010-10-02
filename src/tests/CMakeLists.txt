find_package(GLUT REQUIRED)

# Klotz

set(modelviewer_SRCS
  modelviewer.cpp
)

add_executable(modelviewer ${modelviewer_SRCS})
target_link_libraries(modelviewer libldrawrenderer ${GLUT_LIBRARIES})

