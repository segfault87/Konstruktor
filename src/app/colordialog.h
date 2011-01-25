// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COLORDIALOG_H_
#define _COLORDIALOG_H_

#include <libldr/color.h>

#include <QDialog>
#include <QMap>

namespace Ui
{
	class ColorDialog;
}

class QWidget;
class QTreeWidgetItem;

class KonstruktorColorManager;

class KonstruktorColorDialog : public QDialog
{
	Q_OBJECT;
	
  public:
	KonstruktorColorDialog(QWidget *parent = 0L);
	~KonstruktorColorDialog();

	ldraw::color getSelected() const;
	
  private:
	void initList();
	QTreeWidgetItem* newItem(QTreeWidgetItem *parent, const ldraw::color_entity *ce);
	QString materialDescription(const ldraw::color_entity &c) const;
	void resetFavoritesMap();
	void commitChanges();
	void moveCurrentItem(int offset);

  private slots:
	void apply();
	void colorItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void favoritesItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void addToFavorites();
	void removeFromFavorites();
	void moveUp();
	void moveDown();

  private:
	KonstruktorColorManager *manager_;
	Ui::ColorDialog *ui_;

	QMap<int, int> colorMap_;
	QMap<int, int> favoritesMap_;

	bool changed_;
	ldraw::color selectedColor_;
};

#endif
