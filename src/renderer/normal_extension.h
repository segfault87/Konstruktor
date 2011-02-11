/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_NORMAL_EXTENSION_H_
#define _RENDERER_NORMAL_EXTENSION_H_

#include <map>

#include <libldr/extension.h>
#include <libldr/math.h>

namespace ldraw 
{
    class model;
}

namespace ldraw_renderer
{

class LIBLDRAWRENDERER_EXPORT normal_extension : public ldraw::extension
{
  public:
	normal_extension(ldraw::model *m, void *arg);
	~normal_extension();

	static const std::string identifier() { return "normal_extension"; }

	void update();

	bool has_normal(int idx) const;
	ldraw::vector normal(int idx) const;
	const std::map<int, ldraw::vector>& normals() const;

  private:
	static ldraw::vector calculate_normal(const ldraw::vector &v1, const ldraw::vector &v2, const ldraw::vector &v3);
	
	std::map<int, ldraw::vector> m_normals;
};

}

#endif
