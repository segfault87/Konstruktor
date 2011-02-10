// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <string>

#include <QList>
#include <QSet>
#include <QPair>
#include <QVector>

#ifdef KONSTRUKTOR_NO_KDE
#include "qt/mainwindow.h"
#else
#include "kde/mainwindow.h"
#endif

namespace ldraw 
{
	class model;
}

class QAction;
class QCloseEvent;
class QModelIndex;
class QTreeView;

namespace Konstruktor
{

class ContentsModel;
class ContentsView;
class Document;
class Editor;
class PartsWidget;
class RenderWidget;

class MainWindow : public Platform::MainWindow
{
	Q_OBJECT;
	
  public:
	MainWindow(QWidget *parent = 0L);
	~MainWindow();

	unsigned int viewportModes() const;
	const Editor* editor() const { return editorGroup_; }
	
  signals:
	void activeModelChanged(ldraw::model *m);
	void viewChanged();
									  
  public slots:
	void modelModified(const QSet<int> &selection);
	void updateViewports();
	void changeCaption();
	void activate(bool b);
	void setStatusMessage(const QString &msg);
	
	void newFile();
	void openFile();
	void openFile(const KUrl &url);
	void closeFile();
	void saveFile();
	void saveFileAs();
	void quit();

	void newSubmodel();
	void deleteSubmodel();
	void modelProperties();

	void resetZoom();
	void resetDisplay();

	void render();

	void showConfigDialog();

  private slots:
	void activeDocumentChanged(int index);
	void activeModelChanged(const std::string &name);
	void modelChanged(ldraw::model *m);
	void submodelViewDoubleClicked(const QModelIndex &index);
	void selectionChanged(const QSet<int> &cset);
	void gridModeChanged(QAction *action);
	void modelModified();
	void clipboardChanged();

	void notImplemented();

  private:
	void closeEvent(QCloseEvent *event);
	
	void initGui();
	void initObjects();
	void initActions();
	void initConnections();
	bool confirmQuit();
	bool doSave(Document *document, bool newname = false);
	
  private:
	/*
	 * Document management
	 */
	Document *activeDocument_;
	QVector<QPair<QString, Document *> > documents_;
	QSet<QString> openedUrls_;
	ContentsModel *contentsModel_;
	Editor *editorGroup_;
	
	/*
	 * Widgets
	 */
	PartsWidget *partsWidget_;
	QTreeView *submodelList_;
	ContentsView *contentList_;	
	RenderWidget *renderWidget_[4];
	QTabBar *tabbar_;

	/*
	 * State management
	 */
	bool enabled_;
	int newcnt_;

	/*
	 * Actions
	 */
	QList<QAction *> stateChangeableActions_;
	QList<QAction *> selectionDependentActions_;
	
	// File menu
	QAction *actionNew_;
	QAction *actionOpen_;
	KRecentFilesAction *actionOpenRecent_;
	QAction *actionClose_;
	QAction *actionSave_;
	QAction *actionSaveAs_;
	QAction *actionQuit_;

	// Edit menu
	QAction *actionCopy_;
	QAction *actionCut_;
	QAction *actionPaste_;
	QAction *actionUndo_;
	QAction *actionRedo_;
	QAction *actionSelectAll_;
	QAction *actionSelectNone_;
	QAction *actionHide_;
	QAction *actionUnhideAll_;
	QAction *actionColor_;
	QAction *actionRotationPivot_;
	QAction *actionGridSparse_;
	QAction *actionGridNormal_;
	QAction *actionGridDense_;
	QAction *actionGridNone_;
	QAction *actionDelete_;
	QAction *actionMoveByXPositive_;
	QAction *actionMoveByXNegative_;
	QAction *actionMoveByYPositive_;
	QAction *actionMoveByYNegative_;
	QAction *actionMoveByZPositive_;
	QAction *actionMoveByZNegative_;
	QAction *actionRotateByXClockwise_;
	QAction *actionRotateByXCounterClockwise_;
	QAction *actionRotateByYClockwise_;
	QAction *actionRotateByYCounterClockwise_;
	QAction *actionRotateByZClockwise_;
	QAction *actionRotateByZCounterClockwise_;

	// View menu
	QAction *actionResetZoom_;
	QAction *actionReset3dView_;

	// Submodel menu
	QAction *actionNewSubmodel_;
	QAction *actionDeleteSubmodel_;
	QAction *actionCollapseSubmodel_;
	QAction *actionModelProperties_;

	// Render menu
	QAction *actionRender_;
	QAction *actionRenderSteps_;
	QAction *actionRenderSetup_;

	// Setting menu
	QAction *actionConfigure_;
};

}

#endif
