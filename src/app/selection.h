// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _SELECTION_H_
#define _SELECTION_H_

#include <GL/gl.h>

#include <list>
#include <set>
#include <utility>

#include <QSet>

#include <renderer/renderer_opengl.h>

class KonstruktorSelection : public ldraw_renderer::render_filter
{
  public:
	KonstruktorSelection();
	~KonstruktorSelection();

	void setSelection(const QSet<int> &set);

	void resetSelection();
	void setInversed(bool i);

	const QSet<int>* getSelection() const;
	bool hasSelection() const;

	bool query(const ldraw::model *m, int index, int depth) const;

  private:
	const QSet<int> *tsset_;
	bool inversed_;
};

class KonstruktorIntermediateSelection : public ldraw_renderer::render_filter
{
  public:
	KonstruktorIntermediateSelection(KonstruktorSelection *currentSelection);
	~KonstruktorIntermediateSelection();

	void setList(const std::list<std::pair<int, GLuint> > &list);
	void clear();

	void setSelectionMethod(int method);

	bool hasSelection() const;
	bool query(const ldraw::model *m, int index, int depth) const;

  private:
	std::set<int> tsset_;
	KonstruktorSelection *currentSelection_;
	int selectionMethod_;
};

#endif
