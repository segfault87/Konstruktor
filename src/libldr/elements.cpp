/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */
#include <cmath>

#include "model.h"
#include "part_library.h"

#include "elements.h"

namespace ldraw {

element_ref::element_ref(const color &color, const matrix &matrix, const std::string &filename)
  : element_colored_base(color), m_matrix(matrix), m_model(0L), m_parent(0L), m_linkpoint(0L)
{
	set_filename(filename);
}

element_ref::element_ref(element_ref &rhs)
	: element_colored_base(rhs.get_color()), m_matrix(rhs.get_matrix()),
   m_model(0L), m_parent(0L), m_linkpoint(rhs.linkpoint())
{
	set_filename(rhs.filename());
}

element_ref::~element_ref()
{
	if(m_linkpoint)
		m_linkpoint->unlink_element(this);
}

void element_ref::set_filename(const std::string &s)
{
	m_filename = s;
	
	link();
}

void element_ref::link()
{
	if (!m_parent && !m_linkpoint)
		return;
	
	if (m_linkpoint)
		m_linkpoint->unlink_element(this);

	m_model = 0L;

	model_multipart *mp = 0L;
	if (m_parent)
		mp = m_parent->parent();

	if ((!mp || !mp->link_submodel_element(this)) && m_linkpoint)
		m_linkpoint->link_element(this);
}

void element_ref::operator= (element_ref &rhs)
{
	m_model = 0L;
	m_color = rhs.get_color();
	m_matrix = rhs.get_matrix();
	m_parent = 0L;
	m_linkpoint = rhs.linkpoint();
	set_filename(rhs.filename());
}

void element_line::operator= (const element_line &rhs)
{
	m_color = rhs.get_color();
	m_pos1 = rhs.pos1();
	m_pos2 = rhs.pos2();
}

void element_triangle::operator= (const element_triangle &rhs)
{
	m_color = rhs.get_color();
	m_pos1 = rhs.pos1();
	m_pos2 = rhs.pos2();
	m_pos3 = rhs.pos3();
}

void element_quadrilateral::operator= (const element_quadrilateral &rhs)
{
	m_color = rhs.get_color();
	m_pos1 = rhs.pos1();
	m_pos2 = rhs.pos2();
	m_pos3 = rhs.pos3();
	m_pos4 = rhs.pos4();
}

void element_condline::operator= (const element_condline &rhs)
{
	m_color = rhs.get_color();
	m_pos1 = rhs.pos1();
	m_pos2 = rhs.pos2();
	m_pos3 = rhs.pos3();
	m_pos4 = rhs.pos4();
}

}
