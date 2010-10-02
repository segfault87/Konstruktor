/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2009 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include "bfc.h"

namespace ldraw
{

bfc_certification::bfc_certification(model *m, void *arg = 0L)
	: extension(m, arg)
{
	m_cert = unknown;
	m_ori = ccw;
}

bfc_certification::bfc_certification(cert_status cert, winding ori)
	: extension(0L, 0L)
{
	m_cert = cert;
	m_ori = ori;
}

bfc_certification& bfc_certification::operator= (const bfc_certification &rhs)
{
	m_cert = rhs.certification();
	m_ori = rhs.orientation();

	return *this;
}

bfc_certification& bfc_certification::operator= (cert_status cert)
{
	m_cert = cert;

	return *this;
}

element_bfc::element_bfc(element_bfc::command cmd)
{
	m_cmd = cmd;
}

element_bfc::element_bfc(const element_bfc &b)
{
	m_cmd = b.get_command();
}

element_bfc::command element_bfc::get_command() const
{
	return m_cmd;
}

void element_bfc::set_command(element_bfc::command cmd)
{
	m_cmd = cmd;
}

element_bfc& element_bfc::operator= (const element_bfc &rhs)
{
	m_cmd = rhs.get_command();

	return *this;
}

bfc_state_tracker::bfc_state_tracker()
{
	m_cullstack.push(1);
	m_invertstack.push(0);
	m_localinvertstack.push(0);
}

bfc_state_tracker::~bfc_state_tracker()
{
	
}

bool bfc_state_tracker::culling() const
{
	return m_cullstack.top();
}
	
bool bfc_state_tracker::inverted() const
{
	return m_invertstack.top();
}

bool bfc_state_tracker::localinverted() const
{
	return m_localinvertstack.top();
}

void bfc_state_tracker::accumulate_culling(bool b)
{
	m_cullstack.push(m_cullstack.top() & b ? 1 : 0);
}
	
void bfc_state_tracker::accumulate_invert(bool b, bool r)
{
	m_invertstack.push(m_invertstack.top() ^ (b ^ r) ? 1 : 0);
	m_localinvertstack.push(m_localinvertstack.top() ^ b ? 1 : 0);
}

void bfc_state_tracker::pop_culling()
{
	m_cullstack.pop();
}
	
void bfc_state_tracker::pop_invert()
{
	m_invertstack.pop();
	m_localinvertstack.pop();
}

}
