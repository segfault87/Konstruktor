/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_COLOR_H_
#define _LIBLDR_COLOR_H_

#include <map>
#include <string>

#include <libldr/common.h>

namespace ldraw
{

enum material_type {
	material_normal,
	material_transparent,
	material_luminant,
	material_glitter,
	material_pearlescent,
	material_chrome,
	material_metallic,
	material_rubber,
	material_speckle
};

struct material_traits_speckle
{
	unsigned char color[3];
	float fraction;
	int minsize;
	int maxsize;
};

struct material_traits_glitter
{
	unsigned char color[3];
	float fraction;
	float vfraction;
	int size;
};

struct color_entity
{
	material_type material;
	unsigned char rgba[4]; // RGBA Array
	unsigned char complement[4]; // RGBA Array
	char luminance;
	int id; // LDraw Color ID
	std::string name; // Name String
	const void *traits;
};

// Represents a color.
class LIBLDR_EXPORT color
{
  public:
	static const material_traits_glitter material_chart_glitter[];
	static const material_traits_speckle material_chart_speckle[];
	static const color_entity color_chart[];
	static const std::map<int, const color_entity *> color_map;

	static void init();
	
	color() : m_valid(true), m_id(0) { link(); }
	color(int id) : m_id(id) { link(); }
	color(const color &c) : m_id(c.get_id()) { link(); }
	~color();
	
	void operator=(int cid) { m_id = cid; link(); }
	void operator=(const color &rhs) { m_id = rhs.get_id(); link(); }
	bool operator<(const color &rhs) const { return m_id < rhs.get_id(); }
	
	
	int get_id() const { return m_id; }
	void set_id(int i) { m_id = i; link(); }
	
	bool is_valid() { return m_valid; }
	bool is_null() { return m_id == 16 || m_id == 24; }
	const color_entity* get_entity() const { return m_entity; }
	
  private:
	static bool m_initialized;
	
	void link();
	
	bool m_custom_color;
	bool m_valid;
	int m_id;
	const color_entity *m_entity;
};

}

#endif
