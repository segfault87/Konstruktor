/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_OPENGL_EXTENSION_VSHADER_H_
#define _RENDERER_OPENGL_EXTENSION_VSHADER_H_

#include <libldr/common.h>

#include "opengl.h"
#include <renderer/opengl_extension.h>

namespace ldraw_renderer
{

class LIBLDRAWRENDERER_EXPORT opengl_extension_shader : public opengl_extension
{
  public:
	static opengl_extension_shader* self();

	opengl_extension_shader();
	
	GLuint glCreateShader(GLenum shaderType);
	void glDeleteShader(GLuint shader);
	void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
	void glCompileShader(GLuint shader);

	GLuint glCreateProgram();
	void glDeleteProgram(GLuint program);
	void glLinkProgram(GLuint program);
	void glUseProgram(GLuint program);
	void glAttachShader(GLuint program, GLuint shader);
	void glDetachShader(GLuint program, GLuint shader);

	GLint glGetUniformLocation(GLuint program, const GLchar *name);	
	GLint glGetAttribLocation(GLuint program, const GLchar *name);

	void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
	void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);
	void glEnableVertexAttribArray(GLuint index);
	void glDisableVertexAttribArray(GLuint index);

	void glUniform1f(GLint location, GLfloat v0);
	void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
	void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void glUniform1i(GLint location, GLint v0);
	void glUniform2i(GLint location, GLint v0, GLint v1);
	void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
	void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	void glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
	void glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
	void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
	void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
	void glUniform1iv(GLint location, GLsizei count, const GLint *value);
	void glUniform2iv(GLint location, GLsizei count, const GLint *value);
	void glUniform3iv(GLint location, GLsizei count, const GLint *value);
	void glUniform4iv(GLint location, GLsizei count, const GLint *value);

  private:
	static opengl_extension_shader *m_instance;

	GLuint (*m_glcreateshader)(GLenum);
	void (*m_gldeleteshader)(GLuint);
	void (*m_glshadersource)(GLuint, GLsizei, const GLchar **, const GLint *);
	void (*m_glcompileshader)(GLuint);

	GLuint (*m_glcreateprogram)();
	void (*m_gldeleteprogram)(GLuint);
	void (*m_gllinkprogram)(GLuint);
	void (*m_gluseprogram)(GLuint);
	void (*m_glattachshader)(GLuint, GLuint);
	void (*m_gldetachshader)(GLuint, GLuint);

	GLint (*m_glgetuniformlocation)(GLuint, const GLchar *);
	GLint (*m_glgetattriblocation)(GLuint, const GLchar *);

	void (*m_glvertexattribpointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
	void (*m_glbindattriblocation)(GLuint, GLuint, const GLchar *);
	void (*m_glenablevertexattribarray)(GLuint);
	void (*m_gldisablevertexattribarray)(GLuint);

	void (*m_gluniform1f)(GLint, GLfloat);
	void (*m_gluniform2f)(GLint, GLfloat, GLfloat);
	void (*m_gluniform3f)(GLint, GLfloat, GLfloat, GLfloat);
	void (*m_gluniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
	void (*m_gluniform1i)(GLint, GLint);
	void (*m_gluniform2i)(GLint, GLint, GLint);
	void (*m_gluniform3i)(GLint, GLint, GLint, GLint);
	void (*m_gluniform4i)(GLint, GLint, GLint, GLint, GLint);
	void (*m_gluniform1fv)(GLint, GLsizei, const GLfloat *);
	void (*m_gluniform2fv)(GLint, GLsizei, const GLfloat *);
	void (*m_gluniform3fv)(GLint, GLsizei, const GLfloat *);
	void (*m_gluniform4fv)(GLint, GLsizei, const GLfloat *);
	void (*m_gluniform1iv)(GLint, GLsizei, const GLint *);
	void (*m_gluniform2iv)(GLint, GLsizei, const GLint *);
	void (*m_gluniform3iv)(GLint, GLsizei, const GLint *);
	void (*m_gluniform4iv)(GLint, GLsizei, const GLint *);
};

}

#endif

