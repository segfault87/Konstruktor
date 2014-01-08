// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <QGLFormat>
#include <QMutex>
#include <QObject>
#include <QProcess>
#include <QString>

#include "config.h"

#if defined(Q_WS_WIN)
#define LDRAW_DL_URL "http://bit.ly/ldraw-part-library-windows"
#else
#define LDRAW_DL_URL "http://bit.ly/ldraw-part-library"
#endif

namespace ldraw
{
  class model;
  class part_library;
}

namespace ldraw_renderer
{
  class parameters;
}

class QGLWidget;
class QProgressDialog;
class QProcess;

namespace Konstruktor
{

class ColorManager;
class DBManager;
class MainWindow;
class PixmapRenderer;

// Main application entrypoint
class Application : public QObject
{
  Q_OBJECT;
  
 public:
  Application(QObject *parent = 0L);
  ~Application();
  
  bool initialize();
  void startup();
  
  static Application* self() { return instance_; }
  
  void testPovRay(bool overrideconfig = false);
  void initializeRenderer(QGLWidget *glBase);
  
  QString saveLocation(const QString &directory);
  
  ldraw::part_library* library() { return library_; }
  ldraw_renderer::parameters* renderer_params() { return params_; }
  Config* config() { return config_; }
  DBManager* database() { return db_; }
  ColorManager* colorManager() { return colorManager_; }
  PixmapRenderer* pixmapRenderer() { return renderer_; }
  QWidget* rootWindow();
  bool hasPovRay() const { return hasPovRay_; }

 public slots:
  void configUpdated();
                      
 private:
  static Application *instance_;
  
  MainWindow *window_;
  PixmapRenderer *renderer_;
  
  Config *config_;
  ldraw::part_library *library_;
  ldraw_renderer::parameters *params_;
  DBManager *db_;
  ColorManager *colorManager_;
  
  QMutex globalDirsMutex_;
  bool hasPovRay_;
};

}

#endif
