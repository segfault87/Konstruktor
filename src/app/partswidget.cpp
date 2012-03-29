// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QList>
#include <QPixmapCache>
#include <QSortFilterProxyModel>
#include <QTimer>

#include "dbmanager.h"
#include "application.h"
#include "partitems.h"
#include "partsmodel.h"
#include "ui_partswidget.h"

#include "partswidget.h"

namespace Konstruktor
{

PartsWidget::PartsWidget(QWidget *parent)
	: QWidget(parent)
{
  lastCat_ = -1;
  searchDelay_ = new QTimer(this);
  searchDelay_->setSingleShot(true);
  
  hideUnofficial_ = false;
  
  ui_ = new Ui::PartsWidget;
  ui_->setupUi(this);
  
  model_ = new PartsModel(categories_, categorymap_, list_, this);
  sortModel_ = new QSortFilterProxyModel(this);
  
  initialize();
  resetItems(search_, hideUnofficial_);
  
  sortModel_->setSourceModel(model_);
  ui_->partView->setModel(sortModel_);
  ui_->partView->setSortingEnabled(true);
  ui_->partView->sortByColumn(0, Qt::AscendingOrder);
  
  connect(searchDelay_, SIGNAL(timeout()), this, SLOT(search()));
  connect(ui_->searchEdit, SIGNAL(textEdited(const QString &)), this, SLOT(searchTextChanged(const QString &)));
  connect(ui_->hideUnofficial, SIGNAL(stateChanged(int)), this, SLOT(hideUnofficial(int)));
  connect(ui_->partView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &, const QModelIndex &)));
  connect(ui_->iconView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(iconSelected(QListWidgetItem *)));
}

PartsWidget::~PartsWidget()
{
  delete ui_;
}

void PartsWidget::resetItems(const QString &search, bool hideUnofficial)
{
  /* FIXME optimize */
  
  QString subq1;
  QString subq2;
  if (hideUnofficial)
    subq1 = QString("p.unofficial = 0 AND");
  if (!search.isEmpty())
    subq2 = QString("(id IN (SELECT partid AS id FROM part_keywords WHERE keyword LIKE '%%1%') OR p.desc LIKE '%%1%' OR p.partid LIKE '%%1%') AND").arg(search);
  
  list_.clear();
  catidmap_.clear();
  
  categories_ = allCategories_;
  
  DBManager *db = Application::self()->database();
  
  QString query = QString("SELECT p.desc, p.filename, p.minx, p.miny, p.minz, p.maxx, p.maxy, p.maxz, pc.catid FROM parts AS p, part_categories AS pc WHERE %1 %2 pc.partid = p.id ORDER BY p.desc ASC").arg(subq1, subq2);
  
  QStringList parts = db->query(query);
  for (QStringList::Iterator it = parts.begin(); it != parts.end(); ++it) {
    QString desc = *it++;
    QString fn = *it++;
    
    float minx, miny, minz;
    float maxx, maxy, maxz;
    
    minx = (*it++).toFloat();
    miny = (*it++).toFloat();
    minz = (*it++).toFloat();
    
    maxx = (*it++).toFloat();
    maxy = (*it++).toFloat();
    maxz = (*it++).toFloat();
    
    list_[(*it).toInt()].append(PartItem(categorymap_[(*it).toInt()], desc, fn, ldraw::metrics(ldraw::vector(minx, miny, minz), ldraw::vector(maxx, maxy, maxz))));
  }
  
  // Delete if there is no part in the category
  for (int i = categories_.size() - 1; i >= 0; --i) {
    if (!list_[categories_[i].id()].size())
      categories_.removeAt(i);
  }
  
  int j = 0;
  for (int i = 0; i < categories_.size(); ++i) {
    catidmap_[categories_[i].index()] = j++;
  }
  
  model_->reset();
}

void PartsWidget::hideUnofficial(int checkState)
{
  if (checkState == Qt::Checked)
    hideUnofficial_ = true;
  else
    hideUnofficial_ = false;
  
  resetItems(search_, hideUnofficial_);
}

void PartsWidget::selectionChanged(const QModelIndex &current, const QModelIndex &)
{
  if (!current.isValid())
    return;
  
  QModelIndex index = sortModel_->mapToSource(current);
  
  int cat;
  if (index.parent().isValid())
    cat = catidmap_[index.parent().row()];
  else
    cat = index.row();
  
  if (lastCat_ != cat) {
    lastCat_ = cat;
    
    ui_->iconView->clear();
    
    QString saveLocation = Application::self()->saveLocation("partimgs/");
    for (QList<PartItem>::ConstIterator it = list_[categories_[cat].id()].constBegin(); it != list_[categories_[cat].id()].constEnd(); ++it) {
      QListWidgetItem *obj = new QListWidgetItem(ui_->iconView);
      
      QPixmap pixmap;
      if (!QPixmapCache::find((*it).filename(), pixmap)) {
        pixmap = QPixmap(saveLocation + (*it).filename() + ".png").scaledToHeight(64, Qt::SmoothTransformation);
        QPixmapCache::insert((*it).filename(), pixmap);
      }
      
      obj->setData(Qt::DecorationRole, pixmap);
      obj->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
      obj->setData(Qt::UserRole, QVariant::fromValue(*it));
      ui_->iconView->addItem(obj);
    }
  }
  
  if (index.parent().isValid())
    ui_->iconView->setCurrentItem(ui_->iconView->item(index.row()));
}

void PartsWidget::searchTextChanged(const QString &ref)
{
  search_ = ref;
  
  if (searchDelay_->isActive())
    searchDelay_->stop();
  
  searchDelay_->start(1000);
}

void PartsWidget::search()
{
  resetItems(ui_->searchEdit->text(), hideUnofficial_);
}

void PartsWidget::iconSelected(QListWidgetItem *item)
{
  Q_UNUSED(item);
  /*ui_->partView->selectionModel()->select(
    sortModel_->mapToSource(model_->index(ui_->iconView->row(item), 0, model_->index(lastCat_, 0)),
    QItemSelectionModel::SelectCurrent);*/
}

void PartsWidget::initialize()
{
  DBManager *db = Application::self()->database();
  
  QStringList cats = db->query("SELECT category, id, visibility FROM categories WHERE visibility < 2 ORDER BY category ASC");
  int i = 0;
  for (QStringList::Iterator it = cats.begin(); it != cats.end(); ++it, ++i) {
    QString name = *it++;
    int id = (*it++).toInt();
    int visibility = (*it).toInt();
    allCategories_.append(PartCategory(name, id, visibility, i));
    categorymap_[id] = &allCategories_[i];
  }
}

}
