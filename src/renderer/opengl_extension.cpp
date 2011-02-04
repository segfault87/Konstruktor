/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <cstring>

#include <GL/gl.h>

#if defined(WIN32)
#include <wingdi.h>
#else
#include <GL/glx.h>
#endif

#include "opengl_extension.h"

namespace ldraw_renderer
{

opengl_extension::opengl_extension(const char *name)
{
	const GLubyte *str = glGetString(GL_EXTENSIONS);

	if (!str) {
		m_supported = false;
		return;
	}

	if (std::strstr((const char *)str, name) != NULL)
		m_supported = true;
	else
		m_supported = false;
}

bool opengl_extension::is_supported() const
{
	return m_supported;
}

opengl_extension::func_ptr opengl_extension::get_glext_proc(const char *procname)
{
#if defined(WIN32)
	return wglGetProcAddress(procname);
#else
	return glXGetProcAddress((const GLubyte *)procname);
#endif
}

}
