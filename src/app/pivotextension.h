// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PIVOTEXTENSION_H_
#define _PIVOTEXTENSION_H_

#include <libldr/extension.h>
#include <libldr/math.h>

namespace ldraw
{
	class filter;
}

namespace Konstruktor
{

class PivotExtension : public ldraw::extension
{
  public:
	PivotExtension();
	PivotExtension(ldraw::model *m, void *arg = 0L);
	PivotExtension(ldraw::model *m, const ldraw::filter *filter);
	~PivotExtension();

	void update();
	
	void updateToCenter(const ldraw::filter *filter);
	void updateToCenter();

	const ldraw::vector& pivot() const;

	void setPivot(const ldraw::vector &v);

	static ldraw::vector queryPivot(ldraw::model *m, bool center = false, const ldraw::filter *filter = 0L);

	static const std::string identifier() { return "pivot"; }

  private:
	ldraw::vector pivot_;
};

}

#endif
