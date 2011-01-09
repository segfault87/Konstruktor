/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_RENDERER_OPENGL_RETAINED_H_
#define _RENDERER_RENDERER_OPENGL_RETAINED_H_

#include <GL/gl.h>

#include <renderer/renderer_opengl.h>

namespace ldraw_renderer
{

class parameters;

/* OpenGL retained rendering path */

class LIBLDR_EXPORT renderer_opengl_retained : public renderer_opengl
{
  public:
	~renderer_opengl_retained();
	
	void render(ldraw::model *m, const render_filter *filter);
	void render_bounding_box(const ldraw::metrics &metrics);
	void render_bounding_box_filled(const ldraw::metrics &metrics);

	void render_bounding_boxes(ldraw::model *m, const render_filter *filter);

	bool hit_test(float *projection_matrix, float *modelview_matrix, int x, int y, int w, int h, ldraw::model *m, const render_filter *filter);
	std::list<std::pair<int, GLuint> > select(float *projection_matrix, float *modelview_matrix, int x, int y, int w, int h, ldraw::model *m, const render_filter *filter);
	
  private:
	friend class renderer_opengl_factory;

	renderer_opengl_retained(const parameters *rp, bool force_vbuffer, bool force_fixed);

	void init_shader();
	void init_vbuffer();
	
	void render_recursive(ldraw::model *m, const render_filter *filter, int depth = 0);

	static const float m_bbox_lines[];
	static const float m_bbox_filled[];

	static const char m_shader_color_modifier[];

	bool m_vbo;
	bool m_shader;

	/* VBO */
	GLuint m_vbo_bbox_lines;
	GLuint m_vbo_bbox_filled;

	/* Vertex shader */
	GLint m_vs_color_location_rgba;
	GLint m_vs_color_location_complement;
	GLint m_vs_color_location_verttype;
	GLuint m_vs_color_program;
	GLuint m_vs_color_shader;
};

}

#endif
