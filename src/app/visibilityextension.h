// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _VISIBILITYEXTENSION_H_
#define _VISIBILITYEXTENSION_H_

#include <set>
#include <string>

#include <libldr/extension.h>

#include <renderer/renderer.h>

namespace Konstruktor
{

class VisibilityExtension : public ldraw::extension, public ldraw_renderer::render_filter
{
  public:
	VisibilityExtension(ldraw::model *m, void *arg = 0L);
	~VisibilityExtension();

	const std::set<int>& set() const;
	std::set<int>& set();
	
	void replaceSet(const std::set<int> &vs);

	bool find(int idx) const;
	void insert(int idx);
	bool remove(int idx);
	void clear();

	bool query(const ldraw::model *m, int index, int depth) const;

	static const std::string identifier() { return "visibility"; }

	static VisibilityExtension* query(ldraw::model *model);

  private:
	std::set<int> visibilityset_;
};

}

#endif

