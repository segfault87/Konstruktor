/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <libldr/model.h>

#include "opengl_extension_vbo.h"
#include "opengl_extension_shader.h"
#include "vbuffer_extension.h"

#include "renderer_opengl_retained.h"

namespace ldraw_renderer
{

const float renderer_opengl_retained::m_bbox_lines[] = {
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,

	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,

	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,

	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

const float renderer_opengl_retained::m_bbox_filled[] = {
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f ,1.0f,

	1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f
};

const char renderer_opengl_retained::m_shader_color_modifier[] =
#	include "renderer_opengl_retained_vshader.h"
	;


renderer_opengl_retained::renderer_opengl_retained(const parameters *rp, bool force_vbuffer, bool force_fixed)
	: renderer_opengl(rp)
{
	if (force_vbuffer)
		m_vbo = false;
 	else
		init_vbuffer();

	if (force_fixed)
		m_shader = false;
	else
		init_shader();
}

renderer_opengl_retained::~renderer_opengl_retained()
{
	if (m_vbo) {
		opengl_extension_vbo *vbo = opengl_extension_vbo::self();
		
		vbo->glDeleteBuffers(1, &m_vbo_bbox_lines);
		vbo->glDeleteBuffers(1, &m_vbo_bbox_filled);
	}

	if (m_shader) {
		opengl_extension_shader *shader = opengl_extension_shader::self();

		shader->glDetachShader(m_vs_color_program, m_vs_color_shader);
		shader->glDeleteShader(m_vs_color_shader);
		shader->glDeleteProgram(m_vs_color_program);
	}
}

/* render filter works properly only with PARTS, PRIMITIVE mode. */
void renderer_opengl_retained::render(ldraw::model *m, const render_filter *filter)
{
	opengl_extension_shader *shader = opengl_extension_shader::self();
	opengl_extension_vbo *vbo = opengl_extension_vbo::self();
	
	glEnableClientState(GL_VERTEX_ARRAY);

	if (m_params->get_rendering_mode() == parameters::model_boundingboxes) {
		render_bounding_boxes(m, filter);
	} else {
		glEnableClientState(GL_COLOR_ARRAY);
		
		if (m_shader)
			shader->glEnableVertexAttribArray(m_vs_color_location_verttype);
		
		render_recursive(m, filter, 0);
		
		if (m_shader)
			shader->glDisableVertexAttribArray(m_vs_color_location_verttype);
		
		if (m_vbo)
			vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

		glDisableClientState(GL_COLOR_ARRAY);
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
}

void renderer_opengl_retained::render_bounding_box(const ldraw::metrics &metrics)
{
	opengl_extension_vbo *vbo = opengl_extension_vbo::self();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	const ldraw::vector &pos = metrics.min();
	ldraw::vector len = metrics.max() - metrics.min();

	glPushMatrix();
	glTranslatef(pos.x(), pos.y(), pos.z());
	glScalef(len.x(), len.y(), len.z());

	const float *bbox = 0L;
	
	if (m_vbo)
		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_bbox_lines);
	else
		bbox = m_bbox_lines;
	glVertexPointer(3, GL_FLOAT, 0, bbox);

	glDrawArrays(GL_LINES, 0, sizeof(m_bbox_lines) / sizeof(float) / 3);

	glPopMatrix();

	if (m_vbo)
		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}

void renderer_opengl_retained::render_bounding_box_filled(const ldraw::metrics &metrics)
{
	opengl_extension_vbo *vbo = opengl_extension_vbo::self();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	const ldraw::vector &pos = metrics.min();
	ldraw::vector len = metrics.max() - metrics.min();

	glPushMatrix();
	glTranslatef(pos.x(), pos.y(), pos.z());
	glScalef(len.x(), len.y(), len.z());

	const float *bbox = 0L;
	
	if (m_vbo)
		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_bbox_filled);
	else
		bbox = m_bbox_filled;
	glVertexPointer(3, GL_FLOAT, 0, bbox);

	glDrawArrays(GL_QUADS, 0, sizeof(m_bbox_filled) / sizeof(float) / 3);

	glPopMatrix();

	if (m_vbo)
		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
}

void renderer_opengl_retained::render_bounding_boxes(ldraw::model *m, const render_filter *filter)
{
	int i = 0;
	for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		if ((*it)->get_type() == ldraw::type_ref) {
			ldraw::element_ref *r = CAST_AS_REF(*it);
			ldraw::model *rm = r->get_model();

			if (rm) {
				if (!filter || (filter && !filter->query(rm, i, 0))) {
					if (!rm->custom_data<ldraw::metrics>())
						rm->init_custom_data<ldraw::metrics>();

					glPushMatrix();
					glMultMatrixf(r->get_matrix().transpose().get_pointer());
					render_bounding_box(*rm->custom_data<ldraw::metrics>());
					glPopMatrix();
				}
			}
		}
		++i;
	}
}

bool renderer_opengl_retained::hit_test(float *projection_matrix, float *modelview_matrix, int x, int y, int w, int h, ldraw::model *m, const render_filter *skip_filter)
{
	GLint viewport[4];
	GLuint selectionBuffer[4];

	if (m_vbo)
		opengl_extension_vbo::self()->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	
	if (w == 0)
		w = 1;
	else if (w < 0)
		x += w, w = -w;

	if (h == 0)
		h = 1;
	else if (h < 0)
		y += h, h = -h;

	glSelectBuffer(4, selectionBuffer);
	glRenderMode(GL_SELECT);
	
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(x + w/2, viewport[3] - (y + h/2), w, h, viewport);
	glMultMatrixf(projection_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview_matrix);
	glInitNames();
	glPushName(0);

	glPointSize(7.0f);

	int i = 0;
	for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
		ldraw::type elemtype = (*it)->get_type();

		if (skip_filter->query(m, i, 0)) {
			if (elemtype == ldraw::type_ref) {
				ldraw::element_ref *l = CAST_AS_REF(*it);
				
				if (!l->get_model())
					continue;
				
				if (!l->get_model()->custom_data<ldraw::metrics>())
					l->get_model()->update_custom_data<ldraw::metrics>();
				
				glPushMatrix();
				glMultMatrixf(l->get_matrix().transpose().get_pointer());
				
				render_bounding_box_filled(*l->get_model()->custom_data<ldraw::metrics>());
				
				glPopMatrix();
			}
		}

		++i;
	}

	if (m_vbo)
		opengl_extension_vbo::self()->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

	if (i == 0)
		return false;

	if (glRenderMode(GL_RENDER))
		return true;
	else
		return false;
}

selection_list renderer_opengl_retained::select(float *projection_matrix, float *modelview_matrix, int x, int y, int w, int h, ldraw::model *m, const render_filter *skip_filter)
{
	GLint hits, viewport[4];
	GLuint selectionBuffer[4096];

	if (m_vbo)
		opengl_extension_vbo::self()->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

	if (w == 0)
		w = 1;
	else if (w < 0)
		x += w, w = -w;

	if (h == 0)
		h = 1;
	else if (h < 0)
		y += h, h = -h;

	glSelectBuffer(1024, selectionBuffer);
	glRenderMode(GL_SELECT);
	
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(x + w/2, viewport[3] - (y + h/2), w, h, viewport);
	glMultMatrixf(projection_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview_matrix);
	glInitNames();
	glPushName(0);

	glPointSize(7.0f);

	if (m_selection == selection_model_full) {
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_DEPTH_TEST);
	}

	// Iterate!
	int i = 0;
	for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
 		if (skip_filter && skip_filter->query(m, i, 0)) {
			++i;
			continue;
		}
		
		ldraw::type elemtype = (*it)->get_type();
			
		if (elemtype == ldraw::type_ref) {
			ldraw::element_ref *l = CAST_AS_REF(*it);

			if (!l->get_model()) {
				++i;
				continue;
			}

			ldraw::model *rm = l->get_model();
			
			if (!rm->custom_data<ldraw::metrics>())
				rm->init_custom_data<ldraw::metrics>();
			
			glLoadName(i);

			const ldraw::metrics *rmm = rm->custom_data<ldraw::metrics>();
			const ldraw::matrix &rmt = l->get_matrix();
			
			if (m_selection == selection_points) {
				ldraw::vector center = (rmt * rmm->min() + rmt * rmm->max()) * 0.5f;
				
				glBegin(GL_POINTS);
				glVertex3fv(center.get_pointer());
				glEnd();
			} else {
				glPushMatrix();
				glMultMatrixf(l->get_matrix().transpose().get_pointer());

				if (m_selection == selection_model_full)
					render_bounding_box_filled(*rmm);
				else
					render(rm, 0L);
				
				glPopMatrix();
			}
		}
		
		++i;
	}

	hits = glRenderMode(GL_RENDER);

	if (m_selection == selection_model_full) {
		glPopAttrib();
	}

	std::list<std::pair<int, GLuint> > result;
	for (int i = 0; i < hits; ++i)
		result.push_back(std::pair<int, GLuint>(selectionBuffer[i * 4 + 3], selectionBuffer[i * 4 + 1]));

	return result;
}

#if 0
void printInfo(GLenum e)
{
	int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
	
	glGetObjectParameterivARB(e, GL_OBJECT_INFO_LOG_LENGTH_ARB,
							  &infologLength);
	
    if (infologLength > 0)
    {
        infoLog = new char[infologLength];
        glGetInfoLogARB(e, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        delete infoLog;
    }
}
#endif

void renderer_opengl_retained::init_shader()
{
	opengl_extension_shader *shader = opengl_extension_shader::self();

	if (shader->is_supported()) {
		m_shader = true;

		m_vs_color_program = shader->glCreateProgram();
		
		const char *str = m_shader_color_modifier;
		m_vs_color_shader = shader->glCreateShader(GL_VERTEX_SHADER_ARB);
		shader->glShaderSource(m_vs_color_shader, 1, &str, 0L);
		shader->glCompileShader(m_vs_color_shader);
		shader->glAttachShader(m_vs_color_program, m_vs_color_shader);
		shader->glLinkProgram(m_vs_color_program);

#if 0
		printInfo(m_vs_color_shader);
		printInfo(m_vs_color_program);
#endif

		m_vs_color_location_rgba = shader->glGetUniformLocation(m_vs_color_program, "rgba");
		m_vs_color_location_complement = shader->glGetUniformLocation(m_vs_color_program, "complement");
		m_vs_color_location_verttype = shader->glGetAttribLocation(m_vs_color_program, "verttype");
	} else {
		m_shader = false;
	}
}

void renderer_opengl_retained::init_vbuffer()
{
	opengl_extension_vbo *vbo = opengl_extension_vbo::self();
	
	if (vbo->is_supported()) {
		m_vbo = true;
		
		vbo->glGenBuffers(1, &m_vbo_bbox_lines);
		vbo->glGenBuffers(1, &m_vbo_bbox_filled);

		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_bbox_lines);
		vbo->glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(m_bbox_lines), m_bbox_lines, GL_STATIC_DRAW_ARB);

		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, m_vbo_bbox_filled);
		vbo->glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(m_bbox_filled), m_bbox_filled, GL_STATIC_DRAW_ARB);

		vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	} else {
		m_vbo = false;
	}
}

void renderer_opengl_retained::render_recursive(ldraw::model *m, const render_filter *filter, int depth)
{
	if (!m)
		return;

	bool edgesonly = m_params->get_rendering_mode() == parameters::model_edges;
	
	bool collapse = false;
	parameters::vbuffer_criteria vc = m_params->get_vbuffer_criteria();

	if (vc == parameters::vbuffer_everything && depth == 0)
		collapse = true;
	else if (vc == parameters::vbuffer_submodels && m->modeltype() <= ldraw::model::submodel)
		collapse = true;
	else if (vc == parameters::vbuffer_parts && m->modeltype() <= ldraw::model::part)
		collapse = true;
	else
		collapse = false;

	vbuffer_extension *ve = m->custom_data<vbuffer_extension>();
	if (!ve) {
		vbuffer_extension::vbuffer_params p;
		p.force_vbuffer = !m_vbo;
		p.collapse_subfiles = collapse;
		p.params = m_params;
		
		ve = m->init_custom_data<vbuffer_extension>(&p);
		ve->update();
	} else {
		if (ve->is_update_required(collapse))
			ve->update(collapse);
	}

	if (!ve->is_null()) {
		const float *color;
		GLuint vbo_color;
		bool shading = m_params->get_shading();
		opengl_extension_vbo *vbo = opengl_extension_vbo::self();
		opengl_extension_shader *shader = opengl_extension_shader::self();
		
		if (m_shader) {
			shader->glUseProgram(m_vs_color_program);
			ldraw::color c(0);
			if (m_colorstack.size() > 0)
				c = m_colorstack.top();
			
			const unsigned char *cptr;
			
			cptr = c.get_entity()->rgba;
			shader->glUniform4f(m_vs_color_location_rgba, cptr[0] / 255.0f, cptr[1] / 255.0f, cptr[2] / 255.0f, cptr[3] / 255.0f);
			cptr = c.get_entity()->complement;
			glUniform4f(m_vs_color_location_complement, cptr[0] / 255.0f, cptr[1] / 255.0f, cptr[2] / 255.0f, cptr[3] / 255.0f);
		}

		glDisable(GL_LIGHTING);
		
		/* lines */
		if (ve->count(vbuffer_extension::type_lines) > 0) {
			if (m_vbo)
				vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, ve->get_vbo_vertices(vbuffer_extension::type_lines));
			glVertexPointer(3, GL_FLOAT, 0, ve->get_vertex_array(vbuffer_extension::type_lines));
			if (m_vbo) {
				if (m_shader)
					vbo_color = ve->get_vbo_colors(vbuffer_extension::type_lines);
				else
					vbo_color = ve->get_vbo_precolored(vbuffer_extension::type_lines, m_colorstack.top());
				vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_color);
			}
			if (m_shader)
				color = ve->get_color_array(vbuffer_extension::type_lines);
			else
				color = ve->get_precolored_array(vbuffer_extension::type_lines, m_colorstack.top());
			glColorPointer(4, GL_FLOAT, 0, color);
			glDrawArrays(GL_LINES, 0, ve->count(vbuffer_extension::type_lines));
		}
		
		/* conditional lines */
		if (ve->count(vbuffer_extension::type_condlines) > 0) {
#if 0
			if (m_vbo)
				vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, ve->get_vbo_vertices(vbuffer_extension::type_condlines));
			glVertexPointer(3, GL_FLOAT, 0, ve->get_vertex_array(vbuffer_extension::type_condlines));
			if (m_vbo)
				vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, ve->get_vbo_colors(vbuffer_extension::type_condlines));
			glColorPointer(4, GL_FLOAT, 0, ve->get_color_array(vbuffer_extension::type_condlines));
			glDrawArrays(GL_LINES, 0, ve->count(vbuffer_extension::type_condlines));
#endif
		}

		if (!edgesonly) {
			if (shading) {
				glEnable(GL_LIGHTING);
				glEnableClientState(GL_NORMAL_ARRAY);
			}
			
			/* triangles */
			if (ve->count(vbuffer_extension::type_triangles) > 0) {
				if (m_vbo)
					vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, ve->get_vbo_vertices(vbuffer_extension::type_triangles));
				glVertexPointer(3, GL_FLOAT, 0, ve->get_vertex_array(vbuffer_extension::type_triangles));
				if (shading) {
					if (m_vbo)
						vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, ve->get_vbo_normals(vbuffer_extension::type_triangles));
					glNormalPointer(GL_FLOAT, 0, ve->get_normal_array(vbuffer_extension::type_triangles));
				}
				
				if (m_vbo) {
					if (m_shader)
						vbo_color = ve->get_vbo_colors(vbuffer_extension::type_triangles);
					else
						vbo_color = ve->get_vbo_precolored(vbuffer_extension::type_triangles, m_colorstack.top());
					vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_color);
				}
				if (m_shader)
					color = ve->get_color_array(vbuffer_extension::type_triangles);
				else
					color = ve->get_precolored_array(vbuffer_extension::type_triangles, m_colorstack.top());
				glColorPointer(4, GL_FLOAT, 0, color);
				glDrawArrays(GL_TRIANGLES, 0, ve->count(vbuffer_extension::type_triangles));
			}
			
			/* quads */
			if (ve->count(vbuffer_extension::type_quads) > 0) {
				if (m_vbo)
					vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, ve->get_vbo_vertices(vbuffer_extension::type_quads));
				glVertexPointer(3, GL_FLOAT, 0, ve->get_vertex_array(vbuffer_extension::type_quads));
				if (shading) {
					if (m_vbo)
						vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, ve->get_vbo_normals(vbuffer_extension::type_quads));
					glNormalPointer(GL_FLOAT, 0, ve->get_normal_array(vbuffer_extension::type_quads));
				}
				if (m_vbo) {
					if (m_shader)
						vbo_color = ve->get_vbo_colors(vbuffer_extension::type_quads);
					else
						vbo_color = ve->get_vbo_precolored(vbuffer_extension::type_quads, m_colorstack.top());
					vbo->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo_color);
				}
				if (m_shader)
					color = ve->get_color_array(vbuffer_extension::type_quads);
				else
					color = ve->get_precolored_array(vbuffer_extension::type_quads, m_colorstack.top());
				glColorPointer(4, GL_FLOAT, 0, color);
				glDrawArrays(GL_QUADS, 0, ve->count(vbuffer_extension::type_quads));
			}
			
			if (m_shader)
				shader->glUseProgram(0);
			
			if (shading)
				glDisableClientState(GL_NORMAL_ARRAY);
		}
	}

	if (!collapse) {
		int i  = 0;
		for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
			if ((*it)->get_type() == ldraw::type_ref) {
				ldraw::element_ref *r = CAST_AS_REF(*it);

				if (!filter || (filter && !filter->query(m, i, depth))) {
					m_colorstack.push(r->get_color());
					
					glPushMatrix();
					glMultMatrixf(r->get_matrix().transpose().get_pointer());
					render_recursive(r->get_model(), filter, depth + 1);
					glPopMatrix();

					m_colorstack.pop();
				}
			}
			++i;
		}
	}
}

}
