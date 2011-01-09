/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <renderer/parameters.h>

#include "renderer.h"

namespace ldraw_renderer
{

renderer::renderer(const parameters *rp)
{
	set_base_color(ldraw::color(7));
	
	m_params = rp;
	m_selection = selection_points;
}

renderer::~renderer()
{

}

void renderer::set_base_color(const ldraw::color &c)
{
	while (!m_colorstack.empty())
		m_colorstack.pop();
	
	m_colorstack.push(c);
}

void renderer::set_selection_type(selection s)
{
	m_selection = s;
}

void renderer::setup()
{

}

// Get current color
const unsigned char* renderer::get_color(const ldraw::color &c) const
{
	if (c.get_id() == 16)
		return m_colorstack.top().get_entity()->rgba;
	else if (c.get_id() == 24)
		return m_colorstack.top().get_entity()->complement;
	else
		return c.get_entity()->rgba;
}

}
