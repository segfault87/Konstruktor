/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_VBUFFER_EXTENSION_H_
#define _RENDERER_VBUFFER_EXTENSION_H_

#include <map>
#include <stack>

#include <libldr/color.h>
#include <libldr/extension.h>
#include <libldr/math.h>

#include <renderer/parameters.h>

namespace ldraw
{
	class model;
}

namespace ldraw_renderer
{

class LIBLDR_EXPORT vbuffer_extension : public ldraw::extension
{
  public:
	enum buffer_type
	{
		type_lines, type_triangles, type_quads, type_condlines
	};
	
	struct vbuffer_params
	{
		bool force_fixed;
		bool force_vbuffer;
		bool collapse_subfiles;
		const parameters *params;
	};
	
	vbuffer_extension(ldraw::model *m, void *arg);
	~vbuffer_extension();

	static const std::string identifier() { return "vbuffer_extension"; }

	static int get_total_memory_usage();
	static void reset_total_memory_usage();

	void clear();
	void update();
	void update(bool collapse);

	bool is_vbo() const;
	bool is_null() const;
	bool is_update_required(bool collapse) const;

	int count(buffer_type type) const;

	GLuint get_vbo_vertices(buffer_type type) const;
	GLuint get_vbo_normals(buffer_type type) const;
	GLuint get_vbo_colors(buffer_type type) const;
	GLuint get_vbo_condline_directions() const;
	GLuint get_vbo_precolored(buffer_type type, const ldraw::color &c);

	const float* get_vertex_array(buffer_type type) const;
	const float* get_normal_array(buffer_type type) const;
	const float* get_color_array(buffer_type type) const;
	const float* get_condline_direction_array() const;
	const float* get_precolored_array(buffer_type type, const ldraw::color &c);

  private:
	bool is_color_ambiguous() const;
	bool is_color_ambiguous_recursive(const ldraw::model *m) const;
	void fork_color(const ldraw::color &c);
	
	void count_elements_stud(const ldraw::model *m);
	void count_elements_recursive(const ldraw::model *m);
	void count_elements();

	void fill_element_atomic(const ldraw::vector &v, float *data, int *iterator, bool quadruple = false);
	void fill_element_atomic(const unsigned char *color, float *data, int *iterator);
	void fill_element_atomic(const float *cflag, float *data, int *iterator);

	void fill_color(const std::stack<ldraw::color> &colorstack, const ldraw::color &color, int count, buffer_type type);
	void fill_elements_recursive(std::stack<ldraw::color> &colorstack, ldraw::model *m, const ldraw::matrix &transform);
	void fill_elements_stud(std::stack<ldraw::color> &colorstack, ldraw::model *m, const ldraw::matrix &transform);
	void fill_elements();

  private:
	static int s_memory_usage;
	
	vbuffer_params *m_params;

	bool m_isnull;
	bool m_isvbo;
	bool m_colorfixed;
	parameters::stud_rendering_mode m_stud;
	
	GLuint m_vbo_vertices[4];
	GLuint m_vbo_normals[2];
	GLuint m_vbo_colors[4];
	GLuint m_vbo_condparams;
	
	int m_elemcnt[4];
	
	float *m_vertices[4];
	float *m_normals[2];
	float *m_colors[4];
	float *m_condparams;

	int m_vertptr[4];
	int m_normptr[2];
	int m_colorptr[4];
	int m_condparamptr;

	std::map<ldraw::color, float **> m_precolored_buf;
	std::map<ldraw::color, GLuint *> m_vbo_precolored;
};	

}

#endif
