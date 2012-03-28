// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "actionmanager.h"

namespace Konstruktor
{

ActionManager::ActionManager(QObject *parent)
    : QObject(parent)
{
  /* for negating initially */
  modelState_ = true;
  selectionState_ = true;
}

ActionManager::~ActionManager()
{

}

QAction* ActionManager::createAction(const QString &actionName, const QString &name, QObject *receiver, const char *slot, const QKeySequence &shortcut, const QIcon &icon)
{
  QAction *action = new QAction(name, this);
  if (!icon.isNull())
    action->setIcon(icon);
  if (!shortcut.isEmpty())
    action->setShortcut(shortcut);
  if (receiver && slot)
    connect(action, SIGNAL(triggered()), receiver, slot);
  
  actions_[actionName] = action;

  return action;
}

void ActionManager::addAction(const QString &actionName, QAction *action)
{
  actions_[actionName] = action;
}

QAction* ActionManager::query(const QString &actionName)
{
  if (!actions_.contains(actionName))
    return 0L;

  return actions_[actionName];
}

void ActionManager::registerDocumentAction(const QString &actionName)
{
  QAction *a = query(actionName);
  if (a)
    modelActions_.append(a);
}

void ActionManager::registerSelectionAction(const QString &actionName)
{
  QAction *a = query(actionName);
  if (a)
    selectionActions_.append(a);
}

void ActionManager::setModelState(bool enabled)
{
  if (modelState_ == enabled)
    return;

  modelState_ = enabled;

  foreach (QAction *a, modelActions_) {
    a->setEnabled(enabled);
  }
}

void ActionManager::setSelectionState(bool enabled)
{
  if (selectionState_ == enabled)
    return;

  selectionState_ = enabled;

  foreach (QAction *a, selectionActions_) {
    a->setEnabled(enabled);
  }
}

}













