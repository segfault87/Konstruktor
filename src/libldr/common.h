/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_COMMON_H_
#define _LIBLDR_COMMON_H_

#ifdef WIN32
#define LIBLDR_EXPORT __declspec(dllexport)
#define DIRECTORY_SEPARATOR "\\"
#else
#define LIBLDR_EXPORT __attribute__ ((visibility("default")))
#define DIRECTORY_SEPARATOR "/"
#endif

#include "exception.h"

#endif
