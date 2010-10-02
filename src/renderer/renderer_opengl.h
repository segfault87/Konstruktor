/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_RENDERER_OPENGL_H_
#define _RENDERER_RENDERER_OPENGL_H_

#include <set>
#include <stack>

#include <libldr/common.h>
#include <libldr/bfc.h>
#include <libldr/elements.h>
#include <libldr/model.h>

namespace ldraw
{
    class bfc_state_tracker;
    class color;
    struct color_entity;
    class metrics;
}

struct statistics
{
	int triangles;
	int quads;
	int faces;
	int lines;
};

class LIBLDR_EXPORT renderer_opengl
{
  public:
	enum stud_rendering_mode { regular, line, square };
	enum render_method { full, edges, boundingboxes };
	
	static const unsigned char stipple_pattern[];
	
	renderer_opengl();
	virtual ~renderer_opengl();
	
	void set_base_color(const ldraw::color &c);
	
	stud_rendering_mode get_stud_rendering_mode() const { return m_stud_mode; }
	render_method get_rendering_mode() const { return m_mode; }
	bool get_shading() const { return m_shading; }
	bool get_debug() const { return m_debug; }
	const statistics* get_stats() const { return &m_stats; }
	
	void set_stud_rendering_mode(stud_rendering_mode s) { m_stud_mode = s; }
	void set_rendering_mode(render_method m) { m_mode = m; }
	void set_shading(bool b) { m_shading = b; }
	void set_debug(bool b) { m_debug = b; }
	
	virtual void setup();
	
	void draw_model(ldraw::model *m, const std::set<int> &skip_set);

  protected:
	void draw_model_full(const ldraw::model_multipart *base, ldraw::model *m, int depth, const std::set<int> &skip_set);
	void draw_model_edges(const ldraw::model_multipart *base, const ldraw::model *m, int depth, const std::set<int> &skip_set);
	void draw_model_bounding_boxes(const ldraw::model_multipart *base, const ldraw::model *m, int depth, const std::set<int> &skip_set);

	bool hit_test(float *projectionMatrix, float *modelviewMatrix, int x, int y, int w, int h, ldraw::model *m, const std::set<int> &selection_set);
	std::list<int> get_selection(float *projectionMatrix, float *modelviewMatrix, int x, int y, int w, int h, ldraw::model *m, const std::set<int> &skip_set);

	void render_filled_bounding_box(const ldraw::metrics &metrics);
	void render_bounding_box(const ldraw::metrics &metrics);
	void render_line(const ldraw::element_line &l);
	void render_triangle(const ldraw::element_triangle &l);
	void render_quadrilateral(const ldraw::element_quadrilateral &l);
	void render_condline(const ldraw::element_condline &l, const ldraw::matrix &projectionMatrix);
	void render_stud(ldraw::model *l, bool edges = false);
	void render_normal_orientation(const ldraw::element_base *el, const ldraw::vector &nv, bool isccw = true);
	
	stud_rendering_mode m_stud_mode;
	render_method m_mode;
	bool m_shading;
	bool m_debug;
	
  private:
  	/* helpers */
	const ldraw::color_entity* get_color(const ldraw::color &c) const;
	float det3(const ldraw::matrix &m) const;
	
	std::stack<const ldraw::color_entity *> m_colorstack;
	ldraw::bfc_state_tracker m_bfc_tracker;

	statistics m_stats;
};

#endif
