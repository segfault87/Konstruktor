// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _SELECTION_H_
#define _SELECTION_H_


#if defined(WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>

#include <list>
#include <set>
#include <utility>

#include <QSet>

#include <libldr/filter.h>
#include <libldr/math.h>

namespace Konstruktor
{

class VisibilityExtension;

class Selection : public ldraw::filter
{
  public:
	Selection();
	~Selection();

	void setSelection(const QSet<int> &set);
	void setModel(const ldraw::model *m);

	void resetSelection();
	void setInversed(bool i);

	const QSet<int>* getSelection() const;
	bool hasSelection() const;

	/* utility */
	const ldraw::element_ref* getLastRef() const;
	ldraw::matrix getLastMatrix() const;
	ldraw::color getLastColor() const;

	bool query(const ldraw::model *m, int index, int depth) const;

  private:
	const QSet<int> *tsset_;
	const ldraw::model *model_;
	const VisibilityExtension *visibility_;
	
	bool inversed_;
};

class IntermediateSelection : public ldraw::filter
{
  public:
	IntermediateSelection(Selection *currentSelection);
	~IntermediateSelection();

	void setList(const std::list<std::pair<int, GLuint> > &list);
	void clear();

	void setSelectionMethod(int method);

	bool hasSelection() const;
	bool query(const ldraw::model *m, int index, int depth) const;

  private:
	std::set<int> tsset_;
	Selection *currentSelection_;
	int selectionMethod_;
};

}

#endif
