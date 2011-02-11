/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include "opengl_extension_shader.h"

namespace ldraw_renderer
{

opengl_extension_shader* opengl_extension_shader::m_instance = 0L;

opengl_extension_shader* opengl_extension_shader::self()
{
	if (!m_instance)
		m_instance = new opengl_extension_shader();

	return m_instance;
}

opengl_extension_shader::opengl_extension_shader()
	: opengl_extension("GL_ARB_vertex_shader")
{
	if (m_supported) {
		m_glcreateshader = (PFNGLCREATESHADERPROC) get_glext_proc("glCreateShaderObjectARB");
		m_gldeleteshader = (PFNGLDELETESHADERPROC) get_glext_proc("glDeleteObjectARB");
		m_glshadersource = (PFNGLSHADERSOURCEPROC) get_glext_proc("glShaderSourceARB");
		m_glcompileshader = (PFNGLCOMPILESHADERPROC) get_glext_proc("glCompileShaderARB");
		m_glcreateprogram = (PFNGLCREATEPROGRAMPROC) get_glext_proc("glCreateProgramObjectARB");
		m_gldeleteprogram = (PFNGLDELETEPROGRAMPROC) get_glext_proc("glDeleteObjectARB");
		m_gllinkprogram = (PFNGLLINKPROGRAMPROC) get_glext_proc("glLinkProgramARB");
		m_gluseprogram = (PFNGLUSEPROGRAMPROC) get_glext_proc("glUseProgramObjectARB");
		m_glattachshader = (PFNGLATTACHSHADERPROC) get_glext_proc("glAttachObjectARB");
		m_gldetachshader = (PFNGLDETACHSHADERPROC) get_glext_proc("glDetachObjectARB");
		m_glgetuniformlocation = (PFNGLGETUNIFORMLOCATIONPROC) get_glext_proc("glGetUniformLocationARB");
		m_glgetattriblocation = (PFNGLGETATTRIBLOCATIONPROC) get_glext_proc("glGetAttribLocationARB");
		m_glvertexattribpointer = (PFNGLVERTEXATTRIBPOINTERPROC) get_glext_proc("glVertexAttribPointerARB");
		m_glbindattriblocation = (PFNGLBINDATTRIBLOCATIONPROC) get_glext_proc("glBindAttribLocationARB");
		m_glenablevertexattribarray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) get_glext_proc("glEnableVertexAttribArrayARB");
		m_gldisablevertexattribarray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) get_glext_proc("glDisableVertexAttribArrayARB");
		m_gluniform1f = (PFNGLUNIFORM1FPROC) get_glext_proc("glUniform1fARB");
		m_gluniform2f = (PFNGLUNIFORM2FPROC) get_glext_proc("glUniform2fARB");
		m_gluniform3f = (PFNGLUNIFORM3FPROC) get_glext_proc("glUniform3fARB");
		m_gluniform4f = (PFNGLUNIFORM4FPROC) get_glext_proc("glUniform4fARB");
		m_gluniform1i = (PFNGLUNIFORM1IPROC) get_glext_proc("glUniform1iARB");
		m_gluniform2i = (PFNGLUNIFORM2IPROC) get_glext_proc("glUniform2iARB");
		m_gluniform3i = (PFNGLUNIFORM3IPROC) get_glext_proc("glUniform3iARB");
		m_gluniform4i = (PFNGLUNIFORM4IPROC) get_glext_proc("glUniform4iARB");
		m_gluniform1fv = (PFNGLUNIFORM1FVPROC) get_glext_proc("glUniform1fvARB");
		m_gluniform2fv = (PFNGLUNIFORM2FVPROC) get_glext_proc("glUniform2fvARB");
		m_gluniform3fv = (PFNGLUNIFORM3FVPROC) get_glext_proc("glUniform3fvARB");
		m_gluniform4fv = (PFNGLUNIFORM4FVPROC) get_glext_proc("glUniform4fvARB");
		m_gluniform1iv = (PFNGLUNIFORM1IVPROC) get_glext_proc("glUniform1ivARB");
		m_gluniform2iv = (PFNGLUNIFORM2IVPROC) get_glext_proc("glUniform2ivARB");
		m_gluniform3iv = (PFNGLUNIFORM3IVPROC) get_glext_proc("glUniform3ivARB");
		m_gluniform4iv = (PFNGLUNIFORM4IVPROC) get_glext_proc("glUniform4ivARB");
	}
}

GLuint opengl_extension_shader::glCreateShader(GLenum shaderType)
{
	if (m_supported)
		return m_glcreateshader(shaderType);

	return 0;
}

void opengl_extension_shader::glDeleteShader(GLuint shader)
{
	if (m_supported)
		m_gldeleteshader(shader);
}

void opengl_extension_shader::glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
	if (m_supported)
		m_glshadersource(shader, count, string, length);
}

void opengl_extension_shader::glCompileShader(GLuint shader)
{
	if (m_supported)
		m_glcompileshader(shader);
}

GLuint opengl_extension_shader::glCreateProgram()
{
	if (m_supported)
		return m_glcreateprogram();

	return 0;
}

void opengl_extension_shader::glDeleteProgram(GLuint program)
{
	if (m_supported)
		m_gldeleteprogram(program);
}

void opengl_extension_shader::glLinkProgram(GLuint program)
{
	if (m_supported)
		m_gllinkprogram(program);
}

void opengl_extension_shader::glUseProgram(GLuint program)
{
	if (m_supported)
		m_gluseprogram(program);
}

void opengl_extension_shader::glAttachShader(GLuint program, GLuint shader)
{
	if (m_supported)
		m_glattachshader(program, shader);
}

void opengl_extension_shader::glDetachShader(GLuint program, GLuint shader)
{
	if (m_supported)
		m_gldetachshader(program, shader);
}

GLint opengl_extension_shader::glGetUniformLocation(GLuint program, const GLchar *name)
{
	if (m_supported)
		return m_glgetuniformlocation(program, name);

	return -1;
}

GLint opengl_extension_shader::glGetAttribLocation(GLuint program, const GLchar *name)
{
	if (m_supported)
		return m_glgetattriblocation(program, name);

	return -1;
}

void opengl_extension_shader::glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
	if (m_supported)
		m_glvertexattribpointer(index, size, type, normalized, stride, pointer);
}

void opengl_extension_shader::glBindAttribLocation(GLuint program, GLuint index, const GLchar *name)
{
	if (m_supported)
		m_glbindattriblocation(program, index, name);
}

void opengl_extension_shader::glEnableVertexAttribArray(GLuint index)
{
	if (m_supported)
		m_glenablevertexattribarray(index);
}

void opengl_extension_shader::glDisableVertexAttribArray(GLuint index)
{
	if (m_supported)
		m_gldisablevertexattribarray(index);
}

void opengl_extension_shader::glUniform1f(GLint location, GLfloat v0)
{
	if (m_supported)
		m_gluniform1f(location, v0);
}

void opengl_extension_shader::glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
	if (m_supported)
		m_gluniform2f(location, v0, v1);
}

void opengl_extension_shader::glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	if (m_supported)
		m_gluniform3f(location, v0, v1, v2);
}

void opengl_extension_shader::glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	if (m_supported)
		m_gluniform4f(location, v0, v1, v2, v3);
}

void opengl_extension_shader::glUniform1i(GLint location, GLint v0)
{
	if (m_supported)
		m_gluniform1i(location, v0);
}

void opengl_extension_shader::glUniform2i(GLint location, GLint v0, GLint v1)
{
	if (m_supported)
		m_gluniform2i(location, v0, v1);
}

void opengl_extension_shader::glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
	if (m_supported)
		m_gluniform3i(location, v0, v1, v2);
}

void opengl_extension_shader::glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
	if (m_supported)
		m_gluniform4i(location, v0, v1, v2, v3);
}

void opengl_extension_shader::glUniform1fv(GLint location, GLsizei count, const GLfloat *value)
{
	if (m_supported)
		m_gluniform1fv(location, count, value);
}

void opengl_extension_shader::glUniform2fv(GLint location, GLsizei count, const GLfloat *value)
{
	if (m_supported)
		m_gluniform2fv(location, count, value);
}

void opengl_extension_shader::glUniform3fv(GLint location, GLsizei count, const GLfloat *value)
{
	if (m_supported)
		m_gluniform3fv(location, count, value);
}

void opengl_extension_shader::glUniform4fv(GLint location, GLsizei count, const GLfloat *value)
{
	if (m_supported)
		m_gluniform4fv(location, count, value);
}

void opengl_extension_shader::glUniform1iv(GLint location, GLsizei count, const GLint *value)
{
	if (m_supported)
		m_gluniform1iv(location, count, value);
}

void opengl_extension_shader::glUniform2iv(GLint location, GLsizei count, const GLint *value)
{
	if (m_supported)
		m_gluniform2iv(location, count, value);
}

void opengl_extension_shader::glUniform3iv(GLint location, GLsizei count, const GLint *value)
{
	if (m_supported)
		m_gluniform3iv(location, count, value);
}

void opengl_extension_shader::glUniform4iv(GLint location, GLsizei count, const GLint *value)
{
	if (m_supported)
		m_gluniform4iv(location, count, value);
}

}
