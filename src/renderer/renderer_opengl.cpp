/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "opengl_extension_vbo.h"
#include "parameters.h"
#include "renderer_opengl_immediate.h"
#include "renderer_opengl_retained.h"

#include "renderer_opengl.h"

namespace ldraw_renderer
{

renderer_opengl::renderer_opengl(const parameters *params)
	: renderer(params)
{

}

renderer_opengl::~renderer_opengl()
{
	
}

void renderer_opengl::setup()
{
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glAlphaFunc(GL_GREATER, 0.1);
	glEnable(GL_ALPHA_TEST);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	if (m_params->get_shading()) {
		GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 0.2f};
		GLfloat specular[] = {0.0f, 0.0f, 0.0f, 0.0f};
		GLfloat shininess = 32.0f;

		/* Basic material setup */
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

		/* Light model */
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

		/* Light sources */
		GLfloat lightpos0[] = {0.0f, -1000.0f, 0.0f, 1.0f};
		GLfloat lightpos1[] = {0.0f,  1000.0f, 0.0f, 1.0f};

		GLfloat lightlocalambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		GLfloat lightlocaldiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
		GLfloat lightlocalspecular[] = {0.0f, 0.0f, 0.0f, 1.0f};

		/* light 0 (local) */
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightlocalambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightlocaldiffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightlocalspecular);

		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

		/* light 1 (flipped) */
		glLightfv(GL_LIGHT1, GL_POSITION, lightpos1);
		glLightfv(GL_LIGHT1, GL_AMBIENT, lightlocalambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightlocaldiffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, lightlocalspecular);

		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
	} else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}
}

renderer_opengl_factory::renderer_opengl_factory(const parameters *params, rendering_mode rm)
{
	m_params = params;
	m_mode = rm;

	if (m_mode == mode_vbo && !opengl_extension_vbo::self()->is_supported())
		m_mode = mode_varray;
}

renderer_opengl_factory::~renderer_opengl_factory()
{

}

renderer_opengl* renderer_opengl_factory::create_renderer() const
{
	if (m_mode == mode_immediate)
		return new renderer_opengl_immediate(m_params);
	else if (m_mode == mode_varray)
		return new renderer_opengl_retained(m_params, true, !m_params->get_shader());
	else
		return new renderer_opengl_retained(m_params, false, !m_params->get_shader());

	return 0L;
}

}

