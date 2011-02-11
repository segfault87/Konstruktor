/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include "opengl_extension_vbo.h"

namespace ldraw_renderer
{

opengl_extension_vbo* opengl_extension_vbo::m_instance = 0L;

opengl_extension_vbo* opengl_extension_vbo::self()
{
	if (!m_instance)
		m_instance = new opengl_extension_vbo();

	return m_instance;
}

opengl_extension_vbo::opengl_extension_vbo()
	: opengl_extension("GL_ARB_vertex_buffer_object")
{
	if (m_supported) {
		m_glgenbuffers = (PFNGLGENBUFFERSPROC) get_glext_proc("glGenBuffersARB");
		m_gldeletebuffers = (PFNGLDELETEBUFFERSPROC) get_glext_proc("glDeleteBuffersARB");
		m_glbindbuffer = (PFNGLBINDBUFFERPROC) get_glext_proc("glBindBufferARB");
		m_glbufferdata = (PFNGLBUFFERDATAPROC) get_glext_proc("glBufferDataARB");
	}
}

void opengl_extension_vbo::glGenBuffers(GLsizei n, GLuint *ids)
{
	if (m_supported)
		m_glgenbuffers(n, ids);
}

void opengl_extension_vbo::glDeleteBuffers(GLsizei n, const GLuint *ids)
{
	if (m_supported)
		m_gldeletebuffers(n, ids);
}

void opengl_extension_vbo::glBindBuffer(GLenum target, GLuint id)
{
	if (m_supported)
		m_glbindbuffer(target, id);
}

void opengl_extension_vbo::glBufferData(GLenum target, GLsizei size, const void *data, GLenum usage)
{
	if(m_supported)
		m_glbufferdata(target, size, data, usage);
}

}

