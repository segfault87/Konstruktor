/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_UTILS_H_
#define _LIBLDR_UTILS_H_

#include <list>
#include <string>

#include "common.h"

namespace ldraw
{

class element_ref;
class element_quadrilateral;
class model;
class model_multipart;
class part_library;

namespace utils
{

// Cyclic reference test
LIBLDR_EXPORT bool cyclic_reference_test(const model *m);
LIBLDR_EXPORT bool cyclic_reference_test(const model *m, const model *insert);

// Returns a list of submodels (including model) that can be affected by a model
LIBLDR_EXPORT std::list<std::string> affected_models(model_multipart *model, const std::string &name);

// Name duplicate test
LIBLDR_EXPORT bool name_duplicate_test(const std::string &name, const model_multipart *model);
LIBLDR_EXPORT bool name_duplicate_test(const std::string &name, const part_library &library);

// Fix bowtie quads
LIBLDR_EXPORT void validate_bowtie_quad(element_quadrilateral &quad);
LIBLDR_EXPORT void validate_bowtie_quads(model *model);

// String handling
LIBLDR_EXPORT std::string translate_string(const std::string &str);
LIBLDR_EXPORT std::string trim_string(const std::string &str);

// Miscellaneous
LIBLDR_EXPORT bool is_stud(const model *m);
LIBLDR_EXPORT bool is_stud(const element_ref *ref);

}

}

#endif
