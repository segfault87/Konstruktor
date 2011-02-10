/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_FILTER_H_
#define _LIBLDR_FILTER_H_

#include "common.h"

namespace ldraw
{

class model;

class LIBLDR_EXPORT filter
{
  public:
	virtual ~filter() {}
	
	virtual bool query(const model *m, int index, int depth) const = 0;
};	

}

#endif
