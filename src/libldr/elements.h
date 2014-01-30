/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_ELEMENTS_H_
#define _LIBLDR_ELEMENTS_H_

#include <string>

#include "common.h"
#include "color.h"
#include "math.h"

#define CAST_AS_COMMENT(c)         (dynamic_cast<ldraw::element_comment         *>(c))
#define CAST_AS_STATE(c)           (dynamic_cast<ldraw::element_state           *>(c))
#define CAST_AS_PRINT(c)           (dynamic_cast<ldraw::element_print           *>(c))
#define CAST_AS_REF(c)             (dynamic_cast<ldraw::element_ref             *>(c))
#define CAST_AS_LINE(c)            (dynamic_cast<ldraw::element_line            *>(c))
#define CAST_AS_TRIANGLE(c)        (dynamic_cast<ldraw::element_triangle        *>(c))
#define CAST_AS_QUADRILATERAL(c)   (dynamic_cast<ldraw::element_quadrilateral   *>(c))
#define CAST_AS_CONDLINE(c)        (dynamic_cast<ldraw::element_condline        *>(c))

#define CAST_AS_CONST_COMMENT(c)         (dynamic_cast<const ldraw::element_comment         *>(c))
#define CAST_AS_CONST_STATE(c)           (dynamic_cast<const ldraw::element_state           *>(c))
#define CAST_AS_CONST_PRINT(c)           (dynamic_cast<const ldraw::element_print           *>(c))
#define CAST_AS_CONST_REF(c)             (dynamic_cast<const ldraw::element_ref             *>(c))
#define CAST_AS_CONST_LINE(c)            (dynamic_cast<const ldraw::element_line            *>(c))
#define CAST_AS_CONST_TRIANGLE(c)        (dynamic_cast<const ldraw::element_triangle        *>(c))
#define CAST_AS_CONST_QUADRILATERAL(c)   (dynamic_cast<const ldraw::element_quadrilateral   *>(c))
#define CAST_AS_CONST_CONDLINE(c)        (dynamic_cast<const ldraw::element_condline        *>(c))

namespace ldraw
{

class model;
class model_multipart;
class part_library;
class reader;

// Element types used throughout libLDR
enum type
{
  type_comment,       // Comment
  type_state,         // State-related
  type_print,         // State-related (print)
  type_ref,           // Reference
  type_line,          // Line
  type_triangle,      // Triangle
  type_quadrilateral, // Quadrilateral
  type_condline,      // Conditional line
  type_bfc            // BFC (defined in bfc.h)
};

enum capabilities
{
  capability_color = 0x1
};

// Parent class of every element classes.
class LIBLDR_EXPORT element_base
{
public:
  element_base() {}
  virtual ~element_base() {}
  
  virtual type get_type() const = 0;
  virtual  int line_type() const = 0;
  virtual unsigned int capabilities() const { return 0; }
};

// Colored element
class LIBLDR_EXPORT element_colored_base : public element_base
{
public:
  element_colored_base(const color &c) : element_base(), m_color(c) {}
  virtual ~element_colored_base() {}
  
  virtual unsigned int capabilities() const { return capability_color; };
  
  const color& get_color() const { return m_color; }
  void set_color(const color &c) { m_color = c; }
  
protected:
  color m_color;
};	

// Comment. does nothing
class LIBLDR_EXPORT element_comment : public element_base
{
public:
  element_comment(const std::string &s) : m_str(s) {}
  element_comment(const element_comment &c) : element_base(), m_str(c.get_comment()) {}
  ~element_comment() {}
  
  const std::string& get_comment() const { return m_str; }
  void set_comment(const std::string &s) { m_str = s;}
  
  type get_type() const { return type_comment; }
  int line_type() const { return 0; }
  
  void operator= (const element_comment &rhs) { m_str = rhs.get_comment(); }
  
private:
  std::string m_str;
};

// status definition
class LIBLDR_EXPORT element_state : public element_base
{
public:
  enum state { state_step, state_pause, state_clear, state_save };
  
  element_state(state s) : element_base(), m_state(s) {}
  element_state(const element_state &s) : element_base(), m_state(s.get_state()) {}
  ~element_state() {}
  
  state& get_state() { return m_state; }
  const state& get_state() const { return m_state; }
  
  type get_type() const { return type_state; }
  int line_type() const { return 0; }
  
  void operator= (const element_state &rhs) { m_state = rhs.get_state(); }
  
private:
  state m_state;
};

// Can be used for printing a string on the screen.
class LIBLDR_EXPORT element_print : public element_base
{
public:
  element_print(const std::string &s) : element_base(), m_str(s) {}
  element_print(const element_print &p) : element_base(), m_str(p.get_string()) {}
  ~element_print() {}
  
  const std::string& get_string() const { return m_str; }
  void set_string(const std::string &s) { m_str = s; }
  
  type get_type() const { return type_print; }
  int line_type() const { return 0; }
  
  void operator= (const element_print &rhs) { m_str = rhs.get_string(); }
  
private:
  std::string m_str;
};

// External reference. (parts, primitives, submodels)
class LIBLDR_EXPORT element_ref : public element_colored_base
{
public:
  element_ref(const color &color, const matrix &matrix, const std::string &filename);
  element_ref(element_ref &rhs);
  ~element_ref();
  
  const matrix& get_matrix() const { return m_matrix; }
  const std::string& filename() const { return m_filename; }
  model* get_model() const { return m_model; }
  model* parent() const { return m_parent; }
  part_library* linkpoint() { return m_linkpoint; }
  
  void set_matrix(const matrix &m) { m_matrix = m; }
  void set_filename(const std::string &s);
  void link();
  
  type get_type() const { return type_ref; }
  int line_type() const { return 1; }
  
  void operator= (element_ref &rhs);
  
private:
  friend class model;
  friend class model_multipart;
  friend class part_library;
  friend class reader;
  
  void set_model(model *m) { m_model = m; }
  void set_parent(model *p) { m_parent = p; }
  void resolve(part_library *l) { m_linkpoint = l; }
  
  matrix m_matrix;
  std::string m_filename;
  model *m_model;
  model *m_parent;
  part_library *m_linkpoint;
};

// Line
class LIBLDR_EXPORT element_line : public element_colored_base
{
public:
  element_line(const color &c, const vector &p1, const vector &p2) :
      element_colored_base(c), m_pos1(p1), m_pos2(p2) {}
  element_line(const element_line &l) :
      element_colored_base(l.get_color()), m_pos1(l.pos1()), m_pos2(l.pos2()) {}
  ~element_line() {}
  
  vector& pos1() { return m_pos1; }
  const vector& pos1() const { return m_pos1; }
  vector& pos2() { return m_pos2; }
  const vector& pos2() const { return m_pos2; }
  
  type get_type() const { return type_line; }
  int line_type() const { return 2; }
  
  void operator= (const element_line &rhs);
  
private:
  vector m_pos1;
  vector m_pos2;
};

// Triangle
class LIBLDR_EXPORT element_triangle : public element_colored_base
{
public:
  element_triangle(const color &c, const vector &p1, const vector &p2, const vector &p3) :
      element_colored_base(c), m_pos1(p1), m_pos2(p2), m_pos3(p3) {}
  element_triangle(const element_triangle &t) :
      element_colored_base(t.get_color()), m_pos1(t.pos1()), m_pos2(t.pos2()), m_pos3(t.pos3()) {}
  ~element_triangle() {}
  
  vector& pos1() { return m_pos1; }
  const vector& pos1() const { return m_pos1; }
  vector& pos2() { return m_pos2; }
  const vector& pos2() const { return m_pos2; }
  vector& pos3() { return m_pos3; }
  const vector& pos3() const { return m_pos3; }
  
  type get_type() const { return type_triangle; }
  int line_type() const { return 3; }
  
  void operator= (const element_triangle &rhs);
  
private:
  vector m_pos1, m_pos2, m_pos3;
};

// Quadrilateral
class LIBLDR_EXPORT element_quadrilateral : public element_colored_base
{
public:
  element_quadrilateral(const color &c, const vector &p1, const vector &p2, const vector &p3, const vector &p4) :
      element_colored_base(c), m_pos1(p1), m_pos2(p2), m_pos3(p3), m_pos4(p4) {}
  element_quadrilateral(const element_quadrilateral &q) :
      element_colored_base(q.get_color()), m_pos1(q.pos1()), m_pos2(q.pos2()), m_pos3(q.pos3()), m_pos4(q.pos4()) {}
  ~element_quadrilateral() {}
  
  vector& pos1() { return m_pos1; }
  const vector& pos1() const { return m_pos1; }
  vector& pos2() { return m_pos2; }
  const vector& pos2() const { return m_pos2; }
  vector& pos3() { return m_pos3; }
  const vector& pos3() const { return m_pos3; }
  vector& pos4() { return m_pos4; }
  const vector& pos4() const { return m_pos4; }
  
  type get_type() const { return type_quadrilateral; }
  int line_type() const { return 4; }
  
  void operator= (const element_quadrilateral &rhs);
  
private:
  vector m_pos1, m_pos2, m_pos3, m_pos4;
};

// Conditional line.
class LIBLDR_EXPORT element_condline : public element_colored_base
{
public:
  element_condline(const color &c, const vector &p1, const vector &p2, const vector &p3, const vector &p4) :
      element_colored_base(c), m_pos1(p1), m_pos2(p2), m_pos3(p3), m_pos4(p4) {}
  element_condline(const element_condline &l) :
      element_colored_base(l.get_color()), m_pos1(l.pos1()), m_pos2(l.pos2()), m_pos3(l.pos3()), m_pos4(l.pos4()) {}
  ~element_condline() {}
  
  vector& pos1() { return m_pos1; }
  const vector& pos1() const { return m_pos1; }
  vector& pos2() { return m_pos2; }
  const vector& pos2() const { return m_pos2; }
  vector& pos3() { return m_pos3; }
  const vector& pos3() const { return m_pos3; }
  vector& pos4() { return m_pos4; }
  const vector& pos4() const { return m_pos4; }
  
  type get_type() const { return type_condline; }
  int line_type() const { return 5; }
  
  void operator= (const element_condline &rhs);
  
private:
  vector m_pos1, m_pos2, m_pos3, m_pos4;
};

}

#endif
