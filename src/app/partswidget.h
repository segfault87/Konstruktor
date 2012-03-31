// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PARTSWIDGET_H_
#define _PARTSWIDGET_H_

#include <QList>
#include <QMap>
#include <QMutex>
#include <QPair>
#include <QThread>
#include <QWaitCondition>
#include <QWidget>

#include "partitems.h"

namespace Ui { class PartsWidget; }

class QListWidget;
class QListWidgetItem;
class QSortFilterProxyModel;
class QModelIndex;
class QTimer;

namespace Konstruktor
{

class PartsModel;

class IconViewItem
{
 public:
  IconViewItem(int r, QListWidgetItem *w, const PartItem *p) {
    rev = r;
    widgetItem = w;
    partItem = p;
  }
  
  void operator=(const IconViewItem &rhs) {
    rev = rhs.rev;
    widgetItem = rhs.widgetItem;
    partItem = rhs.partItem;
  }

  int rev;
  QListWidgetItem *widgetItem;
  const PartItem *partItem;
};

class PixmapLoader : public QThread
{
  Q_OBJECT;
  
 public:
  PixmapLoader(QListWidget *widget, QObject *parent = 0L);
  ~PixmapLoader();

 signals:
  void loadImage(int rev, QListWidgetItem *item, const QImage &image);

 public slots:
  void startJob(const QList<IconViewItem> &items);
  void cancel();

 private:
  void run();
  
  QMutex mutex_;
  QWaitCondition condition_;
  bool restart_;
  bool abort_;
  bool running_;
  QList<IconViewItem> pendingRequests_;
  QListWidget *list_;
  QString saveLocation_;
};

class PartsWidget : public QWidget
{
  Q_OBJECT;
  
 public:
  PartsWidget(QWidget *parent = 0L);
  virtual ~PartsWidget();

 signals:
  void beginPixmapUpdate(const QList<IconViewItem> &items);
  void cancelPixmapUpdate();
                        
 public slots:
  void resetItems(const QString &search = QString(), bool hideUnofficial = false);
  void hideUnofficial(int checkState);
                                     
 private slots:
  void selectionChanged(const QModelIndex &current, const QModelIndex &previous);
  void searchTextChanged(const QString &ref);
  void search();
  void iconSelected(QListWidgetItem *item);
  void updateIcon(int rev, QListWidgetItem *item, const QImage &image);
  
 private:
  void initialize();
  
 private:
  Ui::PartsWidget *ui_;
  PartsModel *model_;
  QSortFilterProxyModel *sortModel_;
  
  QString search_;
  bool hideUnofficial_;
  
  QList<PartCategory> categories_;
  QList<PartCategory> allCategories_;
  QMap<int, int> catidmap_;
  QMap<int, PartCategory *> categorymap_;
  QMap<int, QList<PartItem> > list_;
  
  int lastCat_;
  int stateCounter_;
  
  QTimer *searchDelay_;

  PixmapLoader *pixmapLoader_;
};

}

#endif

