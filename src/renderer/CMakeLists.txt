project(libldrawrenderer)

set(libldrawrenderer_SOURCES
	mouse_rotation.cpp
	normal_extension.cpp
	renderer_opengl.cpp
)

set(libldrawrenderer_HEADERS
	mouse_rotation.h
	normal_extension.h
	renderer_opengl.h
)

add_library(libldrawrenderer SHARED ${libldrawrenderer_SOURCES})
target_link_libraries(libldrawrenderer libldr ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
set_target_properties(libldrawrenderer PROPERTIES OUTPUT_NAME ldrawrenderer)
set_target_properties(libldrawrenderer PROPERTIES VERSION 0.4.0 SOVERSION 1)

# install
install(TARGETS libldrawrenderer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${libldrawrenderer_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/libldr/renderer)

