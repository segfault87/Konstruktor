// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <QSettings>
#include <QList>
#include <QVariant>

namespace Konstruktor
{

class Config
{
 public:
  Config();
  ~Config();

  enum Viewport {
    Top,
    Bottom,
    Front,
    Back,
    Left,
    Right,
    Free
  };

  enum RenderingMode {
    VBO,
    VertexArray,
    Immediate
  };

  enum DrawingMode {
    Full,
    Edges,
    BoundingBoxes
  };

  enum StudType {
    Normal,
    Square,
    Line
  };

 public:
  void reloadConfig();
  void writeConfig();

  /* General */
  QByteArray state() const;
  QByteArray geometry() const;
  
  void setState(const QByteArray &v);
  void setGeometry(const QByteArray &v);

  /* LDraw */
  QString path() const;

  void setPath(const QString &v);

  /* Database */

  QSize thumbnailSize() const;
  bool thumbnailCrop() const;
  int partCount() const;
  int databaseRevision() const;
  int magic() const;
  
  void setThumbnailSize(const QSize &v);
  void setThumbnailCrop(bool v);
  void setPartCount(int v);
  void setDatabaseRevision(int v);
  void setMagic(int v);

  /* Editor */
  
  int undoStackDepth() const;
  QList<int> colorList() const;
  int recentlyUsedColorCount() const;
  
  void setUndoStackDepth(int v);
  void setColorList(const QList<int> &v);
  void setRecentlyUsedColorCount(int v);

  /* UI */

  QColor activeSubmodelColor() const;
  bool listRefColorize() const;
  QColor listPartColor() const;
  QColor listSubmodelColor() const;
  QColor listUnresolvedColor() const;
  bool listPrimitiveColorize() const;
  QColor listPrimitiveColor() const;
  bool listMetaColorize() const;
  QColor listMetaColor() const;
  bool listCommentColorize() const;
  QColor listCommentColor() const;
  Viewport viewportTopLeft() const;
  Viewport viewportTopRight() const;
  Viewport viewportBottomLeft() const;
  Viewport viewportBottomRight() const;
  
  void setActiveSubmodelColor(const QColor &v);
  void setListRefColorize(bool v);
  void setListPartColor(const QColor &v);
  void setListSubmodelColor(const QColor &v);
  void setListUnresolvedColor(const QColor &v);
  void setListPrimitiveColorize(bool v);
  void setListPrimitiveColor(const QColor &v);
  void setListMetaColorize(bool v);
  void setListMetaColor(const QColor &v);
  void setListCommentColorize(bool v);
  void setListCommentColor(const QColor &v);
  void setViewportTopLeft(Viewport v);
  void setViewportTopRight(Viewport v);
  void setViewportBottomLeft(Viewport v);
  void setViewportBottomRight(Viewport v);
  
  /* Renderer */

  RenderingMode renderingMode() const;
  bool multisampling() const;
  DrawingMode renderMode() const;
  DrawingMode dragMode() const;
  StudType studMode() const;
  QColor highlightColor() const;
  QColor highlightDragColor() const;
  bool drawGrids() const;
  double gridResolution() const;
  double gridXOffset() const;
  double gridYOffset() const;
  double gridZOffset() const;
  int gridRows() const;
  int gridColumns() const;
  QColor backgroundColor() const;

  void setRenderingMode(RenderingMode v);
  void setMultisampling(bool v);
  void setRenderMode(DrawingMode v);
  void setDragMode(DrawingMode v);
  void setStudMode(StudType v);
  void setHighlightColor(const QColor &v);
  void setHighlightDragColor(const QColor &v);
  void setDrawGrids(bool v);
  void setGridResolution(double v);
  void setGridXOffset(double v);
  void setGridYOffset(double v);
  void setGridZOffset(double v);
  void setGridRows(int v);
  void setGridColumns(int v);
  void setBackgroundColor(const QColor &v);

  /* POV-Ray */

  QString povRayExecutablePath() const;

  void setPovRayExecutablePath(const QString &v);
  
 private:
  QSettings *settings_;
};

}

#endif
