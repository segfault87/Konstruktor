/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <cmath>

#include "mouse_rotation.h"

/* Default isometric transformation matrix */
const ldraw::matrix mouse_rotation::isometric_projection_matrix(
	 0.707106f, 0.0f     ,  0.707106f,
	 0.408248f, 0.816497f, -0.408248f,
	-0.57735f , 0.57735f ,  0.57735f
);


mouse_rotation::mouse_rotation()
{
	m_current = isometric_projection_matrix;
}

mouse_rotation::mouse_rotation(const ldraw::matrix &init)
{
	m_current = init;
}

void mouse_rotation::press_event(int px, int py, int w, int h)
{
	if (w < 2 || h < 2)
		return;
	
	m_last = m_current;
	m_stvec = map_to_sphere(px, py, w, h);
}

void mouse_rotation::move_event(int px, int py, int w, int h)
{
	if (w < 2 || h < 2)
		return;
	
	m_envec = map_to_sphere(px, py, w, h);
	
	ldraw::vector perp = ldraw::vector::cross_product(m_stvec, m_envec);
	ldraw::vector quat;
	
	if (perp.length() > LDR_EPSILON)
		quat = ldraw::vector(perp.x(), perp.y(), perp.z(), ldraw::vector::dot_product(m_stvec, m_envec));
	else
		quat.w() = 0.0f;
	
	float n, s;
	float xs, ys, zs;
	float wx, wy, wz;
	float xx, xy, xz;
	float yy, yz, zz;
	
	n = quat.x()*quat.x() + quat.y()*quat.y() + quat.z()*quat.z() + quat.w()*quat.w();
	s = n > 0.0f ? 2.0f/n : 0.0f;
	
	xs = quat.x()*s , ys = quat.y()*s , zs = quat.z()*s ;
	wx = quat.w()*xs, wy = quat.w()*ys, wz = quat.w()*zs;
	xx = quat.x()*xs, xy = quat.x()*ys, xz = quat.x()*zs;
	yy = quat.y()*ys, yz = quat.y()*zs, zz = quat.z()*zs;
	
	m_current.value(0, 0) = 1.0f - (yy+zz); m_current.value(0, 1) =         xy-wz ; m_current.value(0, 2) =         xz+wy ;
	m_current.value(1, 0) =         xy+wz ; m_current.value(1, 1) = 1.0f - (xx+zz); m_current.value(1, 2) =         yz-wx ;
	m_current.value(2, 0) =         xz-wy ; m_current.value(2, 1) =         yz+wx ; m_current.value(2, 2) = 1.0f - (xx+yy);
	
	m_current = m_current * m_last;
}

ldraw::vector mouse_rotation::map_to_sphere(int px, int py, int w, int h)
{
	ldraw::vector out;
	float tx, ty;
	float length;
	float aw, ah;
	
	aw = 1.0f / ((w - 1.0f) * 0.5f);
	ah = 1.0f / ((h - 1.0f) * 0.5f);
	
	tx =        (px * aw) - 1.0f;
	ty = 1.0f - (py * ah);
	
	length = tx*tx + ty*ty;
	
	if (length > 1.0f) {
		// If the point is outside of the sphere
		float norm = 1.0f / std::sqrt(length);
		out = ldraw::vector(-tx * norm, ty * norm, 0.0f);
	} else
		out = ldraw::vector(-tx, ty, std::sqrt(1.0f - length));
	
	return out;
}
