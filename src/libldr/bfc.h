/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2009 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_BFC_H_
#define _LIBLDR_BFC_H_

#include <stack>

#include "elements.h"
#include "extension.h"

#define CAST_AS_BFC(c) (dynamic_cast<ldraw::element_bfc *>(c))
#define CAST_AS_CONST_BFC(c) (dynamic_cast<const ldraw::element_bfc *>(c))

namespace ldraw
{

class LIBLDR_EXPORT bfc_certification : public extension
{
  public:
	enum cert_status
	{
		certified, uncertified, unknown
	};

	enum winding
	{
		ccw, cw
	};
	
	bfc_certification(model *m, void *arg);
	bfc_certification(cert_status cert, winding ori = ccw);
	virtual ~bfc_certification() {}

	static const std::string identifier() { return "bfc_certification"; }

	bfc_certification& operator=(const bfc_certification &rhs);
	bfc_certification& operator=(cert_status cert);

	cert_status certification() const { return m_cert; }
	winding orientation() const { return m_ori; }

	void set_certification(cert_status cert) { m_cert = cert; }
	void set_orientation(winding ori) { m_ori = ori; }

  private:
	cert_status m_cert;
	winding m_ori;
 };

class LIBLDR_EXPORT element_bfc : public element_base
{
  public:
	enum command 
	{
		cw          = 0x1,
		ccw         = 0x2,
		clip        =       0x04,
		clip_cw     = 0x1 | 0x04,
		clip_ccw    = 0x2 | 0x04,
		noclip      =       0x08,
		invertnext  =       0x10
	};
	
	element_bfc(command cmd);
	element_bfc(const element_bfc &b);
	virtual ~element_bfc() {}

	command get_command() const;
	void set_command(command cmd);

	virtual type get_type() const { return type_bfc; }
	virtual int line_type() const { return 0; }

	element_bfc& operator= (const element_bfc &rhs);

  private:
	command m_cmd;
};

class LIBLDR_EXPORT bfc_state_tracker
{
  public:
	bfc_state_tracker();
	~bfc_state_tracker();

	bool culling() const;
	bool inverted() const;
	bool localinverted() const;

	void accumulate_culling(bool b);
	void accumulate_invert(bool b, bool r);

	void pop_culling();
	void pop_invert();

  private:
	std::stack<int> m_cullstack;
	std::stack<int> m_invertstack;
	std::stack<int> m_localinvertstack;
};

}

#endif
