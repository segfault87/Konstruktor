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

// Determinant.
float renderer::det3(const ldraw::matrix &m) const
{
	float v = m.value(0, 0)*m.value(1, 1)*m.value(2, 2) + m.value(0, 2)*m.value(2, 1)*m.value(1, 0) + m.value(0, 1)*m.value(1, 2)*m.value(2, 0) -
		m.value(2, 0)*m.value(1, 1)*m.value(0, 2) - m.value(0, 0)*m.value(1, 2)*m.value(2, 1) - m.value(0, 1)*m.value(1, 0)*m.value(2, 2);

	return v;
}

}
