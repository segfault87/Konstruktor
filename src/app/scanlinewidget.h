// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _SCANLINEWIDGET_H_
#define _SCANLINEWIDGET_H_

#include <QScrollArea>
#include <QWidget>

class QImage;
class QPaintEvent;

namespace Konstruktor
{

class ScanlineWidget : public QWidget
{
	Q_OBJECT;
	
  public:
	ScanlineWidget(const QImage &image, QWidget *parent = 0L);
	~ScanlineWidget();

	QSize sizeHint() const;
						   
  public slots:
	void updateLine(int line);

  private:
	virtual void paintEvent(QPaintEvent *event);

	const QImage &image_;
	int lastline_;
};

class ScanlineWidgetContainer : public QScrollArea
{
  public:
	ScanlineWidgetContainer(const QImage &image, QWidget *parent = 0L);
	~ScanlineWidgetContainer();

	QSize sizeHint() const;

	ScanlineWidget* scanlineWidget();

  private:
	ScanlineWidget *scanlineWidget_;
};

}

#endif
