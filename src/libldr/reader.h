/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_READER_H_
#define _LIBLDR_READER_H_

#include <string>

#include "common.h"

namespace ldraw
{

class element_base;
class model;
class model_multipart;

class LIBLDR_EXPORT reader
{
  public:
	reader();
	reader(const std::string &basepath);
	
	model_multipart* load_from_file(const std::string &name) const;
	static model_multipart* load_from_stream(std::istream &stream, std::string name = "");
	static element_base* parse_line(const std::string &command, model *m = 0L);
	
	const std::string& basepath() const { return m_basepath; }
	void set_basepath(const std::string &path) { m_basepath = path; }
	
  private:
	static bool parse_stream(model *m, std::istream &stream, bool multipart, std::string *keyname = 0L);
	
	std::string m_basepath;
};

}

#endif
