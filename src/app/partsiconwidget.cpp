// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QMimeData>

#include "partitems.h"

#include "partsiconwidget.h"

namespace Konstruktor
{

PartsIconWidget::PartsIconWidget(QWidget *parent)
	: QListWidget(parent)
{
	setViewMode(QListWidget::IconMode);
	setMovement(QListWidget::Static);
	setResizeMode(QListWidget::Adjust);
	setDragEnabled(true);
}

PartsIconWidget::~PartsIconWidget()
{

}

QStringList PartsIconWidget::mimeTypes() const
{	
	QStringList types;
	types << "application/konstruktor-refobject";
	return types;
}

QMimeData* PartsIconWidget::mimeData(const QList<QListWidgetItem *> items) const
{
	if (items.size() != 1)
		return 0L;

	PartItem item = items[0]->data(Qt::UserRole).value<PartItem>();
	
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData = item.serialize();

	mimeData->setData("application/konstruktor-refobject", encodedData);
	return mimeData;
}

}
