// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <QAction>
#include <QSet>
#include <QUndoGroup>

#include <libldr/color.h>
#include <libldr/math.h>

#include "commandbase.h"

namespace ldraw
{
  class model;
}

class QMenu;

namespace Konstruktor
{

class UndoAction : public QAction 
{
  Q_OBJECT;
  
 public:
  UndoAction(const QString &prefix, QObject *parent = 0L);
                                                         
 public slots:
  void setPrefixedText(const QString &text);
  
 private:
  QString prefix_;
};


class Editor : public QUndoGroup
{
  Q_OBJECT;
  
 public:
  enum GridMode { Grid20, Grid10, Grid5, Grid1 };
  enum RotationPivot { PivotEach, PivotCenter, PivotManual };
  enum Axis { AxisX, AxisY, AxisZ };

  static Editor* instance() { return instance_; }
  
  Editor(QObject *parent = 0L);
  ~Editor();
  
  QAction* createRedoAction();
  QAction* createUndoAction();
  
  RotationPivot rotationPivotMode() const { return pivotMode_; }
  GridMode gridMode() const { return gridMode_; }
  
  float snap(float v) const;
  float snapYAxis(float v) const;
  float gridDensity() const;
  float gridDensityYAxis() const;
  float gridDensityAngle() const;

  QAction* getColor(const ldraw::color &color) const;
  QList<QAction *> getFavoriteColors() const;
  QList<QAction *> getRecentlyUsedColors() const;
  const ldraw::vector& getPivot() const;

  void setRotationPivotMode(RotationPivot pivot);
  void setPivot(const ldraw::vector &pos);
  
 signals:
  void selectionIndexModified(const QSet<int> &selection);
  void selectionRemoved(const QSet<int> &selection);
  void objectInserted(int offset, int items);
  void rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &rowList);
  void modified();
  void needRepaint();
  void colorListChanged();
                    
 public slots:
  void selectionChanged(const QSet<int> &selection);
  void modelChanged(ldraw::model *model);
  void activeChanged(QUndoStack *stack);
  void stackAdded(QUndoStack *stack);
  void updatePivot();
  
  void setGridMode(GridMode mode);
  
  // Editing
  void cut();
  void copy();
  void paste();
  void deleteSelected();
  void editColor();
  void setColor(const ldraw::color &c);
  void translate(const ldraw::matrix &matrix);
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
  static Editor *instance_;
  
  GridMode gridMode_;
  RotationPivot pivotMode_;
  
  // Stacks
  QUndoStack *activeStack_;
  bool changedFlag_;
  int lastIndex_;
  
  const QSet<int> *selection_;
  ldraw::model *model_;
  ldraw::vector pivot_;
};

}

#endif
