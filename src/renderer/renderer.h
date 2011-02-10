/* LDRrenderer: LDraw model rendering library which based on libLDR                  *
 * To obtain more information about LDraw, visit http://www.ldraw.org                *
 * Distributed in terms of the General Public License v2                             *
 *                                                                                   *
 * Author: (c)2006-2010 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _RENDERER_RENDERER_H_
#define _RENDERER_RENDERER_H_

#include <list>
#include <stack>
#include <utility>

#include <libldr/color.h>
#include <libldr/math.h>
#include <libldr/metrics.h>

namespace ldraw
{
	class filter;
	class model;
	class color_entity;
}

namespace ldraw_renderer
{

class parameters;

typedef std::list<std::pair<int, unsigned int> > selection_list;

class LIBLDR_EXPORT renderer
{
  public:
	enum selection { selection_points, selection_bounding_boxes, selection_model_full };
	
	renderer(const parameters *rp);
	virtual ~renderer();

	void set_base_color(const ldraw::color &c);
	void set_selection_type(selection s);

	virtual void render(ldraw::model *m, const ldraw::filter *filter = 0L) = 0;
	virtual void render_bounding_box(const ldraw::metrics &metrics) = 0;
	
	virtual bool hit_test(float *projection_matrix, float *modelview_matrix, int x, int y, int w, int h, ldraw::model *m, const ldraw::filter *hit_filter) = 0;
	virtual selection_list select(float *projection_matrix, float *modelview_matrix, int x, int y, int wh, int h, ldraw::model *m, const ldraw::filter *skip_filter) = 0;

	virtual void setup();

  protected:
	const parameters *m_params;
	selection m_selection;

	/* helpers */
	const unsigned char* get_color(const ldraw::color &c) const;
	
	std::stack<ldraw::color> m_colorstack;
};

}

#endif
