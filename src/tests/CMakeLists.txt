find_package(GLUT REQUIRED)

# Model viewer

set(modelviewer_SRCS
  modelviewer.cpp
  modelviewer_glut.cpp
)

add_executable(modelviewer ${modelviewer_SRCS})
target_link_libraries(modelviewer libldrawrenderer ${GLUT_LIBRARIES})

set(modelviewer_qt_SRCS
  modelviewer.cpp
  modelviewer_qt.cpp
)

add_executable(modelviewer_qt ${modelviewer_qt_SRCS})
target_link_libraries(modelviewer_qt libldrawrenderer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY})