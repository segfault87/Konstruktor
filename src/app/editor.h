// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <QSet>
#include <QUndoGroup>

#include "commandbase.h"

namespace ldraw
{
    class color;
    class matrix;
    class model;
}

class QAction;
class KActionCollection;
class KMenu;

namespace Konstruktor
{

class Editor : public QUndoGroup
{
	Q_OBJECT;
	
  public:
	enum GridMode { Grid20, Grid10, Grid5, Grid1 };
	enum Axis { AxisX, AxisY, AxisZ };	
	
	Editor(QObject *parent = 0L);
	~Editor();

	QAction* createRedoAction(KActionCollection *actionCollection, const QString &actionName = QString());
	QAction* createUndoAction(KActionCollection *actionCollection, const QString &actionName = QString());

	GridMode gridMode() const { return gridMode_; }

	float snap(float v) const;
	float snapYAxis(float v) const;
	float gridDensity() const;
	float gridDensityYAxis() const;
	float gridDensityAngle() const;

  signals:
	void selectionIndexModified(const QSet<int> &selection);
	void selectionRemoved(const QSet<int> &selection);
	void objectInserted(int offset, int items);
	void rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &rowList);
	void modified();
	void needRepaint();

  public slots:
	void selectionChanged(const QSet<int> &selection);
	void modelChanged(ldraw::model *model);
	void activeChanged(QUndoStack *stack);
	void stackAdded(QUndoStack *stack);

	void setGridMode(GridMode mode);
	
	// Editing
	void deleteSelected();
	void editColor();
	void move(const ldraw::vector &vector);
	void moveByXPositive();
	void moveByXNegative();
	void moveByYPositive();
	void moveByYNegative();
	void moveByZPositive();
	void moveByZNegative();
	void rotateByXClockwise();
	void rotateByXCounterClockwise();
	void rotateByYClockwise();
	void rotateByYCounterClockwise();
	void rotateByZClockwise();
	void rotateByZCounterClockwise();
	void insert(const QString &filename, const ldraw::matrix &matrix, const ldraw::color &color);

  private slots:
	void indexChanged(int index);

  private:
	GridMode gridMode_;

	// Stacks
	QUndoStack *activeStack_;
	bool changedFlag_;
	int lastIndex_;
	
	const QSet<int> *selection_;
	ldraw::model *model_;
};

}

#endif
