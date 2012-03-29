// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QColor>
#include <QSize>

#include "config.h"

Q_DECLARE_METATYPE(QColor);

namespace Konstruktor
{

Config::Config()
{
  settings_ = new QSettings();
}

Config::~Config()
{
  writeConfig();
  
  delete settings_;
}

void Config::reloadConfig()
{
  settings_->sync();
}

void Config::writeConfig()
{
  settings_->sync();
}

/* General */

QByteArray Config::state() const
{
  return settings_->value("general/state", QByteArray()).toByteArray();
}

QByteArray Config::geometry() const
{
  return settings_->value("general/geometry", QByteArray()).toByteArray();
}
  
void Config::setState(const QByteArray &v)
{
  settings_->setValue("general/state", v);
}

void Config::setGeometry(const QByteArray &v)
{
  settings_->setValue("general/geometry", v);
}

/* LDraw */

QString Config::path() const
{
  return settings_->value("ldraw/path", "").toString();
}

void Config::setPath(const QString &v)
{
  settings_->setValue("ldraw/path", v);
}

/* Database */

QSize Config::thumbnailSize() const
{
  return settings_->value("database/thumbnail_size", QSize(256, 256)).toSize();
}

bool Config::thumbnailCrop() const
{
  return settings_->value("database/thumbnail_crop", true).toBool();
}

int Config::partCount() const
{
  return settings_->value("database/part_count", -1).toInt();
}

int Config::databaseRevision() const
{
  return settings_->value("database/database_revision", -1).toInt();
}

int Config::magic() const
{
  return settings_->value("database/magic", 1).toInt();
}

void Config::setThumbnailSize(const QSize &v)
{
  settings_->setValue("database/thumbnail_size", v);
}

void Config::setThumbnailCrop(bool v)
{
  settings_->setValue("database/thumbnail_crop", v);
}

void Config::setPartCount(int v)
{
  settings_->setValue("database/part_count", v);
}

void Config::setDatabaseRevision(int v)
{
  settings_->setValue("database/database_revision", v);
}

void Config::setMagic(int v)
{
  settings_->setValue("database/magic", v);
}

/* Editor */

int Config::undoStackDepth() const
{
  return settings_->value("editor/undo_stack_depth", 200).toInt();
}

QList<int> Config::colorList() const
{
  QList<int> output;

  QVariant v = settings_->value("editor/color_list", QVariant());
  if (v.isNull()) {
    output << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 <<
        8 << 9 << 10 << 11 << 12 << 13 << 14 << 15;
  } else {
    foreach (QVariant i, v.toList())
      output << i.toInt();
  }

  return output;
}

int Config::recentlyUsedColorCount() const
{
  return settings_->value("editor/recently_used_color_count", 10).toInt();
}

void Config::setUndoStackDepth(int v)
{
  settings_->setValue("editor/undo_stack_depth", v);
}

void Config::setColorList(const QList<int> &v)
{
  QList<QVariant> l;

  foreach (int i, v)
      l << i;
  
  settings_->setValue("editor/color_list", l);
}

void Config::setRecentlyUsedColorCount(int v)
{
  settings_->setValue("editor/recently_used_color_count", v);
}

/* UI */

QColor Config::activeSubmodelColor() const
{
  return settings_->value("ui/active_submodel_color", QColor("#ff9b9d")).value<QColor>();
}

bool Config::listRefColorize() const
{
  return settings_->value("ui/list_ref_colorize", true).toBool();
}

QColor Config::listPartColor() const
{
  return settings_->value("ui/list_part_color", QColor("#ffd3d7")).value<QColor>();
}

QColor Config::listSubmodelColor() const
{
  return settings_->value("ui/list_submodel_color", QColor("#dae4ff")).value<QColor>();
}

QColor Config::listUnresolvedColor() const
{
  return settings_->value("ui/list_unresolved_color", QColor("#ffff00")).value<QColor>();
}

bool Config::listPrimitiveColorize() const
{
  return settings_->value("ui/list_primitive_colorize", false).toBool();
}

QColor Config::listPrimitiveColor() const
{
  return settings_->value("ui/list_primitive_color", QColor("#c6ffc3")).value<QColor>();
}

bool Config::listMetaColorize() const
{
  return settings_->value("ui/list_meta_colorize", true).toBool();
}

QColor Config::listMetaColor() const
{
  return settings_->value("ui/list_meta_color", QColor("#ffcc99")).value<QColor>();
}

bool Config::listCommentColorize() const
{
  return settings_->value("ui/list_comment_colorize", false).toBool();
}

QColor Config::listCommentColor() const
{
  return settings_->value("ui/list_comment_color", QColor("#ffffff")).value<QColor>();
}

Config::Viewport Config::viewportTopLeft() const
{
  return (Config::Viewport) settings_->value("ui/viewport_top_left", 0).toInt();
}

Config::Viewport Config::viewportTopRight() const
{
  return (Config::Viewport) settings_->value("ui/viewport_top_left", 6).toInt();
}

Config::Viewport Config::viewportBottomLeft() const
{
  return (Config::Viewport) settings_->value("ui/viewport_top_left", 2).toInt();
}

Config::Viewport Config::viewportBottomRight() const
{
  return (Config::Viewport) settings_->value("ui/viewport_top_left", 5).toInt();
}

void Config::setActiveSubmodelColor(const QColor &v)
{
  settings_->setValue("ui/active_submodel_color", v);
}

void Config::setListRefColorize(bool v)
{
  settings_->setValue("ui/list_ref_colorize", v);
}

void Config::setListPartColor(const QColor &v)
{
  settings_->setValue("ui/list_part_color", v);
}

void Config::setListSubmodelColor(const QColor &v)
{
  settings_->setValue("ui/list_submodel_color", v);
}

void Config::setListUnresolvedColor(const QColor &v) 
{
  settings_->setValue("ui/list_unresolved_color", v);
}

void Config::setListPrimitiveColorize(bool v) 
{
  settings_->setValue("ui/list_primitive_colorize", v);
}

void Config::setListPrimitiveColor(const QColor &v) 
{
  settings_->setValue("ui/list_primitive_color", v);
}

void Config::setListMetaColorize(bool v)
{
  settings_->setValue("ui/list_meta_colorize", v);
}

void Config::setListMetaColor(const QColor &v)
{
  settings_->setValue("ui/list_meta_color", v);
}

void Config::setListCommentColorize(bool v)
{
  settings_->setValue("ui/list_comment_colorize", v);
}

void Config::setListCommentColor(const QColor &v)
{
  settings_->setValue("ui/list_comment_color", v);
}

void Config::setViewportTopLeft(Config::Viewport v)
{
  settings_->setValue("ui/viewport_top_left", (int) v);
}

void Config::setViewportTopRight(Config::Viewport v)
{
  settings_->setValue("ui/viewport_top_right", (int) v);
}

void Config::setViewportBottomLeft(Config::Viewport v)
{
  settings_->setValue("ui/viewport_bottom_left", (int) v);
}

void Config::setViewportBottomRight(Config::Viewport v)
{
  settings_->setValue("ui/viewport_bottom_right", (int) v);
}

/* Renderer */

Config::RenderingMode Config::renderingMode() const
{
  return (RenderingMode) settings_->value("renderer/rendering_mode", 0).toInt();
}

bool Config::multisampling() const
{
  return settings_->value("renderer/multisampling", true).toBool();
}

Config::DrawingMode Config::renderMode() const
{
  return (DrawingMode) settings_->value("renderer/render_mode", 0).toInt();
}

Config::DrawingMode Config::dragMode() const
{
  return (DrawingMode) settings_->value("renderer/drag_mode", 0).toInt();
}

Config::StudType Config::studMode() const
{
  return (StudType) settings_->value("renderer/stud_mode", 0).toInt();
}

QColor Config::highlightColor() const
{
  return settings_->value("renderer/highlight_color", QColor("#ff00ff")).value<QColor>();
}

QColor Config::highlightDragColor() const
{
  return settings_->value("renderer/highlight_drag_color", QColor("#4188d4")).value<QColor>();
}

bool Config::drawGrids() const
{
  return settings_->value("renderer/draw_grids", true).toBool();
}

double Config::gridResolution() const
{
  return settings_->value("renderer/grid_resolution", 20.0).toDouble();
}

double Config::gridXOffset() const
{
  return settings_->value("renderer/grid_x_offset", 0.0).toDouble();
}

double Config::gridYOffset() const
{
  return settings_->value("renderer/grid_y_offset", 0.0).toDouble();
}

double Config::gridZOffset() const
{
  return settings_->value("renderer/grid_z_offset", 0.0).toDouble();
}

int Config::gridRows() const
{
  return settings_->value("renderer/grid_rows", 50).toInt();
}

int Config::gridColumns() const
{
  return settings_->value("renderer/grid_columns", 50).toInt();
}

QColor Config::backgroundColor() const
{
  return settings_->value("renderer/background_color", QColor("#cccccc")).value<QColor>();
}

void Config::setRenderingMode(RenderingMode v)
{
  settings_->setValue("renderer/rendering_mode", (int) v);
}

void Config::setMultisampling(bool v)
{
  settings_->setValue("renderer/multisampling", v);
}

void Config::setRenderMode(DrawingMode v)
{
  settings_->setValue("renderer/render_mode", (int) v);
}

void Config::setDragMode(DrawingMode v)
{
  settings_->setValue("renderer/drag_mode", (int) v);
}

void Config::setStudMode(StudType v)
{
  settings_->setValue("renderer/stud_mode", (int) v);
}

void Config::setHighlightColor(const QColor &v)
{
  settings_->setValue("renderer/highlight_color", v);
}

void Config::setHighlightDragColor(const QColor &v)
{
  settings_->setValue("renderer/highlight_drag_color", v);
}

void Config::setDrawGrids(bool v)
{
  settings_->setValue("renderer/drag_grids", v);
}

void Config::setGridResolution(double v)
{
  settings_->setValue("renderer/grid_resolution", v);
}

void Config::setGridXOffset(double v)
{
  settings_->setValue("renderer/grid_x_offset", v);
}

void Config::setGridYOffset(double v)
{
  settings_->setValue("renderer/grid_y_offset", v);
}

void Config::setGridZOffset(double v)
{
  settings_->setValue("renderer/grid_z_offset", v);
}

void Config::setGridRows(int v)
{
  settings_->setValue("renderer/grid_rows", v);
}

void Config::setGridColumns(int v)
{
  settings_->setValue("renderer/grid_columns", v);
}

void Config::setBackgroundColor(const QColor &v)
{
  settings_->setValue("renderer/background_color", v);
}

/* POV-Ray */

QString Config::povRayExecutablePath() const
{
  return settings_->value("renderer/pov_ray_executable_path", "povray").toString();
}

void Config::setPovRayExecutablePath(const QString &v)
{
  settings_->setValue("povray/pov_ray_executable_path", v);
}

}
