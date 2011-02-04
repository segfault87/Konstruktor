/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_MOUSE_ROTATION_H_
#define _RENDERER_MOUSE_ROTATION_H_

#include <libldr/math.h>

namespace ldraw_renderer
{

class LIBLDRAWRENDERER_EXPORT mouse_rotation
{
  public:
	static const ldraw::matrix isometric_projection_matrix;

	mouse_rotation();
	mouse_rotation(const ldraw::matrix &init);
	~mouse_rotation() {}

	const ldraw::matrix& get_matrix() { return m_current; }

	void press_event(int px, int py, int w, int h);
	void move_event(int px, int py, int w, int h);

  private:
	ldraw::vector map_to_sphere(int px, int py, int w, int h);
	
	ldraw::matrix m_current;
	ldraw::matrix m_last;
	
	ldraw::vector m_stvec;
	ldraw::vector m_envec;
};

}

#endif
