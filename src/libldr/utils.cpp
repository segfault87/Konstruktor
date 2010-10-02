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

std::list<std::string> affected_models(model_multipart *model, const std::string &name)
{
	std::list<std::string> list;

	std::string lname = translate_string(name);

	for (model::const_iterator it = model->main_model()->elements().begin(); it != model->main_model()->elements().end(); ++it) {
		if ((*it)->get_type() == ldraw::type_ref) {
			if (translate_string(CAST_AS_CONST_REF(*it)->filename()) == lname) {
				list.push_back("");
				break;
			}
		}
	}
	for (std::map<std::string, ldraw::model *>::const_iterator it = model->submodel_list().begin(); it != model->submodel_list().end(); ++it) {
		for (model::const_iterator it2 = (*it).second->elements().begin(); it2 != (*it).second->elements().end(); ++it2) {
			if ((*it2)->get_type() == ldraw::type_ref) {
				if (translate_string(CAST_AS_CONST_REF(*it2)->filename()) == lname) {
					list.push_back((*it).second->name());
					break;
				}
			}
		}
	}

	return list;
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
	float angle;
	
	angle = vector::get_angle(quad.pos2() - quad.pos1(), quad.pos4() - quad.pos1());
	angle += vector::get_angle(quad.pos1() - quad.pos4(), quad.pos3() - quad.pos4());
	angle += vector::get_angle(quad.pos4() - quad.pos3(), quad.pos2() - quad.pos3());
	angle += vector::get_angle(quad.pos1() - quad.pos2(), quad.pos3() - quad.pos2());
	
	if (fabs(angle-M_PI*2.0f) > LDR_EPSILON) {
		// swap
		ldraw::vector tmp = quad.pos3();
		quad.pos3() = quad.pos4();
		quad.pos4() = tmp;
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

}

}
