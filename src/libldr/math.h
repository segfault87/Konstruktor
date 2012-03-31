/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_MATH_H_
#define _LIBLDR_MATH_H_

#include <cstring>
#include <string>

#include "common.h"

// approximation threshold
#define LDR_EPSILON 1e-2

namespace ldraw
{

// Space vector
class LIBLDR_EXPORT vector
{
 public:
  vector() { m_array[0] = 0.0f, m_array[1] = 0.0f, m_array[2] = 0.0f; }
  vector(float x, float y, float z, float w = 1.0f) { m_array[0] = x, m_array[1] = y, m_array[2] = z; m_array[3] = w; }
  vector(const vector &p) { m_array[0] = p.x(), m_array[1] = p.y(), m_array[2] = p.z(), m_array[3] = p.w(); }
  ~vector() {}
  
  inline float& x() { return m_array[0]; }
  inline const float& x() const { return m_array[0]; }
  inline float& y() { return m_array[1]; }
  inline const float& y() const { return m_array[1]; }
  inline float& z() { return m_array[2]; }
  inline const float& z() const { return m_array[2]; }
  // quadruple
  inline float& w() { return m_array[3]; }
  inline const float& w() const { return m_array[3]; }
  inline const float* get_pointer() const { return m_array; }
  
  float length() const;
  vector normalize() const;
  
  static float distance(const vector &a, const vector &b);
  static float get_angle(const vector &a, const vector &b);
  static float dot_product(const vector &a, const vector &b);
  static vector cross_product(const vector &a, const vector &b);
  
  bool compare(const vector &other, float epsilon = 0.05f) const;
  
  vector operator+ (const vector &rhs) const;
  vector operator- (const vector &rhs) const;
  vector operator- () const;
  vector operator* (float factor) const; // Scalar multiplication
  float& operator[] (int n) { return m_array[n]; }
  const float& operator[] (int n) const { return m_array[n]; }
  
  void operator= (const vector &rhs);
  
 private:
  float m_array[4];
};

// Generic 3D-space transformation matrix
class LIBLDR_EXPORT matrix
{
 public:
  matrix();
  matrix(float a, float b, float c, float d, float e, float f, float g, float h, float i, float x = 0.0f, float y = 0.0f, float z = 0.0f);
  matrix(float *m);
  matrix(const matrix &m);
  ~matrix() {}
  
  matrix operator+ (const matrix &m) const;
  matrix operator- (const matrix &m) const;
  matrix operator* (const matrix &m) const; // 4x4 * 4x4
  vector operator* (const vector &v) const; // 4x4 * 4x1
  matrix operator* (const int &f) const; // Scalar multiplication
  matrix operator~ () const; // Inversion
  matrix transpose() const;
  
  vector get_translation_vector() const;
  void set_translation_vector(const vector &v);
  
  inline float& value(int r, int c) { return m_matrix[r][c]; }
  inline const float& value(int r, int c) const { return m_matrix[r][c]; }
  inline const float* get_pointer() const { return &m_matrix[0][0]; }
  
  void operator= (const matrix &rhs) { std::memcpy(&m_matrix, rhs.get_pointer(), sizeof(float)*16); }
	
 private:
  float m_matrix[4][4];
};

}

#endif
