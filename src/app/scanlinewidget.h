// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _SCANLINEWIDGET_H_
#define _SCANLINEWIDGET_H_

#include <QWidget>

class QImage;
class QPaintEvent;

class KonstruktorScanlineWidget : public QWidget
{
	Q_OBJECT;
	
  public:
	KonstruktorScanlineWidget(const QImage &image, QWidget *parent = 0L);
	~KonstruktorScanlineWidget();

	QSize sizeHint() const;
						   
  public slots:
	void updateLine(int line);

  private:
	virtual void paintEvent(QPaintEvent *event);

	const QImage &image_;
	int lastline_;
};

#endif
