/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_RENDERER_OPENGL_H_
#define _RENDERER_RENDERER_OPENGL_H_

#include <libldr/common.h>

#include <renderer/renderer.h>

namespace ldraw_renderer
{

class parameters;
class renderer;

class LIBLDRAWRENDERER_EXPORT renderer_opengl : public renderer
{
  public:
	renderer_opengl(const parameters *params);
	virtual ~renderer_opengl();
	
	virtual void setup();
};

class LIBLDRAWRENDERER_EXPORT renderer_opengl_factory
{
  public:
	enum rendering_mode { mode_immediate, mode_varray, mode_vbo };
	
	renderer_opengl_factory(const parameters *params, rendering_mode rm = mode_vbo);
	~renderer_opengl_factory();

	void set_rendering_mode(rendering_mode rm) { m_mode = rm; }
	rendering_mode get_rendering_mode() const { return m_mode; }

	renderer_opengl* create_renderer() const;

  private:
	const parameters *m_params;
	rendering_mode m_mode;
};

}

#endif
