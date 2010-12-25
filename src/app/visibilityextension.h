// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _VISIBILITYEXTENSION_H_
#define _VISIBILITYEXTENSION_H_

#include <set>
#include <string>

#include <libldr/extension.h>

#include <renderer/renderer.h>

class KonstruktorVisibilityExtension : public ldraw::extension, public ldraw_renderer::render_filter
{
  public:
	KonstruktorVisibilityExtension(ldraw::model *m, void *arg = 0L);
	~KonstruktorVisibilityExtension();

	const std::set<int>& set() const;
	std::set<int>& set();
	
	void replaceSet(const std::set<int> &vs);

	bool find(int idx) const;
	void insert(int idx);
	bool remove(int idx);
	void clear();

	bool query(const ldraw::model *m, int index, int depth) const;

	static const std::string identifier() { return "visibility"; }

	static KonstruktorVisibilityExtension* query(ldraw::model *model);

  private:
	std::set<int> visibilityset_;
};	

#endif

