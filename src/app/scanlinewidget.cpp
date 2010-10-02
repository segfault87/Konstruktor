// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>

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
	int w, h;

	if (image_.width() > 800)
		w = 800;
	else
		w = image_.width();

	if (image_.height() > 600)
		h = 600;
	else
		h = image_.height();

	return QSize(w, h);
}

void KonstruktorScanlineWidget::updateLine(int line)
{
	repaint(0, lastline_, image_.width(), line - lastline_ + 1);
	lastline_ = line;
}

void KonstruktorScanlineWidget::paintEvent(QPaintEvent *ev)
{
	QPainter painter(this);
	const QRect &rect = ev->rect();

	painter.drawImage(rect.x(), rect.y(), image_, rect.x(), rect.y(), rect.width(), rect.height());
}

