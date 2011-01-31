/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <algorithm>
#include <cctype>
#include <cmath>
#include <set>

#include "elements.h"
#include "math.h"
#include "model.h"
#include "part_library.h"

#include "utils.h"

namespace ldraw
{

namespace utils
{

bool _cyclic_reference_test(std::set<std::string> &sets, const model *m, const model *insert = 0L)
{
	std::string mname = translate_string(m->name());

	if (sets.find(mname) != sets.end())
		return true;
	
	sets.insert(mname);

	for (model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		ldraw::type elemtype = (*it)->get_type();
		if (elemtype == ldraw::type_ref) {
			const ldraw::element_ref *l = CAST_AS_CONST_REF(*it);
			if (l->get_model()) {
				if (_cyclic_reference_test(sets, l->get_model()))
					return true;
			}
		}
	}

	if (insert) {
		if (_cyclic_reference_test(sets, insert))
			return true;
	}

	sets.erase(mname);

	return false;
}

bool cyclic_reference_test(const model *m)
{
	std::set<std::string> names;

	return _cyclic_reference_test(names, m);
}

bool cyclic_reference_test(const model *m, const model *insert)
{
	std::set<std::string> names;

	return _cyclic_reference_test(names, m, insert);
}

bool _affected_model(const model *target, const model *m)
{
	for (model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		if ((*it)->get_type() == ldraw::type_ref) {
			const element_ref *r = CAST_AS_CONST_REF(*it);

			if (!r->get_model())
				continue;

			const model *cm = r->get_model();

			if (cm == target)
				return true;
			else if (cm->modeltype() == model::submodel && m->parent() == target->parent()) {
				if (_affected_model(target, cm))
					return true;
			}
		}
	}

	return false;
}

std::list<model *> affected_models(model_multipart *base, ldraw::model *m)
{
	std::list<ldraw::model *> set;

	if (base->main_model() != m && !m->is_submodel_of(base))
		return set;
	
	if (_affected_model(m, base->main_model()))
		set.push_back(base->main_model());

	for (std::map<std::string, model *>::const_iterator it = base->submodel_list().begin(); it != base->submodel_list().end(); ++it) {
		if (_affected_model(m, (*it).second))
			set.push_back((*it).second);
	}

	return set;
}

bool name_duplicate_test(const std::string &name, const model_multipart *model)
{
	return model->submodel_list().find(name) != model->submodel_list().end();
}

bool name_duplicate_test(const std::string &name, const part_library &library)
{
	return library.find(name);
}

void validate_bowtie_quad(element_quadrilateral &quad)
{
	ldraw::vector v12, v14;
	ldraw::vector v34, v32;
	ldraw::vector v41, v43;
	ldraw::vector c1, c3, c4;

	v12 = quad.pos2() - quad.pos1();
	v14 = quad.pos4() - quad.pos1();
	v34 = quad.pos4() - quad.pos3();
	v32 = quad.pos2() - quad.pos3();
	v41 = quad.pos1() - quad.pos4();
	v43 = quad.pos3() - quad.pos4();

	c1 = ldraw::vector::cross_product(v12, v14);
	c3 = ldraw::vector::cross_product(v34, v32);
	c4 = ldraw::vector::cross_product(v41, v43);

	if (ldraw::vector::dot_product(c1, c4) < 0.0f) {
		ldraw::vector temp = quad.pos3();
		quad.pos3() = quad.pos4();
		quad.pos4() = temp;
	} else if (ldraw::vector::dot_product(c3, c4) < 0.0f) {
		ldraw::vector temp = quad.pos2();
		quad.pos2() = quad.pos3();
		quad.pos3() = temp;
	}
}
	
void validate_bowtie_quads(model *model)
{
	for (int i = 0; i < model->size(); ++i) {
		ldraw::type elemtype = model->at(i)->get_type();
		if (elemtype == ldraw::type_quadrilateral) {
			validate_bowtie_quad(*CAST_AS_QUADRILATERAL(model->at(i)));
		} else if (elemtype == ldraw::type_ref) {
			ldraw::element_ref *l = CAST_AS_REF(model->at(i));
			if (l->get_model())
				validate_bowtie_quads(l->get_model());
		}
	}
}

int translate(int c)
{
	if (c == '\\')
		c = '/';

	return std::tolower(c);
}

std::string translate_string(const std::string &str)
{
	std::string nstr = str;

	std::transform(nstr.begin(), nstr.end(), nstr.begin(), translate);

	return nstr;
}

std::string trim_string(const std::string &str)
{
	int x = 0;
	int y = 0;
	
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (*it != ' ' && *it != '\t') break;
		x++;
	}
	if (x == (int)str.length())
		return "";
	for (std::string::const_reverse_iterator it = str.rbegin(); it != str.rend(); ++it) {
		if (*it != '\r' && *it != '\n' && *it != ' ' && *it != '\t')
			break;
		++y;
	}

	return str.substr(x, str.length()-x-y);
}

bool is_stud(const model *model)
{
	return translate_string(model->name()).find("stu") != std::string::npos;
}

bool is_stud(const element_ref *ref)
{
	return translate_string(ref->filename()).find("stu") != std::string::npos;
}

// Determinant.
float det3(const ldraw::matrix &m)
{
	float v = m.value(0, 0)*m.value(1, 1)*m.value(2, 2) + m.value(0, 2)*m.value(2, 1)*m.value(1, 0) + m.value(0, 1)*m.value(1, 2)*m.value(2, 0) -
		m.value(2, 0)*m.value(1, 1)*m.value(0, 2) - m.value(0, 0)*m.value(1, 2)*m.value(2, 1) - m.value(0, 1)*m.value(1, 0)*m.value(2, 2);

	return v;
}

bool is_singular_matrix(const matrix &m)
{
	float d00, d01, d02, d03;

	d00 = m.value(1, 1)*m.value(2, 2)*m.value(3, 3) + m.value(1, 2)*m.value(2, 3)*m.value(3, 1) +
		m.value(1, 3)*m.value(2, 1)*m.value(3, 2) - m.value(3, 1)*m.value(2, 2)*m.value(1, 3) -
		m.value(3, 2)*m.value(2, 3)*m.value(1, 1) - m.value(3, 3)*m.value(2, 1)*m.value(1, 2);
	d01 = m.value(1, 0)*m.value(2, 2)*m.value(3, 3) + m.value(1, 2)*m.value(2, 3)*m.value(3, 0) +
		m.value(1, 3)*m.value(2, 0)*m.value(3, 2) - m.value(3, 0)*m.value(2, 2)*m.value(1, 3) -
		m.value(3, 2)*m.value(2, 3)*m.value(1, 0) - m.value(3, 3)*m.value(2, 0)*m.value(1, 2);
	d02 = m.value(1, 0)*m.value(2, 1)*m.value(3, 3) + m.value(1, 1)*m.value(2, 3)*m.value(3, 0) +
		m.value(1, 3)*m.value(2, 0)*m.value(3, 1) - m.value(3, 0)*m.value(2, 1)*m.value(1, 3) -
		m.value(3, 1)*m.value(2, 3)*m.value(1, 0) - m.value(3, 3)*m.value(2, 0)*m.value(1, 1);
	d03 = m.value(1, 0)*m.value(2, 1)*m.value(3, 2) + m.value(1, 1)*m.value(2, 2)*m.value(3, 0) +
		m.value(1, 2)*m.value(2, 0)*m.value(3, 1) - m.value(3, 0)*m.value(2, 1)*m.value(1, 2) -
		m.value(3, 1)*m.value(2, 2)*m.value(1, 0) - m.value(3, 2)*m.value(2, 0)*m.value(1, 1);

	return std::fabs(m.value(0, 0) * d00 - m.value(0, 1) * d01 + m.value(0, 2) * d02 - m.value(0, 3) * d03) < LDR_EPSILON;
}

bool is_orthogonal(const matrix &m)
{
	float det = det3(m);
	
	return std::abs(std::floor(det) - det) < LDR_EPSILON;
}

}

}
