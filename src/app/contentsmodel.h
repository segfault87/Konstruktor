// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _CONTENTSMODEL_H_
#define _CONTENTSMODEL_H_

#include <QAbstractItemModel>
#include <QSet>

#include "commandbase.h"
#include "visibilityextension.h"

namespace ldraw { class model; }

class KonstruktorDocument;
class KonstruktorVisibilityExtension;

class KonstruktorContentsModel : public QAbstractItemModel
{
	Q_OBJECT;
	
  public:
	enum {
		ColumnCheck,
		ColumnIndex,
		ColumnObjectType,
		ColumnPosition,
		ColumnData,
		ColumnCount
	};
	
	KonstruktorContentsModel(QObject *parent = 0L);
	virtual ~KonstruktorContentsModel();

	void setDocument(KonstruktorDocument *document);

	// implementation

	int columnCount(const QModelIndex &) const { return ColumnCount; }
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &) const { return QModelIndex(); }

	Qt::ItemFlags flags(const QModelIndex &index) const;

  signals:
	void hide(const QModelIndex &index);
	void unhide(const QModelIndex &index);
	void viewChanged();

  public slots:
	void rowsChanged(const QPair<KonstruktorCommandBase::AffectedRow, QSet<int> > &);
	void hideSelected(const QSet<int> &selection);
	void unhideAll();

  private slots:
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	
  private:
	ldraw::model *model_;
	KonstruktorVisibilityExtension *checkTable_;
};

#endif
