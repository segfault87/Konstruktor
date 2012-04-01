 /* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#if defined(__APPLE__)
#include <dlfcn.h>
#endif

#include <cstring>

#include "opengl.h"
#include "opengl_extension.h"

namespace ldraw_renderer
{

#if defined(__APPLE__)
static const char s_opengl_osx_path[] = "/System/Library/Frameworks/OpenGL.framework/Libraries/libGL.dylib";

void* opengl_extension::s_dlhandle = 0L;
#endif

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

#if defined(__APPLE__)
  if (!s_dlhandle) {
    s_dlhandle = dlopen(s_opengl_osx_path, RTLD_GLOBAL);
  }
#endif
}

bool opengl_extension::is_supported() const
{
  return m_supported;
}

opengl_extension::func_ptr opengl_extension::get_glext_proc(const char *procname)
{
#if defined(WIN32)
  return (opengl_extension::func_ptr) wglGetProcAddress(procname);
#elif defined(__APPLE__)
  return (opengl_extension::func_ptr) dlsym(s_dlhandle, procname);
#else
  return glXGetProcAddress((const GLubyte *) procname);
#endif
}

}
