/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_OPENGL_EXTENSION_VBO_H_
#define _RENDERER_OPENGL_EXTENSION_VBO_H_

#include <GL/gl.h>

#include <libldr/common.h>

#include <renderer/opengl_extension.h>

namespace ldraw_renderer
{

class LIBLDR_EXPORT opengl_extension_vbo : public opengl_extension
{
  public:
	static opengl_extension_vbo* self();

	opengl_extension_vbo();

	void glGenBuffers(GLsizei n, GLuint *ids);
	void glDeleteBuffers(GLsizei n, const GLuint *ids);
	void glBindBuffer(GLenum target, GLuint id);
	void glBufferData(GLenum target, GLsizei size, const void *data, GLenum usage);

  private:
	static opengl_extension_vbo *m_instance;

	void (*m_glgenbuffers)(GLsizei, GLuint *);
	void (*m_gldeletebuffers)(GLsizei, const GLuint *);
	void (*m_glbindbuffer)(GLenum, GLuint);
	void (*m_glbufferdata)(GLenum, GLsizei, const void *, GLenum);
};

}

#endif


