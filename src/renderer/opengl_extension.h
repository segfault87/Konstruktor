/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_OPENGL_EXTENSION_H_
#define _RENDERER_OPENGL_EXTENSION_H_

#include <libldr/common.h>

namespace ldraw_renderer
{

class LIBLDR_EXPORT opengl_extension
{
  public:
	typedef void (*func_ptr)();
	
	opengl_extension(const char *name);

	bool is_supported() const;

  protected:
	bool m_supported;

	func_ptr get_glext_proc(const char *procname);
};

}

#endif
