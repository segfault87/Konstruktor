/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include "elements.h"
#include "model.h"
#include "utils.h"

#include "metrics.h"

namespace ldraw
{

metrics::metrics(model *m, void *arg)
	: extension(m, arg)
{
	m_null = true;
}

metrics::metrics(const vector &min, const vector &max)
	: extension(0L, 0L)
{
	m_null = false;
	m_min = min;
	m_max = max;
}

metrics& metrics::operator=(const metrics &rhs)
{
	m_null = rhs.m_null;
	m_min = rhs.m_min;
	m_max = rhs.m_max;

	return *this;
}

void metrics::update() 
{
	std::stack<matrix> modelview_matrix;

	// set dimension as arbitrary initial value
	m_min = vector(0.0f, 0.0f, 0.0f);
	m_max = vector(0.0f, 0.0f, 0.0f);
	
	// Initialize stack with identity matrix
	modelview_matrix.push(matrix());
	
	// Recurse!
	do_recursive(m_model, &modelview_matrix);
}

void metrics::do_recursive(const model *m, std::stack<matrix> *modelview_matrix, bool orthogonal)
{
	for (model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		type elemtype = (*it)->get_type();
		
		if (elemtype == type_line) {
			element_line *l = CAST_AS_LINE(*it);
			dimension_test(modelview_matrix->top() * l->pos1());
			dimension_test(modelview_matrix->top() * l->pos2());
		} else if (elemtype == type_triangle) {
			element_triangle *l = CAST_AS_TRIANGLE(*it);
			dimension_test(modelview_matrix->top() * l->pos1());
			dimension_test(modelview_matrix->top() * l->pos2());
			dimension_test(modelview_matrix->top() * l->pos3());
		} else if (elemtype == type_quadrilateral) {
			element_quadrilateral *l = CAST_AS_QUADRILATERAL(*it);
			dimension_test(modelview_matrix->top() * l->pos1());
			dimension_test(modelview_matrix->top() * l->pos2());
			dimension_test(modelview_matrix->top() * l->pos3());
			dimension_test(modelview_matrix->top() * l->pos4());
		} else if (elemtype == type_ref) {
			element_ref *l = CAST_AS_REF(*it);
			if (l->get_model()) {
				modelview_matrix->push(modelview_matrix->top() * l->get_matrix());
				
				if (utils::is_stud(l)) {
					// niche optimization: assume a stud as a line.
					dimension_test(modelview_matrix->top() * vector(0.0f, 0.0f, 0.0f));
					dimension_test(modelview_matrix->top() * vector(0.0f, -4.0f, 0.0f));
				} else {
					model *m = l->get_model();
					
					if (orthogonal && utils::is_orthogonal(modelview_matrix->top())) {
						if (!m->custom_data<metrics>())
							m->update_custom_data<metrics>();

						dimension_test(modelview_matrix->top(), *m->custom_data<metrics>());
					} else {
						do_recursive(m, modelview_matrix, false);
					}
				}

				modelview_matrix->pop();
			}
		}
	}
}

void metrics::dimension_test(const vector &v)
{
	if (v.x() < m_min.x())
		m_min.x() = v.x();
	if (v.x() > m_max.x())
		m_max.x() = v.x();
	
	if (v.y() < m_min.y())
		m_min.y() = v.y();
	if (v.y() > m_max.y())
		m_max.y() = v.y();
	
	if (v.z() < m_min.z())
		m_min.z() = v.z();
	if (v.z() > m_max.z())
		m_max.z() = v.z();
}

void metrics::dimension_test(const matrix &transformation, const metrics &m)
{
	vector min = transformation * m.min();
	vector max = transformation * m.max();

	dimension_test(min);
	dimension_test(max);
}

}
