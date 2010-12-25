/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include "parameters.h"

namespace ldraw_renderer
{

parameters::parameters()
{
	m_stud_mode = stud_square;
	m_mode = model_full;
	m_vbuffer_criteria = vbuffer_parts;
	m_shading = true;
	m_debug = false;
	m_culling = false; /* disabled for a while */
	m_shader = true;
}

parameters::parameters(const parameters &rhs)
{
	m_stud_mode = rhs.get_stud_rendering_mode();
	m_mode = rhs.get_rendering_mode();
	m_vbuffer_criteria = rhs.get_vbuffer_criteria();
	m_shading = rhs.get_shading();
	m_debug = rhs.get_debug();
	m_culling = rhs.get_culling();
	m_shader = rhs.get_shader();
}

parameters::~parameters()
{

}

}

