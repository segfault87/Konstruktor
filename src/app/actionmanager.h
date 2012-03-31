// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _ACTIONMANAGER_H_
#define _ACTIONMANAGER_H_

#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QMap>
#include <QObject>
#include <QVector>

namespace Konstruktor
{

class ActionManager : public QObject
{
  Q_OBJECT;

 public:
  ActionManager(QObject *parent = 0L);
  ~ActionManager();

  QAction* createAction(const QString &actionName, const QString &name, QObject *receiver, const char *slot, const QKeySequence &shortcut = QKeySequence(), const QIcon &icon = QIcon());
  void addAction(const QString &actionName, QAction *action);
  void removeAction(const QString &actionName);
  QAction* query(const QString &actionName);
  
  void registerDocumentAction(const QString &actionName);
  void registerSelectionAction(const QString &actionName);

 public slots:
  void setModelState(bool enabled);
  void setSelectionState(bool enabled);

 private:
  QMap<QString, QAction *> actions_;
  QVector<QAction *> modelActions_;
  QVector<QAction *> selectionActions_;

  bool modelState_;
  bool selectionState_;
};

}

#endif
