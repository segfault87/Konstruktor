// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QScrollBar>

#include "scanlinewidget.h"

KonstruktorScanlineWidget::KonstruktorScanlineWidget(const QImage &image, QWidget *parent)
	: QWidget(parent), image_(image)
{
	lastline_ = 0;
}

KonstruktorScanlineWidget::~KonstruktorScanlineWidget()
{

}

QSize KonstruktorScanlineWidget::sizeHint() const
{
	return QSize(image_.width(), image_.height());
}

void KonstruktorScanlineWidget::updateLine(int line)
{
	update(0, lastline_, image_.width(), line - lastline_ + 1);
	lastline_ = line;
}

void KonstruktorScanlineWidget::paintEvent(QPaintEvent *ev)
{
	QPainter painter(this);
	const QRect &rect = ev->rect();

	painter.drawImage(rect.x(), rect.y(), image_, rect.x(), rect.y(), rect.width(), rect.height());
}

KonstruktorScanlineWidgetContainer::KonstruktorScanlineWidgetContainer(const QImage &image, QWidget *parent)
	: QScrollArea(parent)
{
	scanlineWidget_ = new KonstruktorScanlineWidget(image, this);

	setWidget(scanlineWidget_);
}

KonstruktorScanlineWidgetContainer::~KonstruktorScanlineWidgetContainer()
{

}

QSize KonstruktorScanlineWidgetContainer::sizeHint() const
{
	int xsb = 0, ysb = 0;

	if (verticalScrollBar())
		xsb = verticalScrollBar()->sizeHint().height();

	if (horizontalScrollBar())
		ysb = horizontalScrollBar()->sizeHint().width();

	return QSize(1024 + xsb, 768 + ysb);
}

KonstruktorScanlineWidget* KonstruktorScanlineWidgetContainer::scanlineWidget()
{
	return scanlineWidget_;
}

