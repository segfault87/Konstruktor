/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <libldr/elements.h>
#include <libldr/model.h>

#include "normal_extension.h"

namespace ldraw_renderer
{

normal_extension::normal_extension(ldraw::model *m, void *arg)
	: extension(m, arg)
{

}

normal_extension::~normal_extension()
{

}

void normal_extension::update()
{
	int i = 0;

	m_normals.clear();
	
	for (ldraw::model::const_iterator it = m_model->elements().begin(); it != m_model->elements().end(); ++it) {
		ldraw::vector v1, v2, v3;
		ldraw::vector nvec;

		switch ((*it)->get_type()) {
			case ldraw::type_triangle:
			{
				const ldraw::element_triangle *t = CAST_AS_CONST_TRIANGLE(*it);
				nvec = calculate_normal(t->pos1(), t->pos2(), t->pos3());
				break;
			}
			case ldraw::type_quadrilateral:
			{
				const ldraw::element_quadrilateral *t = CAST_AS_CONST_QUADRILATERAL(*it);
				nvec = calculate_normal(t->pos1(), t->pos2(), t->pos3());
				break;
			}
			default:
				++i;
				continue;
		}

		m_normals[i++] = nvec;
	}
}

bool normal_extension::has_normal(int idx) const
{
	return m_normals.find(idx) != m_normals.end();
}

ldraw::vector normal_extension::normal(int idx) const
{
	if (has_normal(idx))
		return (*m_normals.find(idx)).second;
	else
		return ldraw::vector();
}

const std::map<int, ldraw::vector>& normal_extension::normals() const
{
	return m_normals;
}

// Calculates normal vector
ldraw::vector normal_extension::calculate_normal(const ldraw::vector &v1, const ldraw::vector &v2, const ldraw::vector &v3)
{
	ldraw::vector d1, d2, out;
	
	d1 = v2 - v1;
	d2 = v3 - v2;
	
	out = ldraw::vector::cross_product(d1, d2).normalize();
	
	return out;
}

}
