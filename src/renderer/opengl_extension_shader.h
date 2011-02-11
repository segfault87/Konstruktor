/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_OPENGL_EXTENSION_VSHADER_H_
#define _RENDERER_OPENGL_EXTENSION_VSHADER_H_

#include "opengl.h"
#include <libldr/common.h>

#include "opengl_extension.h"

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

	PFNGLCREATESHADERPROC m_glcreateshader;
	PFNGLDELETESHADERPROC m_gldeleteshader;
	PFNGLSHADERSOURCEPROC m_glshadersource;
	PFNGLCOMPILESHADERPROC m_glcompileshader;

	PFNGLCREATEPROGRAMPROC m_glcreateprogram;
	PFNGLDELETEPROGRAMPROC m_gldeleteprogram;
	PFNGLLINKPROGRAMPROC m_gllinkprogram;
	PFNGLUSEPROGRAMPROC m_gluseprogram;
	PFNGLATTACHSHADERPROC m_glattachshader;
	PFNGLDETACHSHADERPROC m_gldetachshader;

	PFNGLGETUNIFORMLOCATIONPROC m_glgetuniformlocation;
	PFNGLGETATTRIBLOCATIONPROC m_glgetattriblocation;

	PFNGLVERTEXATTRIBPOINTERPROC m_glvertexattribpointer;
	PFNGLBINDATTRIBLOCATIONPROC m_glbindattriblocation;
	PFNGLENABLEVERTEXATTRIBARRAYPROC m_glenablevertexattribarray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC m_gldisablevertexattribarray;

	PFNGLUNIFORM1FPROC m_gluniform1f;
	PFNGLUNIFORM2FPROC m_gluniform2f;
	PFNGLUNIFORM3FPROC m_gluniform3f;
	PFNGLUNIFORM4FPROC m_gluniform4f;
	PFNGLUNIFORM1IPROC m_gluniform1i;
	PFNGLUNIFORM2IPROC m_gluniform2i;
	PFNGLUNIFORM3IPROC m_gluniform3i;
	PFNGLUNIFORM4IPROC m_gluniform4i;
	PFNGLUNIFORM1FVPROC m_gluniform1fv;
	PFNGLUNIFORM2FVPROC m_gluniform2fv;
	PFNGLUNIFORM3FVPROC m_gluniform3fv;
	PFNGLUNIFORM4FVPROC m_gluniform4fv;
	PFNGLUNIFORM1IVPROC m_gluniform1iv;
	PFNGLUNIFORM2IVPROC m_gluniform2iv;
	PFNGLUNIFORM3IVPROC m_gluniform3iv;
	PFNGLUNIFORM4IVPROC m_gluniform4iv;
};

}

#endif

