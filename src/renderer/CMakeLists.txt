project(libldrawrenderer)

set(libldrawrenderer_SOURCES
	mouse_rotation.cpp
	normal_extension.cpp
	opengl_extension.cpp
	opengl_extension_vbo.cpp
	opengl_extension_shader.cpp
	parameters.cpp
	renderer.cpp
	renderer_opengl.cpp
	renderer_opengl_immediate.cpp
	renderer_opengl_retained.cpp
	vbuffer_extension.cpp
)

set(libldrawrenderer_HEADERS
	mouse_rotation.h
	normal_extension.h
	opengl_extension.h
	opengl_extension_vbo.h
	opengl_extension_shader.h
	parameters.h
	renderer.h
	renderer_opengl.h
	renderer_opengl_immediate.h
	renderer_opengl_retained.h
	vbuffer_extension.h
)
add_definitions(-DMAKE_LIBLDRAWRENDERER_LIB)

add_library(libldrawrenderer SHARED ${libldrawrenderer_SOURCES})
target_link_libraries(libldrawrenderer libldr ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
set_target_properties(libldrawrenderer PROPERTIES OUTPUT_NAME ldrawrenderer)
set_target_properties(libldrawrenderer PROPERTIES VERSION 0.4.0 SOVERSION 1)

# install
install(TARGETS libldrawrenderer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${libldrawrenderer_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/libldr/renderer)

