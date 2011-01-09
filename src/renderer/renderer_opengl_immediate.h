/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_RENDERER_OPENGL_IMMEDIATE_H_
#define _RENDERER_RENDERER_OPENGL_IMMEDIATE_H_

#include <set>
#include <stack>

#include <libldr/common.h>
#include <libldr/bfc.h>
#include <libldr/elements.h>
#include <libldr/model.h>

#include <renderer/renderer_opengl.h>

namespace ldraw
{
    class bfc_state_tracker;
    class color;
    struct color_entity;
    class metrics;
}

namespace ldraw_renderer
{

struct statistics
{
	int triangles;
	int quads;
	int faces;
	int lines;
};

class parameters;

/* OpenGL immediate mode rendering path (now deprecated) */

class LIBLDR_EXPORT renderer_opengl_immediate : public renderer_opengl
{
  public:
	~renderer_opengl_immediate();
	
	const statistics* get_stats() const { return &m_stats; }

	void render(ldraw::model *m, const render_filter *filter);
	void render_bounding_box(const ldraw::metrics &metrics);
	
	bool hit_test(float *projection_matrix, float *modelview_matrix, int x, int y, int w, int h, ldraw::model *m, const render_filter *filter);
	std::list<std::pair<int, GLuint> > select(float *projection_matrix, float *modelview_matrix, int x, int y, int w, int h, ldraw::model *m, const render_filter *filter);
	
  protected:
	void draw_model_full(const ldraw::model_multipart *base, ldraw::model *m, int depth, const render_filter *filter);
	void draw_model_edges(const ldraw::model_multipart *base, const ldraw::model *m, int depth, const render_filter *filter);
	void draw_model_bounding_boxes(const ldraw::model_multipart *base, const ldraw::model *m, int depth, const render_filter *filter);

	void render_filled_bounding_box(const ldraw::metrics &metrics);
	void render_line(const ldraw::element_line &l);
	void render_triangle(const ldraw::element_triangle &l);
	void render_quadrilateral(const ldraw::element_quadrilateral &l);
	void render_condline(const ldraw::element_condline &l, const ldraw::matrix &projectionMatrix);
	void render_stud(ldraw::model *l, bool edges = false);
	void render_normal_orientation(const ldraw::element_base *el, const ldraw::vector &nv, bool isccw = true);
	
  private:
	friend class renderer_opengl_factory;
	
	renderer_opengl_immediate(const parameters *rp);
	
	ldraw::bfc_state_tracker m_bfc_tracker;
	statistics m_stats;
};

}

#endif
