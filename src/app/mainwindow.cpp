// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDialog>
#include <QDockWidget>
#include <QGLFormat>
#include <QHeaderView>
#include <QSplitter>
#include <QStringList>
#include <QTabBar>
#include <QTreeView>
#include <QUrl>
#include <QVBoxLayout>

#include <kactioncollection.h>
#include <kcmdlineargs.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <krecentfilesaction.h>
#include <kselectaction.h>
#include <kshortcut.h>
#include <kstandardaction.h>
#include <ktabbar.h>
#include <ktemporaryfile.h>
#include <kurl.h>
#include <kio/netaccess.h>

#include <libldr/utils.h>

#include "configdialog.h"
#include "contentsmodel.h"
#include "contentsview.h"
#include "document.h"
#include "editor.h"
#include "application.h"
#include "newmodeldialog.h"
#include "newsubmodeldialog.h"
#include "objectlist.h"
#include "partswidget.h"
#include "povrayrenderparameters.h"
#include "povrayrenderwidget.h"
#include "renderwidget.h"
#include "submodelmodel.h"
#include "submodelwidget.h"
#include "utils.h"

#include "mainwindow.h"

namespace Konstruktor
{

#define EXIT_IF_NO_DOCUMENT if (!activeDocument_) return

MainWindow::MainWindow(QWidget *parent)
	: Platform::MainWindow(parent)
{
	activeDocument_ = 0L;
	newcnt_ = 1;
	
	// set up the main window
	initObjects();
	initGui();
	initConnections();
	initActions();

	setupPlatformGui();

	// to make sure that there is no open models.
	activeDocumentChanged(-1);

	// parse cmd line args
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	for (int i = 0; i < args->count(); ++i)
		openFile(QUrl(args->arg(i)));

	activate(false);
	
	setStatusMessage(i18n("Ready..."));
}

MainWindow::~MainWindow()
{
	// Save recent file entries
	actionOpenRecent_->saveEntries(KGlobal::config()->group("Recent Files"));

	// deselect
	activeDocument_ = 0L;
	activeDocumentChanged(-1);

	for (int i = 0; i < documents_.size(); ++i)
		delete documents_[i].second;
}

// returns currently using viewport modes in bit array format
unsigned int MainWindow::viewportModes() const
{
	int modes = 0;

	for (int i = 0; i < 4; ++i)
		modes |= 1 << renderWidget_[i]->viewportMode();

	return modes;
}

void MainWindow::modelModified(const QSet<int> &)
{
	updateViewports();
}

void MainWindow::clipboardChanged()
{
	EXIT_IF_NO_DOCUMENT;

	const QMimeData *mimeData = qApp->clipboard()->mimeData(QClipboard::Clipboard);

	if (!mimeData)
		actionPaste_->setEnabled(false);
	else if (!mimeData->hasFormat(ObjectList::mimeType))
		actionPaste_->setEnabled(false);
	else
		actionPaste_->setEnabled(true);
}

void MainWindow::updateViewports()
{
	for (int i = 0; i < 4; ++i) {
		renderWidget_[i]->update();
	}
}

void MainWindow::changeCaption()
{
	if (activeDocument_) {
		ldraw::model *main_model = activeDocument_->contents()->main_model();
		setCaption(QString("%2 - %1").arg(main_model->desc().c_str(), main_model->name().c_str()));
	} else {
		setCaption("");
	}
}

void MainWindow::activate(bool b)
{
	enabled_ = b;
	contentList_->setEnabled(b);
	submodelList_->setEnabled(b);
	for (int i = 0; i < 4; ++i)
		renderWidget_[i]->setEnabled(b);
	
	for (QList<QAction *>::iterator it = stateChangeableActions_.begin(); it != stateChangeableActions_.end(); ++it)
		(*it)->setEnabled(b);

	actionRender_->setEnabled(Application::self()->hasPovRay());
	//actionRenderSteps_->setEnabled(Application::self()->hasPovRay());
}

void MainWindow::setStatusMessage(const QString &msg)
{
	statusBar()->showMessage(msg);
}

void MainWindow::newFile()
{
	NewModelDialog *dialog = new NewModelDialog(this);

	dialog->exec();

	if (dialog->result() == QDialog::Accepted) {
		QString filename = QString("unnamed%1.ldr").arg(newcnt_);
		Document *document = new Document(filename, dialog->textDesc(), dialog->textAuthor());

		// Initialize connection
		connect(document, SIGNAL(undoStackAdded(QUndoStack *)), editorGroup_, SLOT(stackAdded(QUndoStack *)));
		connect(document, SIGNAL(undoStackChanged(QUndoStack *)), editorGroup_, SLOT(setActiveStack(QUndoStack *)));

		document->sendSignals();

		// append into tab bar
		documents_.append(QPair<QString, Document *>(QString(), document));
		int tabidx = tabbar_->addTab(filename);
		tabbar_->setTabIcon(tabidx, KIcon("text-plain"));
		tabbar_->setCurrentIndex(tabidx);
		activeDocument_ = document;
		
		++newcnt_;

		setStatusMessage(i18n("New document created."));
	}
	
	dialog->close();
	delete dialog;
}

void MainWindow::openFile()
{
	QStringList mimes;
	mimes << "application/x-ldraw";
	mimes << "application/x-multi-part-ldraw";
	
	KFileDialog dialog(KUrl(), QString(), this);
	dialog.setMimeFilter(mimes, "application/x-ldraw");
	dialog.setCaption(i18n("Select document(s)"));
	dialog.setOperationMode(KFileDialog::Opening);
	dialog.setMode(KFile::Files | KFile::ExistingOnly);
	dialog.exec();

	KUrl::List urls = dialog.selectedUrls();
	if (urls.isEmpty())
			return;

	for (KUrl::List::Iterator it = urls.begin(); it != urls.end(); ++it)
		openFile(*it);
}


void MainWindow::openFile(const QUrl &url)
{
	QUrl aurl = url;
	if ((url.scheme().isEmpty() || url.scheme() == "file") && url.isRelative()) {
		aurl = QUrl(QDir::currentPath() + "/").resolved(url);
	}		
	
	QString strUrl = aurl.toLocalFile();
	
	if (openedUrls_.contains(strUrl)) {
		for (int i = 0; i < documents_.size(); ++i) {
			if (documents_[i].first == strUrl) {
				tabbar_->setCurrentIndex(i);
				return;
			}
		}
	}
	
	Document *document = 0L;
	try {
		QString tmploc;
		
		if (!KIO::NetAccess::download(aurl, tmploc, 0L)) {
			KMessageBox::error(this, i18n("Could not open document '%1'.", aurl.toLocalFile()));
			return;
		}
		
		document = new Document(tmploc, aurl);

		// Initialize connection
		connect(document, SIGNAL(undoStackAdded(QUndoStack *)), editorGroup_, SLOT(stackAdded(QUndoStack *)));
		connect(document, SIGNAL(undoStackChanged(QUndoStack *)), editorGroup_, SLOT(setActiveStack(QUndoStack *)));

		document->sendSignals();

		// append into document list, tab bar
		openedUrls_.insert(strUrl);
		documents_.append(QPair<QString, Document *>(strUrl, document));
		int tabidx = tabbar_->addTab(Utils::urlFileName(url));
		tabbar_->setTabIcon(tabidx, KIcon("text-plain"));
		tabbar_->setCurrentIndex(tabidx);

		activeDocument_ = document;

		actionOpenRecent_->addUrl(aurl);
	} catch (const ldraw::exception &e) {
		if (document)
			delete document;
		
		KMessageBox::error(this, i18n("Could not open a file: %1", e.details().c_str()));
	}

	setStatusMessage(i18n("Document '%1' opened.", aurl.path()));
}

void MainWindow::closeFile()
{
	EXIT_IF_NO_DOCUMENT;

	if (activeDocument_->canSave()) {
		switch (KMessageBox::warningYesNoCancel(this, i18n("The document \"%1\" has been modified. Do you want to save it?", Utils::urlFileName(activeDocument_->location())), QString(), KStandardGuiItem::save(), KStandardGuiItem::discard())) {
			case KMessageBox::Yes:
				if (!doSave(activeDocument_, false))
					return;
				break;
			case KMessageBox::Cancel:
				return;
		}
	}

	Document *t = activeDocument_;
	int ci = tabbar_->currentIndex();
	openedUrls_.remove(t->location().toString());
	documents_.remove(ci);
	tabbar_->removeTab(ci);

	if (ci >= tabbar_->count())
		--ci;
	
	tabbar_->setCurrentIndex(ci);

	delete t;
}

void MainWindow::saveFile()
{
	EXIT_IF_NO_DOCUMENT;
	
	doSave(activeDocument_, false);
}

void MainWindow::saveFileAs()
{
	if (!activeDocument_)
		return;
	
	doSave(activeDocument_, true);
}

void MainWindow::newSubmodel()
{
	NewSubmodelDialog *dialog = new NewSubmodelDialog(this);

  retry:
	dialog->exec();

	if (dialog->result() == QDialog::Accepted) {
		ldraw::model *sm = activeDocument_->newSubmodel(dialog->textName().toLocal8Bit().data(), dialog->textDesc().toLocal8Bit().data(), activeDocument_->contents()->main_model()->author());
		if (!sm) {
			KMessageBox::error(dialog, i18n("The name '%1' is already in use. Try using another one.", dialog->textName()));
			goto retry;
		} else {
			submodelList_->reset();
		}
	}

	modelModified();
	
	delete dialog;
}

void MainWindow::deleteSubmodel()
{
	EXIT_IF_NO_DOCUMENT;

	if (activeDocument_->getActiveModel() == activeDocument_->contents()->main_model())
		return;

	if (ldraw::utils::affected_models(activeDocument_->contents(), activeDocument_->getActiveModel()).size() > 0) {
		KMessageBox::error(this, i18n("This submodel is included in somewhere else."));
		return;
	}

	if (KMessageBox::questionYesNo(this, i18n("This operation cannot be undone. Would you like to proceed?")) == KMessageBox::No)
		return;

	ldraw::model *d = activeDocument_->getActiveModel();
	
	activeModelChanged("");
	
	activeDocument_->deleteSubmodel(d);
	activeDocument_->model()->resetItems();

	modelModified();
}

void MainWindow::modelProperties()
{
	notImplemented();
}

void MainWindow::quit()
{
	// FIXME did i miss something?

	if (confirmQuit()) {
		qApp->quit();
	}
}

void MainWindow::resetZoom()
{
	if (!activeDocument_)
		return;
	
	activeDocument_->recalibrateScreenDimension();

	emit viewChanged();
}

void MainWindow::resetDisplay()
{
	if (!activeDocument_)
		return;

	activeDocument_->resetPerspective();

	for (int i = 0; i < 4; ++i) {
		if (renderWidget_[i]->viewportMode() == RenderWidget::Free) {
			renderWidget_[i]->update();
			return;
		}
	}
}

void MainWindow::render()
{
	if (!activeDocument_)
		return;

	POVRayRenderParameters param;
	
	POVRayRenderWidget dialog(param, activeDocument_->getActiveModel(), this);
	dialog.show();
	dialog.start();
	dialog.exec();
}

void MainWindow::showConfigDialog()
{
	notImplemented();
	/*
	ConfigDialog dialog(this);
	dialog.exec();
	*/
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	if (confirmQuit()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void MainWindow::activeDocumentChanged(int index)
{
	if (!enabled_)
		activate(true);
	else if (index == -1)
		activate(false);

	if (index >= 0) {
		activeDocument_ = documents_[index].second;
		
		if (activeDocument_->canSave())
			actionSave_->setEnabled(true);
		else
			actionSave_->setEnabled(false);

		editorGroup_->setActiveStack(activeDocument_->activeUndoStack());
	} else {
		activeDocument_ = 0L;

		editorGroup_->setActiveStack(0L);
	}
		
	// reset the content list
	contentsModel_->setDocument(activeDocument_);
	contentList_->scrollToBottom();

	// reset the submodel list
	if (activeDocument_)
		submodelList_->setModel(activeDocument_->model());
	
	changeCaption();

	for (QList<QAction *>::iterator it = selectionDependentActions_.begin(); it != selectionDependentActions_.end(); ++it)
	  (*it)->setEnabled(false);

	if (activeDocument_)
		emit activeModelChanged(activeDocument_->getActiveModel());
	else
		emit activeModelChanged(0L);

	emit viewChanged();
}

void MainWindow::activeModelChanged(const std::string &name)
{
	if (name.empty()) {
		activeDocument_->setActiveModel(activeDocument_->contents()->main_model());
	} else if (!activeDocument_->setActiveModel(name)) {
		return;
	}
	
	// reset the content list
	contentsModel_->setDocument(activeDocument_);
	contentList_->scrollToBottom();

	activeDocument_->resetPerspective();
	activeDocument_->recalibrateScreenDimension();

	for (QList<QAction *>::iterator it = selectionDependentActions_.begin(); it != selectionDependentActions_.end(); ++it)
	  (*it)->setEnabled(false);

	emit activeModelChanged(activeDocument_->getActiveModel());
	emit viewChanged();
}

void MainWindow::modelChanged(ldraw::model *m)
{
	EXIT_IF_NO_DOCUMENT;
	
	if (m == activeDocument_->contents()->main_model())
		actionDeleteSubmodel_->setEnabled(false);
	else
		actionDeleteSubmodel_->setEnabled(true);
}

void MainWindow::submodelViewDoubleClicked(const QModelIndex &index)
{
	activeModelChanged(activeDocument_->model()->modelIndexOf(index).first);
}

void MainWindow::selectionChanged(const QSet<int> &cset)
{
	bool selection;
	
	if (cset.count())
		selection = true;
	else
		selection = false;

	for (QList<QAction *>::Iterator it = selectionDependentActions_.begin(); it != selectionDependentActions_.end(); ++it)
		(*it)->setEnabled(selection);
}

void MainWindow::gridModeChanged(QAction *action)
{
	editorGroup_->setGridMode((Editor::GridMode)action->data().toInt());
	action->setChecked(true);
}

void MainWindow::modelModified()
{
	if (activeDocument_) {
		if (!activeDocument_->canSave()) {
			actionSave_->setEnabled(true);
			activeDocument_->setSaveable(true);
			tabbar_->setTabIcon(tabbar_->currentIndex(), KIcon("document-save"));
		}
	}
}

void MainWindow::initObjects()
{
	// model for content lists
	contentsModel_ = new ContentsModel(this);

	// editor group
	editorGroup_ = new Editor(this);
}

void MainWindow::initGui()
{
	QWidget *sc = new QWidget(this);

	// set up docks
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	
	QDockWidget *dockList = new QDockWidget(i18n("Contents"));
	dockList->setObjectName("dockContents");
	dockList->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dockList->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

	QDockWidget *dockSubmodels = new QDockWidget(i18n("Submodels"));
	dockSubmodels->setObjectName("dockSubmodels");
	dockSubmodels->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dockSubmodels->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	QDockWidget *dockParts = new QDockWidget(i18n("Parts"));
	dockParts->setObjectName("dockParts");
	dockParts->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dockParts->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	// content lists
	contentList_ = new ContentsView(dockList);
	contentList_->setModel(contentsModel_);
	dockList->setWidget(contentList_);
	addDockWidget(Qt::TopDockWidgetArea, dockList);

	contentList_->setColumnWidth(0, 32);
	contentList_->setColumnWidth(1, 40);
	contentList_->setColumnWidth(2, 96);
	contentList_->setColumnWidth(3, 150);

	// submodel lists
	submodelList_ = new SubmodelWidget(dockSubmodels);
	dockSubmodels->setWidget(submodelList_);
	addDockWidget(Qt::LeftDockWidgetArea, dockSubmodels);

	// parts widget
	partsWidget_ = new PartsWidget(dockParts);
	dockParts->setWidget(partsWidget_);
	addDockWidget(Qt::LeftDockWidgetArea, dockParts);

	// render widget
	QSplitter *srv = new QSplitter(Qt::Horizontal, sc);
	QSplitter *srh1 = new QSplitter(Qt::Vertical, srv);
	QSplitter *srh2 = new QSplitter(Qt::Vertical, srv);
	
	QGLFormat format = QGLFormat::defaultFormat();
	format.setAlpha(true);
	if (Application::self()->config()->multisampling())
		format.setSampleBuffers(true);

	QGLContext *ctx[4];
	for (int i = 0; i < 4; ++i)
		ctx[i] = new QGLContext(format);
	
	if (!ctx[0]->isValid()) {
		/* fallback if antialiasing is not supported */
		for (int i = 0; i < 4; ++i) {
			delete ctx[i];
			ctx[i] = new QGLContext(QGLFormat::defaultFormat());
		}
	}

	renderWidget_[0] = new RenderWidget(this, &activeDocument_, RenderWidget::Top, ctx[0], 0L, srh1);
	renderWidget_[1] = new RenderWidget(this, &activeDocument_, RenderWidget::Left, ctx[1], renderWidget_[0], srh1);
	renderWidget_[2] = new RenderWidget(this, &activeDocument_, RenderWidget::Front, ctx[2], renderWidget_[0], srh2);
	renderWidget_[3] = new RenderWidget(this, &activeDocument_, RenderWidget::Free, ctx[3], renderWidget_[0], srh2);

	Application::self()->initializeRenderer(renderWidget_[0]);

	tabbar_ = new QTabBar(sc);
	tabbar_->setShape(QTabBar::RoundedSouth);

	// main layout
	QVBoxLayout *layout = new QVBoxLayout(sc);
	layout->addWidget(srv);
	layout->addWidget(tabbar_);
	
	sc->setLayout(layout);
	setCentralWidget(sc);
	
	setDockOptions(QMainWindow::AllowTabbedDocks);
	tabifyDockWidget(dockSubmodels, dockParts);
}

void MainWindow::initActions()
{
	KActionCollection *ac = actionCollection();
	
	// File
	actionNew_ = KStandardAction::openNew(this, SLOT(newFile()), ac);
	actionOpen_ = KStandardAction::open(this, SLOT(openFile()), ac);
	actionOpenRecent_ = KStandardAction::openRecent(this, SLOT(openFile(const KUrl &)), ac);
	actionClose_ = KStandardAction::close(this, SLOT(closeFile()), ac);
	actionSave_ = KStandardAction::save(this, SLOT(saveFile()), ac);
	actionSaveAs_ = KStandardAction::saveAs(this, SLOT(saveFileAs()), ac);
	actionQuit_ = KStandardAction::quit(this, SLOT(quit()), ac);

	actionOpenRecent_->loadEntries(KGlobal::config()->group("Recent Files"));

	actionClose_->setEnabled(false);

	// Edit
	actionUndo_ = editorGroup_->createUndoAction(ac);
	actionRedo_ = editorGroup_->createRedoAction(ac);

	actionCut_ = KStandardAction::cut(editorGroup_, SLOT(cut()), ac);
	actionCopy_ = KStandardAction::copy(editorGroup_, SLOT(copy()), ac);
	actionPaste_ = KStandardAction::paste(editorGroup_, SLOT(paste()), ac);
	actionPaste_->setEnabled(false);

	actionSelectAll_ = createAction("select_all", i18n("Select All"), contentList_, SLOT(selectAll()), "Ctrl+A", "edit-select-all");
	actionSelectNone_ = createAction("select_none", i18n("Select None"), contentList_, SLOT(clearSelection()), "Shift+Ctrl+A", "edit-delete");
	
	actionHide_ = createAction("hide", i18n("Hide"), contentList_, SLOT(hideSelected()), "Ctrl+H");
	actionUnhideAll_ = createAction("unhide_all", i18n("Unhide All"), contentList_, SLOT(unhideAll()), "Shift+Ctrl+H");
	
	actionColor_ = createAction("select_color", i18n("Select Color"), editorGroup_, SLOT(editColor()), "Ctrl+L", "fill-color");
	actionRotationPivot_ = createAction("rotation_pivot", i18n("Rotation Pivot"), editorGroup_, SLOT(rotationPivot()), "Ctrl+Shift+P", "transform-rotate");

	actionGridSparse_ = createAction("grid_sparse", i18n("Thin Grid"), 0L, 0L, "Ctrl+1", "konstruktor-grid1");
	actionGridSparse_->setData(Editor::Grid20);
	actionGridSparse_->setCheckable(true);

	actionGridNormal_ = createAction("grid_normal", i18n("Medium Grid"), 0L, 0L, "Ctrl+2", "konstruktor-grid2");
	actionGridNormal_->setData(Editor::Grid10);
	actionGridNormal_->setCheckable(true);

	actionGridDense_ = createAction("grid_dense", i18n("Dense Grid"), 0L, 0L, "Ctrl+3", "konstruktor-grid3");
	actionGridDense_->setData(Editor::Grid5);
	actionGridDense_->setCheckable(true);

	actionGridNone_ = createAction("grid_none", i18n("Fine Grid"), 0L, 0L, "Ctrl+4", "konstruktor-grid4");
	actionGridNone_->setData(Editor::Grid1);
	actionGridNone_->setCheckable(true);

	QActionGroup *gridActions = new QActionGroup(this);
	gridActions->setExclusive(true);
	gridActions->addAction(actionGridSparse_);
	gridActions->addAction(actionGridNormal_);
	gridActions->addAction(actionGridDense_);
	gridActions->addAction(actionGridNone_);
	connect(gridActions, SIGNAL(triggered(QAction *)), this, SLOT(gridModeChanged(QAction *)));
	actionGridNormal_->setChecked(true);

	actionDelete_ = createAction("delete", i18n("Delete"), editorGroup_, SLOT(deleteSelected()), "Delete", "edit-delete");

	actionMoveByXPositive_ = createAction("move_by_x_positive", i18n("Move -X"), editorGroup_, SLOT(moveByXPositive()), "Right", "konstruktor-move-x-pos");
	actionMoveByXNegative_ = createAction("move_by_x_negative", i18n("Move +X"), editorGroup_, SLOT(moveByXNegative()), "Left", "konstruktor-move-x-neg");
	actionMoveByYPositive_ = createAction("move_by_y_positive", i18n("Move -Y"), editorGroup_, SLOT(moveByYPositive()), "PgDown", "konstruktor-move-y-neg");
	actionMoveByYNegative_ = createAction("move_by_y_negative", i18n("Move +Y"), editorGroup_, SLOT(moveByYNegative()), "PgUp", "konstruktor-move-y-pos");
	actionMoveByZPositive_ = createAction("move_by_z_positive", i18n("Move +Z"), editorGroup_, SLOT(moveByZPositive()), "Up", "konstruktor-move-z-pos");
	actionMoveByZNegative_ = createAction("move_by_z_negative", i18n("Move -Z"), editorGroup_, SLOT(moveByZNegative()), "Down", "konstruktor-move-z-neg");

	actionRotateByXClockwise_ = createAction("rotate_x_cw", i18n("Rotate +X"), editorGroup_, SLOT(rotateByXClockwise()), "Ctrl+Right", "konstruktor-rotate-x-pos");
	actionRotateByXCounterClockwise_ = createAction("rotate_x_ccw", i18n("Rotate -X"), editorGroup_, SLOT(rotateByXCounterClockwise()), "Ctrl+Left", "konstruktor-rotate-x-neg");
	actionRotateByYClockwise_ = createAction("rotate_y_cw", i18n("Rotate +Y"), editorGroup_, SLOT(rotateByYClockwise()), "Ctrl+Shift+Down", "konstruktor-rotate-y-pos");
	actionRotateByYCounterClockwise_ = createAction("rotate_y_ccw", i18n("Rotate -Y"), editorGroup_, SLOT(rotateByYCounterClockwise()), "Ctrl+Shift+Up", "konstruktor-rotate-y-neg");
	actionRotateByZClockwise_ = createAction("rotate_z_cw", i18n("Rotate +Z"), editorGroup_, SLOT(rotateByZClockwise()), "Ctrl+Up", "konstruktor-rotate-z-pos");
	actionRotateByZCounterClockwise_ = createAction("rotate_z_ccw", i18n("Rotate -Z"), editorGroup_, SLOT(rotateByZCounterClockwise()), "Ctrl+Down", "konstruktor-rotate-z-neg");

	// View
	actionResetZoom_ = createAction("reset_zoom", i18n("Reset &Zoom"), this, SLOT(resetZoom()), "R", "view-restore");
	actionReset3dView_ = createAction("reset_3d_view", i18n("Re&set 3D View"), this, SLOT(resetDisplay()));
	
	// Submodel
	actionNewSubmodel_ = createAction("submodel_new", i18n("&New Submodel..."), this, SLOT(newSubmodel()), QString(), "document-new");
	actionDeleteSubmodel_ = createAction("submodel_delete", i18n("&Delete Submodel..."), this, SLOT(deleteSubmodel()), QString(), "edit-delete");
	actionModelProperties_ = createAction("model_properties", i18n("&Model Properties..."), this, SLOT(modelProperties()), QString(), "document-properties");
	
	// Render
	actionRender_ = createAction("render", i18n("R&ender..."), this, SLOT(render()), "Ctrl+F11", "view-preview");
	actionRenderSetup_ = createAction("render_setup", i18n("&Configure Renderer..."), this, SLOT(notImplemented()), QString(), "configure");

	/*actionRenderSteps_ = ac->addAction("render_steps");
	actionRenderSteps_->setText(i18n("Render by &Steps..."));
	actionRenderSteps_->setShortcut("Ctrl+Shift+F11"));
	actionRenderSteps_->setIcon(KIcon("view-preview"));*/

	// Settings
	ac->addAction(KStandardAction::Preferences, this, SLOT(showConfigDialog()));

	stateChangeableActions_.append(actionClose_);
	stateChangeableActions_.append(actionSave_);
	stateChangeableActions_.append(actionSaveAs_);
	stateChangeableActions_.append(actionResetZoom_);
	stateChangeableActions_.append(actionReset3dView_);
	stateChangeableActions_.append(actionNewSubmodel_);
	stateChangeableActions_.append(actionDeleteSubmodel_);
	stateChangeableActions_.append(actionModelProperties_);
	stateChangeableActions_.append(actionUnhideAll_);
	stateChangeableActions_.append(actionRotationPivot_);

	selectionDependentActions_.append(actionCut_);
	selectionDependentActions_.append(actionCopy_);
	selectionDependentActions_.append(actionHide_);
	selectionDependentActions_.append(actionColor_);
	selectionDependentActions_.append(actionDelete_);
	selectionDependentActions_.append(actionMoveByXPositive_);
	selectionDependentActions_.append(actionMoveByXNegative_);
	selectionDependentActions_.append(actionMoveByYPositive_);
	selectionDependentActions_.append(actionMoveByYNegative_);
	selectionDependentActions_.append(actionMoveByZPositive_);
	selectionDependentActions_.append(actionMoveByZNegative_);
	selectionDependentActions_.append(actionRotateByXClockwise_);
	selectionDependentActions_.append(actionRotateByXCounterClockwise_);
	selectionDependentActions_.append(actionRotateByYClockwise_);
	selectionDependentActions_.append(actionRotateByYCounterClockwise_);
	selectionDependentActions_.append(actionRotateByZClockwise_);
	selectionDependentActions_.append(actionRotateByZCounterClockwise_);
}

void MainWindow::initConnections()
{
	connect(tabbar_, SIGNAL(currentChanged(int)), this, SLOT(activeDocumentChanged(int)));
	connect(contentsModel_, SIGNAL(viewChanged()), this, SLOT(updateViewports()));
	connect(contentsModel_, SIGNAL(hide(const QModelIndex &)), contentList_, SLOT(hide(const QModelIndex &)));
	connect(contentsModel_, SIGNAL(unhide(const QModelIndex &)), contentList_, SLOT(unhide(const QModelIndex &)));
	connect(this, SIGNAL(activeModelChanged(ldraw::model *)), contentList_, SLOT(modelChanged(ldraw::model *)));
	connect(this, SIGNAL(activeModelChanged(ldraw::model *)), editorGroup_, SLOT(modelChanged(ldraw::model *)));
	connect(this, SIGNAL(activeModelChanged(ldraw::model *)), submodelList_, SLOT(modelChanged(ldraw::model *)));
	connect(this, SIGNAL(activeModelChanged(ldraw::model *)), this, SLOT(modelChanged(ldraw::model *)));
	connect(this, SIGNAL(viewChanged()), SLOT(updateViewports()));
	connect(submodelList_, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(submodelViewDoubleClicked(const QModelIndex &)));
	connect(contentList_, SIGNAL(selectionChanged(const QSet<int> &)), this, SLOT(selectionChanged(const QSet<int> &)));
	connect(contentList_, SIGNAL(selectionChanged(const QSet<int> &)), editorGroup_, SLOT(selectionChanged(const QSet<int> &)));
	connect(editorGroup_, SIGNAL(selectionIndexModified(const QSet<int> &)), this, SLOT(modelModified(const QSet<int> &)));
	connect(editorGroup_, SIGNAL(needRepaint()), this, SLOT(updateViewports()));
	connect(editorGroup_, SIGNAL(modified()), this, SLOT(modelModified()));
	connect(editorGroup_, SIGNAL(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)), contentsModel_, SLOT(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)));
	connect(editorGroup_, SIGNAL(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)), contentList_, SLOT(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)));
	connect(qApp->clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));

	for (int i = 0; i < 4; ++i) {
		connect(this, SIGNAL(activeModelChanged(ldraw::model *)), renderWidget_[i], SLOT(modelChanged(ldraw::model *)));
		connect(contentList_, SIGNAL(selectionChanged(const QSet<int> &)), renderWidget_[i], SLOT(selectionChanged(const QSet<int> &)));
		connect(renderWidget_[i], SIGNAL(madeSelection(const std::list<int> &, RenderWidget::SelectionMethod)), contentList_, SLOT(updateSelection(const std::list<int> &, RenderWidget::SelectionMethod)));
		connect(renderWidget_[i], SIGNAL(translateObject(const ldraw::vector &)), editorGroup_, SLOT(move(const ldraw::vector &)));
		connect(renderWidget_[i], SIGNAL(objectDropped(const QString &, const ldraw::matrix &, const ldraw::color &)), editorGroup_, SLOT(insert(const QString &, const ldraw::matrix &, const ldraw::color &)));
	}
}

bool MainWindow::confirmQuit()
{
	QVector<QPair<QString, Document *> >::iterator it;
	for (it = documents_.begin(); it != documents_.end(); ++it) {
		if ((*it).second->canSave()) {
			switch (KMessageBox::warningYesNoCancel(this, i18n("The document \"%1\" has been modified. Do you want to save it?", (*it).second->location().toLocalFile()), QString(), KStandardGuiItem::save(), KStandardGuiItem::discard())) {
				case KMessageBox::Yes:
					if (!doSave((*it).second, false))
						return false;
					break;
				case KMessageBox::No:
					break;
				case KMessageBox::Cancel:
					return false;
			}
		}
	}
	
	return true;
}

bool MainWindow::doSave(Document *document, bool newname)
{
	if (!document)
		return false;

	if (document->location().isEmpty())
		newname = true;
	
	QString location;
	QUrl url;
	bool multipart;
	if (newname) {
		location = platformSaveFileDialog(document);

		url = QUrl(location);

		if (location.isEmpty())
			return false;
		
	} else {
		url = document->location();
		location = url.toString();
	}

	multipart = location.endsWith(".mpd", Qt::CaseInsensitive);

	KTemporaryFile fwriter;
	QString tfilename;
	fwriter.setSuffix(".ldr");

	if (newname)
		document->contents()->main_model()->set_name(Utils::urlFileName(url).toLocal8Bit().data());

	if (!fwriter.open()) {
		KMessageBox::error(this, i18n("Could not open temporary file '%1' for writing.", fwriter.fileName()));
		return false;
	}
	tfilename = fwriter.fileName();
	fwriter.write(document->save(multipart));
	fwriter.flush();

	if (!KIO::NetAccess::upload(tfilename, url, this)) {
		KMessageBox::error(this, i18n("Could not write file '%1'.", url.toString()));
		return false;
	}

	document->setSaveable(false);
	if (document == activeDocument_)
		actionSave_->setEnabled(false);

	// Relocate
	if (newname) {
		int newidx = -1;
		QString fname = Utils::urlFileName(url);
		
		if (!document->location().isEmpty())
			openedUrls_.remove(document->location().toString());
		for (int i = 0; i < documents_.size(); ++i) {
			if (documents_[i].second == document) {
				newidx = i;
				break;
			}
		}
		if (newidx != -1) {
			documents_[newidx].first = fname;
			tabbar_->setTabText(newidx, fname);
			tabbar_->setTabIcon(newidx, KIcon("text-plain"));
		}
		openedUrls_.insert(url.toString());
		document->setLocation(QUrl(url));	
	} else {
		int i = 0;
		for (QVector<QPair<QString, Document *> >::ConstIterator it = documents_.constBegin(); it != documents_.constEnd(); ++it) {
			if ((*it).second == document) {
				tabbar_->setTabIcon(i, KIcon("text-plain"));
				break;
			}
			++i;
		}
	}

	changeCaption();

	fwriter.close();

	setStatusMessage(i18n("Document '%1' saved.", Utils::urlFileName(url)));

	return true;
}

void MainWindow::notImplemented()
{
	KMessageBox::sorry(this, i18n("Not implemented yet."));
}

}
