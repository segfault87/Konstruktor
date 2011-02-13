/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <libldr/elements.h>
#include <libldr/model.h>
#include <libldr/utils.h>

#include "opengl.h"
#include "normal_extension.h"
#include "opengl_extension_shader.h"
#include "opengl_extension_vbo.h"
#include "parameters.h"

#include "vbuffer_extension.h"

namespace ldraw_renderer
{

vbuffer_extension::vbuffer_extension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg)
{
	m_params = new vbuffer_params;
	std::memcpy(m_params, arg, sizeof(vbuffer_params));

	m_isnull = true;

	for (int i = 0; i < 4; ++i) {
		m_vbo_vertices[i] = 0;
		m_vbo_colors[i] = 0;

		m_elemcnt[i] = 0;

		m_vertptr[i] = 0;
		m_colorptr[i] = 0;

		m_vertices[i] = 0L;
		m_colors[i] = 0L;
	}

	for (int i = 0; i < 2; ++i) {
		m_vbo_normals[i] = 0;
		m_normptr[i] = 0;
		m_normals[i] = 0L;
	}

	m_vbo_condparams = 0;
	m_condparams = 0L;
	m_condparamptr = 0;

	m_colorfixed = false;
}

vbuffer_extension::~vbuffer_extension()
{
	clear();
	delete m_params;
}

int vbuffer_extension::s_memory_usage = 0;

int vbuffer_extension::get_total_memory_usage()
{
	return s_memory_usage;
}

void vbuffer_extension::reset_total_memory_usage()
{
	s_memory_usage = 0;
}

void vbuffer_extension::clear()
{
	if (!m_isnull) {
		if (m_vertices[0] != 0L) {
			for (int i = 0; i < 4; ++i) {
				delete m_vertices[i];
				delete m_colors[i];
			}
			
			for (int i = 0; i < 2; ++i)
				delete m_normals[i];

			delete m_condparams;
		}
		
		opengl_extension_vbo *vboext = opengl_extension_vbo::self();
		if (!m_params->force_vbuffer && vboext->is_supported()) {
			vboext->glDeleteBuffers(4, m_vbo_vertices);
			vboext->glDeleteBuffers(2, m_vbo_normals);
			vboext->glDeleteBuffers(4, m_vbo_colors);
			vboext->glDeleteBuffers(1, &m_vbo_condparams);
		}

		for (int i = 0; i < 4; ++i)
			m_elemcnt[i] = 0;

		for (std::map<ldraw::color, float **>::iterator it = m_precolored_buf.begin(); it != m_precolored_buf.end(); ++it) {
			for (int i = 0; i < 4; ++i)
				delete (*it).second[i];
		}

		for (std::map<ldraw::color, GLuint *>::iterator it = m_vbo_precolored.begin(); it != m_vbo_precolored.end(); ++it)
			vboext->glDeleteBuffers(4, (*it).second);
		
		m_colorfixed = false;
		
		m_isnull = true;
	}
}

void vbuffer_extension::update()
{
	clear();
	
	opengl_extension_shader *shader = opengl_extension_shader::self();
	bool is_shader = shader->is_supported() && !m_params->force_fixed;

	m_colorfixed = !is_color_ambiguous();

	int nbytes[4];
	int ncolorbytes[4];

	count_elements();

	m_stud = m_params->params->get_stud_rendering_mode();

	if (m_elemcnt[0] + m_elemcnt[1] + m_elemcnt[2] + m_elemcnt[3] == 0)
		return;

	m_isnull = false;

	for (int i = 0; i < 4; ++i) {
		nbytes[i] = 3 * m_elemcnt[i];
		ncolorbytes[i] = 4 * m_elemcnt[i];

		m_vertices[i] = new float[nbytes[i]];
		m_colors[i] = new float[ncolorbytes[i]];

		s_memory_usage += nbytes[i] * sizeof(float);
		s_memory_usage += ncolorbytes[i] * sizeof(float);
	}

	m_normals[0] = new float[nbytes[1]];
	m_normals[1] = new float[nbytes[2]];

	m_condparams = new float[nbytes[3]];

	s_memory_usage += nbytes[1] * sizeof(float) + nbytes[2] * sizeof(float) + nbytes[3] * sizeof(float);

	fill_elements();

	opengl_extension_vbo *vbo = opengl_extension_vbo::self();
	if (!m_params->force_vbuffer && vbo->is_supported()) {
		m_isvbo = true;

		// Create VBO and upload static data to VRAM if needed
		vbo->glGenBuffers(4, m_vbo_vertices);
		vbo->glGenBuffers(2, m_vbo_normals);
		vbo->glGenBuffers(4, m_vbo_colors);
		vbo->glGenBuffers(1, &m_vbo_condparams);

		for (int i = 0; i < 4; ++i) {
			vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_vertices[i]);
			vbo->glBufferData(GL_ARRAY_BUFFER_ARB, nbytes[i] * sizeof(float), m_vertices[i], GL_STATIC_DRAW_ARB);

			if (is_shader || m_colorfixed) {
				vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_colors[i]);
				vbo->glBufferData(GL_ARRAY_BUFFER_ARB, ncolorbytes[i] * sizeof(float), m_colors[i], GL_STATIC_DRAW_ARB);
			}
			
			delete m_vertices[i];
			m_vertices[i] = 0L;

			if (is_shader && m_colorfixed) {
				delete m_colors[i];
				m_colors[i] = 0L;
			}
		}

		for (int i = 0; i < 2; ++i) {
			vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_normals[i]);
			vbo->glBufferData(GL_ARRAY_BUFFER_ARB, nbytes[i + 1] * sizeof(float), m_normals[i], GL_STATIC_DRAW_ARB);
			
			delete m_normals[i];
			m_normals[i] = 0L;
		}

		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_condparams);
		vbo->glBufferData(GL_ARRAY_BUFFER_ARB, nbytes[3] * sizeof(float), m_condparams, GL_STATIC_DRAW_ARB);

		delete m_condparams;
		m_condparams = 0L;
	} else {
		m_isvbo = false;
	}
}

void vbuffer_extension::update(bool collapse)
{
	m_params->collapse_subfiles = collapse;

	update();
}

bool vbuffer_extension::is_vbo() const
{
	return m_isvbo;
}

bool vbuffer_extension::is_null() const
{
	return m_isnull;
}

bool vbuffer_extension::is_update_required(bool collapse) const
{
	if (m_params->collapse_subfiles != collapse || m_stud != m_params->params->get_stud_rendering_mode())
		return true;
	else
		return false;
}

int vbuffer_extension::count(buffer_type type) const
{
	return m_elemcnt[type];
}

GLuint vbuffer_extension::get_vbo_vertices(buffer_type type) const
{
	if (!m_isvbo || m_isnull)
		return 0;

	return m_vbo_vertices[type];
}

GLuint vbuffer_extension::get_vbo_normals(buffer_type type) const
{
	if (!m_isvbo || m_isnull)
		return 0;

	if (type == type_triangles)
		return m_vbo_normals[0];
	else if (type == type_quads)
		return m_vbo_normals[1];

	return 0;
}

GLuint vbuffer_extension::get_vbo_colors(buffer_type type) const
{
	if (!m_isvbo || m_isnull)
		return 0;

	return m_vbo_colors[type];
}

GLuint vbuffer_extension::get_vbo_condline_directions() const
{
	if (!m_isvbo || m_isnull)
		return 0;
	
	return m_vbo_condparams;
}

GLuint vbuffer_extension::get_vbo_precolored(buffer_type type, const ldraw::color &c)
{
	if (!m_isvbo || m_isnull)
		return 0;

	if (m_colorfixed)
		return get_vbo_colors(type);

	if (m_vbo_precolored.find(c) == m_vbo_precolored.end())
		fork_color(c);

	return m_vbo_precolored[c][type];
}

const float* vbuffer_extension::get_vertex_array(buffer_type type) const
{
	if (m_isvbo || m_isnull)
		return 0L;

	return m_vertices[type];
}

const float* vbuffer_extension::get_normal_array(buffer_type type) const
{
	if (m_isvbo || m_isnull)
		return 0L;

	if (type == type_triangles)
		return m_normals[0];
	else if (type == type_quads)
		return m_normals[1];

	return 0L;
}

const float* vbuffer_extension::get_color_array(buffer_type type) const
{
	if (m_isvbo || m_isnull)
		return 0L;

	return m_colors[type];
}

const float* vbuffer_extension::get_condline_direction_array() const
{
	if (m_isvbo || m_isnull)
		return 0L;

	return m_condparams;
}

const float* vbuffer_extension::get_precolored_array(buffer_type type, const ldraw::color &c)
{
	if (m_isvbo || m_isnull)
		return 0L;
	
	if (m_colorfixed)
		return get_color_array(type);

	if (m_precolored_buf.find(c) == m_precolored_buf.end())
		fork_color(c);

	return m_precolored_buf[c][type];
}

bool vbuffer_extension::is_color_ambiguous() const
{
	return is_color_ambiguous_recursive(m_model);
}

bool vbuffer_extension::is_color_ambiguous_recursive(const ldraw::model *m) const
{
	bool found;
	
	for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		ldraw::type t = (*it)->get_type();
		ldraw::color c;

		found = false;

		switch (t) {
			case ldraw::type_line:
				c = CAST_AS_CONST_LINE(*it)->get_color();
				found = true;
				break;
			case ldraw::type_triangle:
				c = CAST_AS_CONST_TRIANGLE(*it)->get_color();
				found = true;
				break;
			case ldraw::type_quadrilateral:
				c = CAST_AS_CONST_QUADRILATERAL(*it)->get_color();
				found = true;
				break;
			case ldraw::type_condline:
				c = CAST_AS_CONST_CONDLINE(*it)->get_color();
				found = true;
				break;
			case ldraw::type_ref:
				if (m_params->collapse_subfiles) {
					const ldraw::model *mm = CAST_AS_CONST_REF(*it)->get_model();

					if (mm) {
						if (is_color_ambiguous_recursive(mm))
							return true;
					}
				}
				break;
			default:
				break;
		}

		if (found) {
			if (c.get_id() == 16 || c.get_id() == 24)
				return true;
		}
	}

	return false;
}

void vbuffer_extension::fork_color(const ldraw::color &c)
{
	const ldraw::color_entity *ce = c.get_entity();
	opengl_extension_vbo *vbo = opengl_extension_vbo::self();

	float **colors = new float *[4];

	for (int i = 0; i < 4; ++i) {
		float *ctemp = m_colors[i];

		if (ctemp && m_elemcnt[i] > 0) {
			colors[i] = new float[4 * m_elemcnt[i]];
			float *cval = colors[i];

			s_memory_usage += 4 * m_elemcnt[i] * sizeof(float);

			for (int j = 0; j < m_elemcnt[i]; ++j) {
				if (*ctemp < -1.0f) {
					*(cval++) = ce->complement[0] / 255.0f;
					*(cval++) = ce->complement[1] / 255.0f;
					*(cval++) = ce->complement[2] / 255.0f;
					*(cval++) = ce->complement[3] / 255.0f;
					ctemp += 4;
				} else if (*ctemp < 0.0f) {
					*(cval++) = ce->rgba[0] / 255.0f;
					*(cval++) = ce->rgba[1] / 255.0f;
					*(cval++) = ce->rgba[2] / 255.0f;
					*(cval++) = ce->rgba[3] / 255.0f;
					ctemp += 4;
				} else {
					*(cval++) = *(ctemp++);
					*(cval++) = *(ctemp++);
					*(cval++) = *(ctemp++);
					*(cval++) = *(ctemp++);
				}
			}
		} else {
			colors[i] = 0L;
		}
	}
	if (m_isvbo) {
		GLuint *vbobuf = new GLuint[4];
		vbo->glGenBuffers(4, vbobuf);
		
		for (int i = 0; i < 4; ++i) {
			if (colors[i]) {
				vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbobuf[i]);
				vbo->glBufferData(GL_ARRAY_BUFFER_ARB, 4 * m_elemcnt[i] * sizeof(float), colors[i], GL_STATIC_DRAW_ARB);
				delete colors[i];
			}
		}
		delete colors;

		if (m_vbo_precolored.find(c) != m_vbo_precolored.end()) {
			GLuint *b = m_vbo_precolored[c];
			vbo->glDeleteBuffers(4, b);
			delete b;
		}
		m_vbo_precolored[c] = vbobuf;
	} else {
		if (m_precolored_buf.find(c) != m_precolored_buf.end()) {
			float **b = m_precolored_buf[c];

			for (int i = 0; i < 4; ++i)
				delete b[i];
			delete b;
		}
		m_precolored_buf[c] = colors;
	}
}

void vbuffer_extension::count_elements_stud(const ldraw::model *m)
{
	if (m_params->params->get_stud_rendering_mode() == parameters::stud_square)
		m_elemcnt[0] += 8;
	else if (m_params->params->get_stud_rendering_mode() == parameters::stud_line)
		m_elemcnt[0] += 2;
	else
		count_elements_recursive(m);
}

void vbuffer_extension::count_elements_recursive(const ldraw::model *m)
{
	for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		ldraw::type t = (*it)->get_type();
		
		if (t == ldraw::type_line) {
			m_elemcnt[0] += 2;
		} else if (t == ldraw::type_triangle) {
			m_elemcnt[1] += 3;
		} else if (t == ldraw::type_quadrilateral) {
			m_elemcnt[2] += 4;
		} else if (t == ldraw::type_condline) {
			m_elemcnt[3] += 2;
		} else if (t == ldraw::type_ref && m_params->collapse_subfiles) {
			const ldraw::model *mm = CAST_AS_CONST_REF(*it)->get_model();

			if (!mm)
				continue;

			if (ldraw::utils::is_stud(mm))
				count_elements_stud(mm);
			else
				count_elements_recursive(mm);
		}
	}
}

void vbuffer_extension::count_elements()
{
	for (int i = 0; i < 4; ++i)
		m_elemcnt[i] = 0;

	count_elements_recursive(m_model);
}

void vbuffer_extension::fill_element_atomic(const ldraw::vector &v, float *data, int *iterator, bool quadruple)
{
	data[(*iterator)++] = v.x();
	data[(*iterator)++] = v.y();
	data[(*iterator)++] = v.z();

	if (quadruple)
		data[(*iterator)++] = v.w();
}

void vbuffer_extension::fill_element_atomic(const unsigned char *color, float *data, int *iterator)
{
	data[(*iterator)++] = color[0] / 255.0f;
	data[(*iterator)++] = color[1] / 255.0f;
	data[(*iterator)++] = color[2] / 255.0f;
	data[(*iterator)++] = color[3] / 255.0f;
}

void vbuffer_extension::fill_element_atomic(const float *cflag, float *data, int *iterator)
{
	data[(*iterator)++] = cflag[0];
	data[(*iterator)++] = cflag[1];
}

void vbuffer_extension::fill_color(const std::stack<ldraw::color> &colorstack, const ldraw::color &color, int count, buffer_type type)
{
	const float null[] = { -1.0f, -1.0f, -1.0f, -1.0f };
	const float null_complement[] = { -2.0f, -2.0f, -2.0f, -2.0f };

	int flag = 0;
	const unsigned char *ce;
	
	if (color.get_id() == 16) {
		if (colorstack.top().get_id() == 16) {
			ce = 0L;
			flag = 1;
		} else {
			ce = colorstack.top().get_entity()->rgba;
		}
	} else if (color.get_id() == 24) {
		if (colorstack.top().get_id() == 16 || colorstack.top().get_id() == 24) {
			ce = 0L;
			flag = -1;
		} else {
			ce = colorstack.top().get_entity()->complement;
		}
	} else {
		ce = color.get_entity()->rgba;
	}

	for (int i = 0; i < count; ++i) {
		if (ce) {
			fill_element_atomic(ce, m_colors[type], &m_colorptr[type]);
		} else {
			const float *cf;

			if (flag == 1)
				cf = null;
			else
				cf = null_complement;
			
			fill_element_atomic(ldraw::vector(cf[0], cf[1], cf[2], cf[3]), m_colors[type], &m_colorptr[type], true);
		}
	}
}

void vbuffer_extension::fill_elements_recursive(std::stack<ldraw::color> &colorstack, ldraw::model *m, const ldraw::matrix &transform)
{
	if (!m->custom_data<normal_extension>())
		m->update_custom_data<normal_extension>();

	ldraw::matrix transform_wo_position = transform;
	transform_wo_position.set_translation_vector(ldraw::vector());

	const std::map<int, ldraw::vector> &norms = m->custom_data<normal_extension>()->normals();
	
	int i = 0;
	
	for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		ldraw::type t = (*it)->get_type();
		
		if (t == ldraw::type_line) {
			const ldraw::element_line *l = CAST_AS_CONST_LINE(*it);

			fill_element_atomic(transform * l->pos1(), m_vertices[0], &m_vertptr[0]);
			fill_element_atomic(transform * l->pos2(), m_vertices[0], &m_vertptr[0]);

			fill_color(colorstack, l->get_color(), 2, type_lines);
		} else if (t == ldraw::type_triangle) {
			const ldraw::element_triangle *l = CAST_AS_CONST_TRIANGLE(*it);

			fill_element_atomic(transform * l->pos1(), m_vertices[1], &m_vertptr[1]);
			fill_element_atomic(transform * l->pos2(), m_vertices[1], &m_vertptr[1]);
			fill_element_atomic(transform * l->pos3(), m_vertices[1], &m_vertptr[1]);

			ldraw::vector n = transform_wo_position * (*norms.find(i)).second;
			
			fill_element_atomic(n, m_normals[0], &m_normptr[0]);
			fill_element_atomic(n, m_normals[0], &m_normptr[0]);
			fill_element_atomic(n, m_normals[0], &m_normptr[0]);

			fill_color(colorstack, l->get_color(), 3, type_triangles);
		} else if (t == ldraw::type_quadrilateral) {
			const ldraw::element_quadrilateral *l = CAST_AS_CONST_QUADRILATERAL(*it);

			fill_element_atomic(transform * l->pos1(), m_vertices[2], &m_vertptr[2]);
			fill_element_atomic(transform * l->pos2(), m_vertices[2], &m_vertptr[2]);
			fill_element_atomic(transform * l->pos3(), m_vertices[2], &m_vertptr[2]);
			fill_element_atomic(transform * l->pos4(), m_vertices[2], &m_vertptr[2]);
			
			ldraw::vector n = transform_wo_position * (*norms.find(i)).second;
			
			fill_element_atomic(n, m_normals[1], &m_normptr[1]);
			fill_element_atomic(n, m_normals[1], &m_normptr[1]);
			fill_element_atomic(n, m_normals[1], &m_normptr[1]);
			fill_element_atomic(n, m_normals[1], &m_normptr[1]);

			fill_color(colorstack, l->get_color(), 4, type_quads);
		} else if (t == ldraw::type_condline) {
			const ldraw::element_condline *l = CAST_AS_CONST_CONDLINE(*it);

			fill_element_atomic(transform * l->pos1(), m_vertices[3], &m_vertptr[3]);
			fill_element_atomic(transform * l->pos2(), m_vertices[3], &m_vertptr[3]);

			fill_color(colorstack, l->get_color(), 2, type_condlines);			
		} else if (t == ldraw::type_ref && m_params->collapse_subfiles) {
			ldraw::element_ref *l = CAST_AS_REF(*it);
			ldraw::model *m = l->get_model();

			if (m) {
				const ldraw::color &c = l->get_color();
				
				if (c.get_id() == 16 || c.get_id() == 24)
					colorstack.push(colorstack.top());
				else
					colorstack.push(c);
				
				if (ldraw::utils::is_stud(m))
					fill_elements_stud(colorstack, m, transform * l->get_matrix());
				else
					fill_elements_recursive(colorstack, m, transform * l->get_matrix());
				
				colorstack.pop();
			}
		}

		++i;
	}
}

void vbuffer_extension::fill_elements_stud(std::stack<ldraw::color> &colorstack, ldraw::model *m, const ldraw::matrix &transform)
{
	if (m_params->params->get_stud_rendering_mode() == parameters::stud_square) {
		ldraw::vector v1(-6.0f, -4.0f, -6.0f);
		ldraw::vector v2(6.0f, -4.0f, -6.0f);
		ldraw::vector v3(6.0f, -4.0f, 6.0f);
		ldraw::vector v4(-6.0f, -4.0f, 6.0f);
		
		v1 = transform * v1;
		v2 = transform * v2;
		v3 = transform * v3;
		v4 = transform * v4;
		
		fill_element_atomic(v1, m_vertices[0], &m_vertptr[0]);
		fill_element_atomic(v2, m_vertices[0], &m_vertptr[0]);
		
		fill_element_atomic(v2, m_vertices[0], &m_vertptr[0]);
		fill_element_atomic(v3, m_vertices[0], &m_vertptr[0]);
		
		fill_element_atomic(v3, m_vertices[0], &m_vertptr[0]);
		fill_element_atomic(v4, m_vertices[0], &m_vertptr[0]);
		
		fill_element_atomic(v4, m_vertices[0], &m_vertptr[0]);
		fill_element_atomic(v1, m_vertices[0], &m_vertptr[0]);

		fill_color(colorstack, ldraw::color(24), 8, type_lines);
	} else if (m_params->params->get_stud_rendering_mode() == parameters::stud_line) {
		fill_element_atomic(transform * ldraw::vector(0.0f, 0.0f, 0.0f), m_vertices[0], &m_vertptr[0]);
		fill_element_atomic(transform * ldraw::vector(0.0f, -4.0f, 0.0f), m_vertices[0], &m_vertptr[0]);

		fill_color(colorstack, ldraw::color(24), 2, type_lines);
	} else if (m_params->params->get_stud_rendering_mode() == parameters::stud_regular) {
		fill_elements_recursive(colorstack, m, transform);
	}
}

void vbuffer_extension::fill_elements()
{
	/* initialize pointers */
	for (int i = 0; i < 2; ++i)
		m_normptr[i] = 0;

	for (int i = 0; i < 4; ++i) {
		m_vertptr[i] = 0;
		m_colorptr[i] = 0;
	}

	m_condparamptr = 0;

	ldraw::matrix transform;
	std::stack<ldraw::color> colorstack;
	
	colorstack.push(ldraw::color(16));
	
	fill_elements_recursive(colorstack, m_model, transform);
}


}


