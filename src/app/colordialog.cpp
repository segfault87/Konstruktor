// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/color.h>

#include "application.h"
#include "colormanager.h"
#include "ui_colordialog.h"

#include "colordialog.h"

namespace Konstruktor
{

ColorDialog::ColorDialog(QWidget *parent)
	: QDialog(parent)
{
  manager_ = Application::self()->colorManager();
  changed_ = false;
  
  ui_ = new Ui::ColorDialog();
  
  ui_->setupUi(this);
  
  ui_->buttonMoveDown->setEnabled(false);
  ui_->buttonMoveUp->setEnabled(false);
  ui_->buttonAdd->setEnabled(false);
  ui_->buttonRemove->setEnabled(false);
  
  ui_->buttonMoveDown->setIcon(QIcon::fromTheme("arrow-down"));
  ui_->buttonMoveUp->setIcon(QIcon::fromTheme("arrow-up"));
  ui_->buttonAdd->setIcon(QIcon::fromTheme("list-add"));
  ui_->buttonRemove->setIcon(QIcon::fromTheme("list-remove"));

  QStringList headers;
  headers << tr("ID") << tr("Name") << tr("Material Type");
  
  ui_->listColors->setHeaderLabels(headers);
  ui_->listFavorites->setHeaderLabels(headers);
  
  ui_->listColors->setColumnWidth(0, 32);
  ui_->listColors->setColumnWidth(1, 128);
  ui_->listColors->setColumnWidth(2, 96);
  
  ui_->listFavorites->setColumnWidth(0, 32);
  ui_->listFavorites->setColumnWidth(1, 128);
  ui_->listFavorites->setColumnWidth(2, 96);
  
  connect(ui_->listColors, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
          this, SLOT(colorItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
  connect(ui_->listFavorites, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
          this, SLOT(favoritesItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
  connect(ui_->buttonAdd, SIGNAL(clicked()), this, SLOT(addToFavorites()));
  connect(ui_->buttonRemove, SIGNAL(clicked()), this, SLOT(removeFromFavorites()));
  connect(ui_->buttonMoveUp, SIGNAL(clicked()), this, SLOT(moveUp()));
  connect(ui_->buttonMoveDown, SIGNAL(clicked()), this, SLOT(moveDown()));
  connect(ui_->buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
  
  initList();
}

ColorDialog::~ColorDialog()
{
  delete ui_;
}

ldraw::color ColorDialog::getSelected() const
{
  return selectedColor_;
}

void ColorDialog::initList()
{
  QTreeWidgetItem *rootItem;
  QList<QTreeWidgetItem *> elist;
  
  /* initialize color list */
  rootItem = ui_->listColors->invisibleRootItem();
  
  elist = rootItem->takeChildren();
  foreach (QTreeWidgetItem *i, elist)
      delete i;
  
  colorMap_.clear();
  
  for (int i = 0; i < ldraw::color::color_chart_count; ++i) {
    const ldraw::color_entity *ce = &ldraw::color::color_chart[i];
    
    newItem(rootItem, ce);
    colorMap_[ce->id] = i;
  }
  
  /* initialize favorites list */
  rootItem = ui_->listFavorites->invisibleRootItem();
  
  elist = rootItem->takeChildren();
  foreach (QTreeWidgetItem *i, elist)
      delete i;
  
  foreach (const ldraw::color &c, manager_->colorList())
      newItem(rootItem, c.get_entity());
  
  resetFavoritesMap();
}

QTreeWidgetItem* ColorDialog::newItem(QTreeWidgetItem *parent, const ldraw::color_entity *ce)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  
  item->setData(0, Qt::DisplayRole, QString::number(ce->id));
  item->setData(1, Qt::DisplayRole, QString::fromStdString(ce->name));
  item->setData(2, Qt::DisplayRole, materialDescription(*ce));
  item->setData(1, Qt::DecorationRole, ColorManager::colorPixmap(ldraw::color(ce->id)));
  item->setData(0, Qt::UserRole, QVariant(ce->id));
  
  return item;
}

QString ColorDialog::materialDescription(const ldraw::color_entity &c) const
{
  switch (c.material) {
    case ldraw::material_normal:
      return tr("Solid");
    case ldraw::material_transparent:
      return tr("Transparent");
    case ldraw::material_luminant:
      return tr("Milky");
    case ldraw::material_glitter:
      return tr("Glitter");
    case ldraw::material_pearlescent:
      return tr("Pearl");
    case ldraw::material_chrome:
      return tr("Chrome");
    case ldraw::material_metallic:
      return tr("Metal");
    case ldraw::material_rubber:
      return tr("Rubber");
    case ldraw::material_speckle:
      return tr("Speckle");
    default:
      return QString();
  }
}

void ColorDialog::resetFavoritesMap()
{
  favoritesMap_.clear();
  
  QTreeWidgetItem *root = ui_->listFavorites->invisibleRootItem();
  
  for (int i = 0; i < root->childCount(); ++i) {
    ldraw::color c(root->child(i)->data(0, Qt::UserRole).toInt());
    const ldraw::color_entity *ce = c.get_entity();
    
    favoritesMap_[ce->id] = i;
  }
}

void ColorDialog::moveCurrentItem(int offset)
{
  QTreeWidgetItem *item = ui_->listFavorites->currentItem();
  QTreeWidgetItem *root = ui_->listFavorites->invisibleRootItem();
  
  if (!item)
    return;
  
  int index = root->indexOfChild(item);
  int cid = item->data(0, Qt::UserRole).toInt();
  
  QTreeWidgetItem *dest = root->child(index + offset);
  
  /* swap between two */
  QVariant d0 = item->data(0, Qt::DisplayRole);
  QVariant d1 = item->data(1, Qt::DisplayRole);
  QVariant d2 = item->data(2, Qt::DisplayRole);
  QVariant di = item->data(1, Qt::DecorationRole);
  QVariant du = item->data(0, Qt::UserRole);
  
  item->setData(0, Qt::DisplayRole, dest->data(0, Qt::DisplayRole));
  item->setData(1, Qt::DisplayRole, dest->data(1, Qt::DisplayRole));
  item->setData(2, Qt::DisplayRole, dest->data(2, Qt::DisplayRole));
  item->setData(1, Qt::DecorationRole, dest->data(1, Qt::DecorationRole));
  item->setData(0, Qt::UserRole, dest->data(0, Qt::UserRole));
  
  dest->setData(0, Qt::DisplayRole, d0);
  dest->setData(1, Qt::DisplayRole, d1);
  dest->setData(2, Qt::DisplayRole, d2);
  dest->setData(1, Qt::DecorationRole, di);
  dest->setData(0, Qt::UserRole, du);
  
  ui_->listFavorites->setCurrentItem(dest);
  
  resetFavoritesMap();
  
  changed_ = true;
}

void ColorDialog::apply()
{
  QTreeWidgetItem *selected = ui_->listColors->currentItem();
  
  if (!selected)
    return;
  
  selectedColor_ = ldraw::color(selected->data(0, Qt::UserRole).toInt());
  
  if (changed_)
    commitChanges();
  
  accept();
}

void ColorDialog::commitChanges()
{
  QList<ldraw::color> colorList;
  
  QTreeWidgetItem *root = ui_->listFavorites->invisibleRootItem();
  for (int i = 0; i < root->childCount(); ++i)
    colorList.append(root->child(i)->data(0, Qt::UserRole).toInt());
  
  manager_->setColorList(colorList);
}

void ColorDialog::colorItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  Q_UNUSED(previous);
  
  if (!current) {
    ui_->buttonAdd->setEnabled(false);
    return;
  } else {
    ui_->buttonAdd->setEnabled(true);
  }
  
  int cid = current->data(0, Qt::UserRole).toInt();
  
  if (favoritesMap_.contains(cid))
    ui_->listFavorites->setCurrentItem(ui_->listFavorites->invisibleRootItem()->child(favoritesMap_[cid]));
  else
    ui_->listFavorites->setCurrentItem(0L);
}

void ColorDialog::favoritesItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  Q_UNUSED(previous);
  
  QTreeWidgetItem *root = ui_->listFavorites->invisibleRootItem();
  
  if (!current) {
    ui_->buttonRemove->setEnabled(false);
    ui_->buttonMoveUp->setEnabled(false);
    ui_->buttonMoveDown->setEnabled(false);
    return;
  } else {
    ui_->buttonRemove->setEnabled(true);
    if (root->indexOfChild(current) != 0)
      ui_->buttonMoveUp->setEnabled(true);
    else
      ui_->buttonMoveUp->setEnabled(false);
    if (root->indexOfChild(current) != root->childCount() - 1)
      ui_->buttonMoveDown->setEnabled(true);
    else
      ui_->buttonMoveDown->setEnabled(false);
  }
  
  int cid = current->data(0, Qt::UserRole).toInt();
  
  ui_->listColors->setCurrentItem(ui_->listColors->invisibleRootItem()->child(colorMap_[cid]));
  
}

void ColorDialog::addToFavorites()
{
  QTreeWidgetItem *item = ui_->listColors->currentItem();
  
  if (!item)
    return;
  
  int cid = item->data(0, Qt::UserRole).toInt();
  if (favoritesMap_.contains(cid))
    return;
  
  QTreeWidgetItem *newitem = newItem(ui_->listFavorites->invisibleRootItem(), ldraw::color(cid).get_entity());
  resetFavoritesMap();
  
  ui_->listFavorites->setCurrentItem(newitem);
  
  changed_ = true;
}

void ColorDialog::removeFromFavorites()
{
  QTreeWidgetItem *item = ui_->listFavorites->currentItem();
  
  if (!item)
    return;
  
  delete item;
  
  resetFavoritesMap();
  
  changed_ = true;
}

void ColorDialog::moveUp()
{
  moveCurrentItem(-1);
}

void ColorDialog::moveDown()
{
  moveCurrentItem(1);
}

}


