// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QList>
#include <QPixmapCache>
#include <QSortFilterProxyModel>

#include "dbmanager.h"
#include "application.h"
#include "partitems.h"
#include "partsmodel.h"
#include "ui_partswidget.h"

#include "partswidget.h"

KonstruktorPartsWidget::KonstruktorPartsWidget(QWidget *parent)
	: QWidget(parent)
{
	lastCat_ = -1;
	
	hideUnofficial_ = false;
	
	ui_ = new Ui::PartsWidget;
	ui_->setupUi(this);
	
	model_ = new KonstruktorPartsModel(categories_, categorymap_, list_, this);
	sortModel_ = new QSortFilterProxyModel(model_);
	sortModel_->setSourceModel(model_);

	initialize(search_, hideUnofficial_);
	
	ui_->partView->setModel(sortModel_);
	ui_->partView->setSortingEnabled(true);
	ui_->partView->sortByColumn(0, Qt::AscendingOrder);

	connect(ui_->hideUnofficial, SIGNAL(stateChanged(int)), this, SLOT(hideUnofficial(int)));
	connect(ui_->partView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &, const QModelIndex &)));
	connect(ui_->iconView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(iconSelected(QListWidgetItem *)));
}

KonstruktorPartsWidget::~KonstruktorPartsWidget()
{
	delete ui_;
}

void KonstruktorPartsWidget::initialize(const QString &search, bool hideUnofficial)
{
	QString subq1;
	QString subq2;
	if (hideUnofficial)
		subq1 = QString("p.unofficial = 0 AND");
	if (!search.isEmpty())
		subq2 = QString("p.desc LIKE '%%1%' AND").arg(search);
	
	categories_.clear();
	categorymap_.clear();
	list_.clear();
	
	KonstruktorDBManager *db = KonstruktorApplication::self()->database();

	QStringList cats = db->query("SELECT category, id, visibility FROM categories WHERE visibility < 2 ORDER BY category ASC");
	int i = 0;
	for (QStringList::Iterator it = cats.begin(); it != cats.end(); ++it, ++i) {
		QString name = *it++;
		int id = (*it++).toInt();
		int visibility = (*it).toInt();
		categories_.append(KonstruktorPartCategory(name, id, visibility, i));
		categorymap_[id] = &categories_[i];
	}

	QStringList parts = db->query(QString("SELECT p.desc, p.filename, p.minx, p.miny, p.minz, p.maxx, p.maxy, p.maxz, pc.catid FROM parts AS p, part_categories AS pc WHERE %1 %2 pc.partid = p.id ORDER BY p.desc ASC").arg(subq1, subq2));
	for (QStringList::Iterator it = parts.begin(); it != parts.end(); ++it) {
		QString desc = *it++;
		QString fn = *it++;
		ldraw::vector min((*it++).toFloat(), (*it++).toFloat(), (*it++).toFloat());
		ldraw::vector max((*it++).toFloat(), (*it++).toFloat(), (*it++).toFloat());
		list_[(*it).toInt()].append(KonstruktorPartItem(categorymap_[(*it).toInt()], desc, fn, ldraw::metrics(min, max)));
	}

	// Delete if there is no part in the category
	for (int i = categories_.size() - 1; i >= 0; --i) {
		if (!list_[categories_[i].id()].size())
			categories_.removeAt(i);
	}

	model_->reset();
}

void KonstruktorPartsWidget::hideUnofficial(int checkState)
{
	if (checkState == Qt::Checked)
		hideUnofficial_ = true;
	else
		hideUnofficial_ = false;
	
	initialize(search_, hideUnofficial_);
}

void KonstruktorPartsWidget::selectionChanged(const QModelIndex &current, const QModelIndex &)
{
	if (!current.isValid())
		return;

	QModelIndex index = sortModel_->mapToSource(current);

	int cat;
	if (index.parent().isValid())
		cat = index.parent().row();
	else
		cat = index.row();

	if (lastCat_ != cat) {
		lastCat_ = cat;
		
		ui_->iconView->clear();

		QString saveLocation = KonstruktorApplication::self()->saveLocation("partimgs/");
		for (QList<KonstruktorPartItem>::ConstIterator it = list_[categories_[cat].id()].constBegin(); it != list_[categories_[cat].id()].constEnd(); ++it) {
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

void KonstruktorPartsWidget::iconSelected(QListWidgetItem *item)
{
	/*ui_->partView->selectionModel()->select(
		sortModel_->mapToSource(model_->index(ui_->iconView->row(item), 0, model_->index(lastCat_, 0)),
		QItemSelectionModel::SelectCurrent);*/
}
