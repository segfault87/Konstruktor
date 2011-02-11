/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_PARAMETERS_H_
#define _RENDERER_PARAMETERS_H_

#include <libldr/common.h>

namespace ldraw_renderer
{

class LIBLDRAWRENDERER_EXPORT parameters
{
  public:
	enum stud_rendering_mode { stud_regular, stud_line, stud_square };
	enum render_method { model_full, model_edges, model_boundingboxes };
	enum vbuffer_criteria { vbuffer_everything, vbuffer_submodels, vbuffer_parts, vbuffer_primitives };
	
	parameters();
	parameters(const parameters &rhs);
	~parameters();

	stud_rendering_mode get_stud_rendering_mode() const { return m_stud_mode; }
	render_method get_rendering_mode() const { return m_mode; }
	vbuffer_criteria get_vbuffer_criteria() const { return m_vbuffer_criteria; }
	bool get_shading() const { return m_shading; }
	bool get_debug() const { return m_debug; }
	bool get_culling() const { return m_culling; }
	bool get_shader() const { return m_shader; }

	void set_stud_rendering_mode(stud_rendering_mode s) { m_stud_mode = s; }
	void set_rendering_mode(render_method m) { m_mode = m; }
	void set_vbuffer_criteria(vbuffer_criteria v) { m_vbuffer_criteria = v; }
	void set_shading(bool b) { m_shading = b; }
	void set_debug(bool b) { m_debug = b; }
	void set_culling(bool b) { m_culling = b; }
	void set_shader(bool b) { m_shader = b; }

  private:
	stud_rendering_mode m_stud_mode;
	render_method m_mode;
	vbuffer_criteria m_vbuffer_criteria;
	bool m_shading;
	bool m_debug;
	bool m_culling;
	bool m_shader;
};

}

#endif
