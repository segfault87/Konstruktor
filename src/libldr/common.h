/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_COMMON_H_
#define _LIBLDR_COMMON_H_

#ifdef WIN32
#ifdef MAKE_LIBLDR_LIB
#define LIBLDR_EXPORT __declspec(dllexport)
#else
#define LIBLDR_EXPORT __declspec(dllimport)
#endif
#define DIRECTORY_SEPARATOR "\\"
#else
#define LIBLDR_EXPORT __attribute__ ((visibility("default")))
#define DIRECTORY_SEPARATOR "/"
#endif

#ifdef WIN32
#ifdef MAKE_LIBLDRAWRENDERER_LIB
#define LIBLDRAWRENDERER_EXPORT __declspec(dllexport)
#else
#define LIBLDRAWRENDERER_EXPORT __declspec(dllimport)
#endif
#else
#define LIBLDRAWRENDERER_EXPORT __attribute__ ((visibility("default")))
#endif

#include "exception.h"

#endif
