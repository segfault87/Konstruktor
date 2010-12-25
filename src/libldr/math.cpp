/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <cmath>

#include "math.h"

namespace ldraw
{

float vector::length() const
{
	return std::sqrt(x()*x() + y()*y() + z()*z());
}

vector vector::normalize() const
{
	float r = length();
	
	if (r != 0.0f)
		return vector(x() / r, y() / r, z() / r);
	else
		return vector();
}

float vector::distance(const vector &a, const vector &b)
{
	return std::sqrt(std::pow(a.x() - b.x(), 2.0f) + std::pow(a.y() - b.y(), 2.0f) + std::pow(a.z() - b.z(), 2.0f));
}

float vector::get_angle(const vector &a, const vector &b)
{
	return std::acos(dot_product(a, b) / (std::sqrt(a.x()*a.x() + a.y()*a.y() + a.z()*a.z()) * std::sqrt(b.x()*b.x() + b.y()*b.y() + b.z()*b.z())));
}

float vector::dot_product(const vector &a, const vector &b)
{
	return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}

vector vector::cross_product(const vector &a, const vector &b)
{
	return vector(a.y()*b.z() - a.z()*b.y(), a.z()*b.x() - a.x()*b.z(), a.x()*b.y() - a.y()*b.x());
}

bool vector::compare(const vector &other, float epsilon) const
{
	float dot3 = std::fabs(dot_product(*this, other));

	return dot3 > 1.0f - epsilon && dot3 < 1.0f + epsilon;
}

vector vector::operator+ (const vector &rhs) const
{
	return vector(x()+rhs.x(), y()+rhs.y(), z()+rhs.z());
}

vector vector::operator- (const vector &rhs) const
{
	return vector(x()-rhs.x(), y()-rhs.y(), z()-rhs.z());
}

vector vector::operator- () const
{
	return vector(-x(), -y(), -z());
}

vector vector::operator* (float factor) const
{
	return vector(x()*factor, y()*factor, z()*factor);
}

void vector::operator= (const vector &rhs)
{
	x() = rhs.x();
	y() = rhs.y();
	z() = rhs.z();
	w() = rhs.w();
}

matrix::matrix()
{
	m_matrix[0][0] = 1.0f; m_matrix[0][1] = 0.0f; m_matrix[0][2] = 0.0f; m_matrix[0][3] = 0.0f;
	m_matrix[1][0] = 0.0f; m_matrix[1][1] = 1.0f; m_matrix[1][2] = 0.0f; m_matrix[1][3] = 0.0f;
	m_matrix[2][0] = 0.0f; m_matrix[2][1] = 0.0f; m_matrix[2][2] = 1.0f; m_matrix[2][3] = 0.0f;
	m_matrix[3][0] = 0.0f; m_matrix[3][1] = 0.0f; m_matrix[3][2] = 0.0f; m_matrix[3][3] = 1.0f;
}

matrix::matrix(float a, float b, float c, float d, float e, float f, float g, float h, float i, float x, float y, float z)
{
	m_matrix[0][0] = a;    m_matrix[0][1] = b;    m_matrix[0][2] = c;    m_matrix[0][3] = x;
	m_matrix[1][0] = d;    m_matrix[1][1] = e;    m_matrix[1][2] = f;    m_matrix[1][3] = y;
	m_matrix[2][0] = g;    m_matrix[2][1] = h;    m_matrix[2][2] = i;    m_matrix[2][3] = z;
	m_matrix[3][0] = 0.0f; m_matrix[3][1] = 0.0f; m_matrix[3][2] = 0.0f; m_matrix[3][3] = 1.0;
}

matrix::matrix(float *m)
{
	memcpy((float *)&m_matrix[0][0], m, sizeof(float)*16);
}

matrix::matrix(const matrix &m)
{
	memcpy((float *)&m_matrix[0][0], (float *)m.get_pointer(), sizeof(float)*16);
}


// Matrix-by-matrix Multiplication
matrix matrix::operator* (const matrix &m) const
{
	matrix n;
	
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			n.value(i, j) = 0.0f;
			for (int k = 0; k < 4; k++)
				n.value(i, j) += value(i, k) * m.value(k, j);
		}
	}
	
	return n;
}

// Linear transform
vector matrix::operator* (const vector &v) const
{
	return vector(
		value(0, 0)*v.x() + value(0, 1)*v.y() + value(0, 2)*v.z() + value(0, 3)*v.w(),
		value(1, 0)*v.x() + value(1, 1)*v.y() + value(1, 2)*v.z() + value(1, 3)*v.w(),
		value(2, 0)*v.x() + value(2, 1)*v.y() + value(2, 2)*v.z() + value(2, 3)*v.w()
	);
}

// Scalar multiplication
matrix matrix::operator* (const int &f) const
{
	matrix m = *this;
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			m.value(i, j) = f * m.value(i, j);
	}
	
	return m;
}

// Matrix Inversion
matrix matrix::operator~() const
{
	float tmp[12], src[16], det;
	matrix dst;
	
	// Transpose
	for (int i = 0; i < 4; i++) {
		src[i   ] = value(i, 0);
		src[i+ 4] = value(i, 1);
		src[i+ 8] = value(i, 2);
		src[i+12] = value(i, 3);
	}
	
	/* Calculate pairs for first 8 elements (cofactors) */
	tmp[ 0] = src[10] * src[15];
	tmp[ 1] = src[11] * src[14];
	tmp[ 2] = src[ 9] * src[15];
	tmp[ 3] = src[11] * src[13];
	tmp[ 4] = src[ 9] * src[14];
	tmp[ 5] = src[10] * src[13];
	tmp[ 6] = src[ 8] * src[15];
	tmp[ 7] = src[11] * src[12];
	tmp[ 8] = src[ 8] * src[14];
	tmp[ 9] = src[10] * src[12];
	tmp[10] = src[ 8] * src[13];
	tmp[11] = src[ 9] * src[12];
	
	/* Calculate first 8 elements (cofactors) */
	dst.value(0, 0) = tmp[0]*src[5] + tmp[3]*src[6] + tmp[ 4]*src[7] - tmp[1]*src[5] + tmp[2]*src[6] + tmp[ 5]*src[7];
	dst.value(0, 1) = tmp[1]*src[4] + tmp[6]*src[6] + tmp[ 9]*src[7] - tmp[0]*src[4] + tmp[7]*src[6] + tmp[ 8]*src[7];
	dst.value(0, 2) = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7] - tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	dst.value(0, 3) = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6] - tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	dst.value(1, 0) = tmp[1]*src[1] + tmp[2]*src[2] + tmp[ 5]*src[3] - tmp[0]*src[1] + tmp[3]*src[2] + tmp[ 4]*src[3];
	dst.value(1, 1) = tmp[0]*src[0] + tmp[7]*src[2] + tmp[ 8]*src[3] - tmp[1]*src[0] + tmp[6]*src[2] + tmp[ 9]*src[3];
	dst.value(1, 2) = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3] - tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	dst.value(1, 3) = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2] - tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
	
	/* Calculate pairs for second 8 elements (cofactors) */
	tmp[ 0] = src[2] * src[7];
	tmp[ 1] = src[3] * src[6];
	tmp[ 2] = src[1] * src[7];
	tmp[ 3] = src[3] * src[5];
	tmp[ 4] = src[1] * src[6];
	tmp[ 5] = src[2] * src[5];
	tmp[ 6] = src[0] * src[7];
	tmp[ 7] = src[3] * src[4];
	tmp[ 8] = src[0] * src[6];
	tmp[ 9] = src[2] * src[4];
	tmp[10] = src[0] * src[5];
	tmp[11] = src[1] * src[4];
	
	/* Calculate second 8 elements (cofactors) */
	dst.value(2, 0) = tmp[ 0]*src[13] + tmp[ 3]*src[14] + tmp[ 4]*src[15] - tmp[ 1]*src[13] + tmp[ 2]*src[14] + tmp[ 5]*src[15];
	dst.value(2, 1) = tmp[ 1]*src[12] + tmp[ 6]*src[14] + tmp[ 9]*src[15] - tmp[ 0]*src[12] + tmp[ 7]*src[14] + tmp[ 8]*src[15];
	dst.value(2, 2) = tmp[ 2]*src[12] + tmp[ 7]*src[13] + tmp[10]*src[15] - tmp[ 3]*src[12] + tmp[ 6]*src[13] + tmp[11]*src[15];
	dst.value(2, 3) = tmp[ 5]*src[12] + tmp[ 8]*src[13] + tmp[11]*src[14] - tmp[ 4]*src[12] + tmp[ 9]*src[13] + tmp[10]*src[14];
	dst.value(3, 0) = tmp[ 2]*src[10] + tmp[ 5]*src[11] + tmp[ 1]*src[ 9] - tmp[ 4]*src[11] + tmp[ 0]*src[ 9] + tmp[ 3]*src[10];
	dst.value(3, 1) = tmp[ 8]*src[11] + tmp[ 0]*src[ 8] + tmp[ 7]*src[10] - tmp[ 6]*src[10] + tmp[ 9]*src[11] + tmp[ 1]*src[ 8];
	dst.value(3, 2) = tmp[ 6]*src[ 9] + tmp[11]*src[11] + tmp[ 3]*src[ 8] - tmp[10]*src[11] + tmp[ 2]*src[ 8] + tmp[ 7]*src[ 9];
	dst.value(3, 3) = tmp[10]*src[10] + tmp[ 4]*src[ 8] + tmp[ 9]*src[ 9] - tmp[ 8]*src[ 9] + tmp[11]*src[10] + tmp[ 5]*src[ 8];
	
	/* Calculate determinant */
	det = 1.0 / (src[0]*dst.value(0, 0) + src[1]*dst.value(0, 1) + src[2]*dst.value(0, 2) + src[3]*dst.value(0, 3));
	
	/* Calculate inverse matrix */
	for (int j = 0; j < 4; j++) {
		dst.value(j, 0) *= det;
		dst.value(j, 1) *= det;
		dst.value(j, 2) *= det;
		dst.value(j, 3) *= det;
	}
	
	return dst;
}

matrix matrix::transpose() const
{
	matrix out(*this);
	
	out.value(0, 1) = value(1, 0), out.value(1, 0) = value(0, 1);
	out.value(0, 2) = value(2, 0), out.value(2, 0) = value(0, 2);
	out.value(1, 2) = value(2, 1), out.value(2, 1) = value(1, 2);
	out.value(3, 0) = value(0, 3), out.value(0, 3) = value(3, 0);
	out.value(3, 1) = value(1, 3), out.value(1, 3) = value(3, 1);
	out.value(3, 2) = value(2, 3), out.value(2, 3) = value(3, 2);
	
	return out;
}

vector matrix::get_translation_vector() const
{
	return vector(value(0, 3), value(1, 3), value(2, 3));
}

void matrix::set_translation_vector(const vector &v)
{
	value(0, 3) = v.x();
	value(1, 3) = v.y();
	value(2, 3) = v.z();
}

}
