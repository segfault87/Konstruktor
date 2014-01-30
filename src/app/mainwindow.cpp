// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDialog>
#include <QDockWidget>
#include <QFileDialog>
#include <QGLFormat>
#include <QHeaderView>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QStringList>
#include <QTabBar>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QUrl>
#include <QVBoxLayout>

#include <libldr/utils.h>

#include "aboutdialog.h"
#include "actionmanager.h"
#include "application.h"
#include "configdialog.h"
#include "contentsmodel.h"
#include "contentsview.h"
#include "document.h"
#include "editor.h"
#include "menumanager.h"
#include "newmodeldialog.h"
#include "newsubmodeldialog.h"
#include "objectlist.h"
#include "partswidget.h"
#include "povrayexporter.h"
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
    : QMainWindow(parent)
{
  activeDocument_ = 0L;
  newcnt_ = 1;
  
  // set up the main window
  init();
  initGui();
  initActions();
  initMenus();
  initToolBars();
  initConnections();

  Config *cfg = Application::self()->config();
  QByteArray state = cfg->state();
  QByteArray geometry = cfg->geometry();
  if (!state.isEmpty())
    restoreState(state);
  if (!geometry.isEmpty())
    restoreGeometry(geometry);
  
  // to make sure that there is no open models.
  activeDocumentChanged(-1);
  
  emit actionEnabled(false);

  // parse cmd line args
  QStringList args = qApp->arguments();
  for (int i = 1; i < args.size(); ++i) {
    const QString &arg = args[i];
    if (!arg.startsWith("-"))
      openFile(arg);
  }
  
  setStatusMessage(tr("Ready..."));
}

MainWindow::~MainWindow()
{
  // Save recent file entries
  //actionOpenRecent_->saveEntries(KGlobal::config()->group("Recent Files"));
  
  // deselect
  activeDocument_ = 0L;
  activeDocumentChanged(-1);
  
  for (int i = 0; i < documents_.size(); ++i)
    delete documents_[i].second;

  for (int i = 0; i < 4; ++i) {
    delete renderWidget_[i];
    delete glContext_[i];
  }
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
  QAction *paste = actionManager_->query("edit/paste");
  
  if (!mimeData)
    paste->setEnabled(false);
  else if (!mimeData->hasFormat(ObjectList::mimeType))
    paste->setEnabled(false);
  else
    paste->setEnabled(true);
}

void MainWindow::resetColorToolBar()
{
  actionManager_->removeAction("color/*");

  QList<QAction *> colors = editor_->getFavoriteColors();
  foreach (QAction *a, colors) {
    QString key = QString("color/%1").arg(a->data().toInt());
    
    actionManager_->addAction(key, a);
    actionManager_->registerSelectionAction(key);
    colorToolBar_->addAction(a);
    colorActionGroup_->addAction(a);
  }
}

void MainWindow::colorActionTriggered(QAction *action)
{
  int cid = action->data().toInt();

  emit colorSelected(ldraw::color(cid));
}

void MainWindow::rotationPivotActionTriggered(QAction *action)
{
  editor_->setRotationPivotMode((Editor::RotationPivot) action->data().toInt());

  for (int i = 0; i < 4; ++i)
    renderWidget_[i]->anchorChanged();
  
  emit viewChanged();
}

void MainWindow::about()
{
  QDialog *dialog = new AboutDialog(this);
  dialog->exec();
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
    if (activeDocument_->canSave())
      setWindowTitle(tr("%1 [modified] - Konstruktor").arg(main_model->desc().c_str()));
    else
      setWindowTitle(tr("%1 - Konstruktor").arg(main_model->desc().c_str()));
  } else {
    setWindowTitle(tr("Konstruktor"));
  }
}

void MainWindow::activate(bool b)
{
  enabled_ = b;
  contentList_->setEnabled(b);
  submodelList_->setEnabled(b);
  for (int i = 0; i < 4; ++i)
    renderWidget_[i]->setEnabled(b);
  
  actionManager_->query("render/render")->setEnabled(b && Application::self()->hasPovRay());
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
    connect(document, SIGNAL(undoStackAdded(QUndoStack *)), editor_, SLOT(stackAdded(QUndoStack *)));
    connect(document, SIGNAL(undoStackChanged(QUndoStack *)), editor_, SLOT(setActiveStack(QUndoStack *)));
    
    document->sendSignals();
    
    // append into tab bar
    documents_.append(QPair<QString, Document *>(QString(), document));
    int tabidx = tabbar_->addTab(filename);
    tabbar_->setTabIcon(tabidx, Utils::icon("text-plain"));
    tabbar_->setCurrentIndex(tabidx);
    activeDocument_ = document;
    
    ++newcnt_;
    
    setStatusMessage(tr("New document created."));
  }
  
  dialog->close();
  delete dialog;
}

void MainWindow::openFile()
{
  QStringList files = QFileDialog::getOpenFileNames(
      this,
      tr("Choose file(s) to load"),
      QString(),
      tr("LDraw Model Files (*.ldr *.mpd *.dat)"));
  
  foreach (QString it, files)
    openFile(it);
}


void MainWindow::openFile(const QString &path)
{
  if (openedUrls_.contains(path)) {
    for (int i = 0; i < documents_.size(); ++i) {
      if (documents_[i].first == path) {
        tabbar_->setCurrentIndex(i);
        return;
      }
    }
  }
  
  Document *document = 0L;
  try {
    document = new Document(path);
    
    // Initialize connection
    connect(document, SIGNAL(undoStackAdded(QUndoStack *)), editor_, SLOT(stackAdded(QUndoStack *)));
    connect(document, SIGNAL(undoStackChanged(QUndoStack *)), editor_, SLOT(setActiveStack(QUndoStack *)));
    
    document->sendSignals();
    
    // append into document list, tab bar
    openedUrls_.insert(path);
    documents_.append(QPair<QString, Document *>(path, document));
    int tabidx = tabbar_->addTab(path);
    tabbar_->setTabIcon(tabidx, Utils::icon("text-plain"));
    tabbar_->setCurrentIndex(tabidx);
    
    activeDocument_ = document;
    
    //actionOpenRecent_->addUrl(aurl);
  } catch (const ldraw::exception &e) {
    if (document)
      delete document;
    
    QMessageBox::critical(this, tr("Error"), tr("Could not open a file: %1").arg(e.details().c_str()));
  }
  
  setStatusMessage(tr("Document '%1' opened.").arg(path));
}

void MainWindow::closeFile()
{
  EXIT_IF_NO_DOCUMENT;
  
  if (activeDocument_->canSave()) {
    switch (QMessageBox::question(this, tr("Confirm"), tr("The document \"%1\" has been modified. Do you want to save it?").arg(Utils::urlFileName(activeDocument_->path())), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel)) {
      case QMessageBox::Yes:
        if (!doSave(activeDocument_, false))
          return;
        break;
      case QMessageBox::No:
        break;
      case QMessageBox::Cancel:
      default:
        return;
    }
  }
  
  Document *t = activeDocument_;
  int ci = tabbar_->currentIndex();
  openedUrls_.remove(t->path());
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
      QMessageBox::critical(dialog, tr("Error"), tr("The name '%1' is already in use. Try using another one.").arg(dialog->textName()));
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
    QMessageBox::critical(this, tr("Error"), tr("This submodel is included in somewhere else."));
    return;
  }
  
  if (QMessageBox::question(this, tr("Confirm"), tr("This operation cannot be undone. Would you like to proceed?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
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
  
  if (confirmQuit())
    qApp->quit();
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

void MainWindow::renderExport()
{
  if (!activeDocument_)
    return;

  QString path = QFileDialog::getSaveFileName(this, tr("Export Model"), QString(), tr("POV-Ray Scene Files (*.pov)"));

  if (!path.isEmpty()) {
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
      QMessageBox::critical(this, tr("Error"), tr("Could not open \"%1\" for writing.").arg(path));
      return;
    }

    POVRayRenderParameters param;
    POVRayExporter exporter(activeDocument_->getActiveModel(), &param);
    exporter.start();
    
    QTextStream stream(&file);
    stream << exporter.output();

    file.close();
  }
}

void MainWindow::showConfigDialog()
{
  /*ConfigDialog dialog(this);
  dialog.exec();

  if (dialog.result() == QDialog::Accepted)
  Application::self()->config()->writeConfig();*/

  notImplemented();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
  if (confirmQuit()) {
    /* save window state */
    Config *cfg = Application::self()->config();
    cfg->setState(saveState());
    cfg->setGeometry(saveGeometry());
    cfg->writeConfig();
    
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::activeDocumentChanged(int index)
{
  if (!enabled_)
    emit actionEnabled(true);
  else if (index == -1)
    emit actionEnabled(false);

  QAction *save = actionManager_->query("file/save");
  
  if (index >= 0) {
    activeDocument_ = documents_[index].second;
    
    if (activeDocument_->canSave())
      save->setEnabled(true);
    else
      save->setEnabled(false);
    
    editor_->setActiveStack(activeDocument_->activeUndoStack());
  } else {
    activeDocument_ = 0L;
    
    editor_->setActiveStack(0L);
  }
  
  // reset the content list
  contentsModel_->setDocument(activeDocument_);
  contentList_->scrollToBottom();
  
  // reset the submodel list
  if (activeDocument_)
    submodelList_->setModel(activeDocument_->model());
  
  changeCaption();
  
  actionManager_->setSelectionState(false);
  
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
  
  actionManager_->setSelectionState(false);
  
  emit activeModelChanged(activeDocument_->getActiveModel());
  emit viewChanged();
}

void MainWindow::modelChanged(ldraw::model *m)
{
  EXIT_IF_NO_DOCUMENT;

  QAction *action = actionManager_->query("submodel/delete");
  
  if (m == activeDocument_->contents()->main_model())
    action->setEnabled(false);
  else
    action->setEnabled(true);
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
  
  actionManager_->setSelectionState(selection);
}

void MainWindow::gridModeChanged(QAction *action)
{
  editor_->setGridMode((Editor::GridMode)action->data().toInt());
  action->setChecked(true);
}

void MainWindow::modelModified()
{
  if (activeDocument_) {
    if (!activeDocument_->canSave()) {
      actionManager_->query("file/save")->setEnabled(true);
      activeDocument_->setSaveable(true);
      tabbar_->setTabIcon(tabbar_->currentIndex(), Utils::icon("document-save"));
      changeCaption();
    }
  }
}

void MainWindow::init()
{
  // model for content lists
  contentsModel_ = new ContentsModel(this);
  
  // editor group
  editor_ = new Editor(this);

  /* basic gui setup */
  actionManager_ = new ActionManager(this);
  menuManager_ = new MenuManager(this);
}

void MainWindow::initGui()
{
  QWidget *sc = new QWidget(this);
  
  // set up docks
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  
  QDockWidget *dockList = new QDockWidget(tr("Contents"));
  dockList->setObjectName("dockContents");
  dockList->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dockList->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  
  QDockWidget *dockSubmodels = new QDockWidget(tr("Submodels"));
  dockSubmodels->setObjectName("dockSubmodels");
  dockSubmodels->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dockSubmodels->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  
  QDockWidget *dockParts = new QDockWidget(tr("Parts"));
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
  
  Config *cfg = Application::self()->config();
  
  // OpenGL context
  QGLFormat format = QGLFormat::defaultFormat();
  format.setAlpha(true);
  if (cfg->multisampling())
    format.setSampleBuffers(true);
  
  for (int i = 0; i < 4; ++i)
    glContext_[i] = new QGLContext(format);
  
  if (!glContext_[0]->isValid()) {
    /* fallback if antialiasing is not supported */
    for (int i = 0; i < 4; ++i) {
      delete glContext_[i];
      glContext_[i] = new QGLContext(QGLFormat::defaultFormat());
    }
  }
  
  renderWidget_[0] = new RenderWidget(this, &activeDocument_, RenderWidget::getViewportMode((int) cfg->viewportTopLeft()), glContext_[0], 0L, srh1);
  renderWidget_[1] = new RenderWidget(this, &activeDocument_, RenderWidget::getViewportMode((int) cfg->viewportBottomLeft()), glContext_[1], renderWidget_[0], srh1);
  renderWidget_[2] = new RenderWidget(this, &activeDocument_, RenderWidget::getViewportMode((int) cfg->viewportTopRight()), glContext_[2], renderWidget_[0], srh2);
  renderWidget_[3] = new RenderWidget(this, &activeDocument_, RenderWidget::getViewportMode((int) cfg->viewportBottomRight()), glContext_[3], renderWidget_[0], srh2);
  
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
  QAction *ac;

  // File
  actionManager_->createAction("file/new", tr("&New..."), this, SLOT(newFile()), QKeySequence::New, Utils::icon("document-new"));
  actionManager_->createAction("file/open", tr("&Open..."), this, SLOT(openFile()), QKeySequence::Open, Utils::icon("document-open"));
  actionManager_->createAction("file/close", tr("&Close"), this, SLOT(closeFile()), QKeySequence::Close, Utils::icon("document-close"));
  actionManager_->createAction("file/save", tr("&Save"), this, SLOT(saveFile()), QKeySequence::Save, Utils::icon("document-save"));
  actionManager_->createAction("file/save_as", tr("Save &As"), this, SLOT(saveFileAs()), QKeySequence::SaveAs, Utils::icon("document-save-as"));
  actionManager_->createAction("file/quit", tr("&Quit"), this, SLOT(quit()), QKeySequence::Quit, Utils::icon("application-exit"));

  actionManager_->query("file/close")->setEnabled(false);
  
  //actionOpenRecent_->loadEntries(KGlobal::config()->group("Recent Files"));
  
  // Edit
  actionManager_->addAction("edit/undo", editor_->createUndoAction());
  actionManager_->addAction("edit/redo", editor_->createRedoAction());
  actionManager_->createAction("edit/cut", tr("Cu&t"), editor_, SLOT(cut()), QKeySequence::Cut, Utils::icon("edit-cut"));
  actionManager_->createAction("edit/copy", tr("&Copy"), editor_, SLOT(copy()), QKeySequence::Copy, Utils::icon("edit-copy"));
  actionManager_->createAction("edit/paste", tr("&Paste"), editor_, SLOT(paste()), QKeySequence::Paste, Utils::icon("edit-paste"));
  
  actionManager_->query("edit/paste")->setEnabled(false);
  
  actionManager_->createAction("edit/select_all", tr("&Select All"), contentList_, SLOT(selectAll()), QKeySequence::SelectAll, Utils::icon("edit-select-all"));
  actionManager_->createAction("edit/select_none", tr("Select &None"), contentList_, SLOT(clearSelection()), QKeySequence("Ctrl+Shift+A"), Utils::icon("edit-delete"));
  actionManager_->createAction("edit/hide", tr("&Hide"), contentList_, SLOT(hideSelected()), QKeySequence("Ctrl+H"));
  actionManager_->createAction("edit/unhide_all", tr("Unhide &All"), contentList_, SLOT(unhideAll()), QKeySequence("Ctrl+Shift+H"));
  actionManager_->createAction("edit/select_color", tr("Select Color"), editor_, SLOT(editColor()), QKeySequence("Ctrl+L"), Utils::icon("fill-color"));

  rotationPivotActionGroup_ = new QActionGroup(this);
  rotationPivotActionGroup_->setExclusive(true);
  ac = actionManager_->createAction("edit/rotation_pivot_each", tr("Each Parts"), 0L, 0L);
  ac->setData((int) Editor::PivotEach);
  ac->setCheckable(true);
  ac->setChecked(true);
  rotationPivotActionGroup_->addAction(ac);
  ac = actionManager_->createAction("edit/rotation_pivot_center", tr("Center"), 0L, 0L);
  ac->setData((int) Editor::PivotCenter);
  ac->setCheckable(true);
  rotationPivotActionGroup_->addAction(ac);
  ac = actionManager_->createAction("edit/rotation_pivot_manual", tr("Manual (if exists)"), 0L, 0L);
  ac->setData((int) Editor::PivotManual);
  ac->setCheckable(true);
  ac->setEnabled(false);
  rotationPivotActionGroup_->addAction(ac);  

  rotationPivotMenu_ = new QMenu(tr("Rotation Pivot"));
  rotationPivotMenu_->setIcon(Utils::icon("transform-rotate"));
  rotationPivotMenu_->addAction(actionManager_->query("edit/rotation_pivot_each"));
  rotationPivotMenu_->addAction(actionManager_->query("edit/rotation_pivot_center"));
  rotationPivotMenu_->addAction(actionManager_->query("edit/rotation_pivot_manual"));

  ac = actionManager_->createAction("edit/grid_1", tr("Grid Level 1"), 0L, 0L, QKeySequence("Ctrl+1"), QIcon(":/icons/grid1.png"));
  ac->setData(Editor::Grid20);
  ac->setCheckable(true);
  ac = actionManager_->createAction("edit/grid_2", tr("Grid Level 2"), 0L, 0L, QKeySequence("Ctrl+2"), QIcon(":/icons/grid2.png"));
  ac->setData(Editor::Grid10);
  ac->setCheckable(true);
  ac = actionManager_->createAction("edit/grid_3", tr("Grid Level 3"), 0L, 0L, QKeySequence("Ctrl+3"), QIcon(":/icons/grid3.png"));
  ac->setData(Editor::Grid5);
  ac->setCheckable(true);
  ac = actionManager_->createAction("edit/grid_4", tr("Grid Level 4"), 0L, 0L, QKeySequence("Ctrl+4"), QIcon(":/icons/grid4.png"));
  ac->setData(Editor::Grid1);
  ac->setCheckable(true);
  
  QActionGroup *gridActions = new QActionGroup(actionManager_);
  gridActions->setExclusive(true);
  gridActions->addAction(actionManager_->query("edit/grid_1"));
  gridActions->addAction(actionManager_->query("edit/grid_2"));
  gridActions->addAction(actionManager_->query("edit/grid_3"));
  gridActions->addAction(actionManager_->query("edit/grid_4"));
  connect(gridActions, SIGNAL(triggered(QAction *)), this, SLOT(gridModeChanged(QAction *)));
  actionManager_->query("edit/grid_2")->setChecked(true);
  
  actionManager_->createAction("edit/delete", tr("&Delete"), editor_, SLOT(deleteSelected()), QKeySequence::Delete, Utils::icon("edit-delete"));

  actionManager_->createAction("edit/move_x_pos", tr("Move -X"), editor_, SLOT(moveByXPositive()), QKeySequence("Right"), QIcon(":/icons/move-x-pos.png"));
  actionManager_->createAction("edit/move_x_neg", tr("Move +X"), editor_, SLOT(moveByXNegative()), QKeySequence("Left"), QIcon(":/icons/move-x-neg.png"));
  actionManager_->createAction("edit/move_y_pos", tr("Move -Y"), editor_, SLOT(moveByYPositive()), QKeySequence("End"), QIcon(":/icons/move-y-pos.png"));
  actionManager_->createAction("edit/move_y_neg", tr("Move +Y"), editor_, SLOT(moveByYNegative()), QKeySequence("Home"), QIcon(":/icons/move-y-neg.png"));
  actionManager_->createAction("edit/move_z_pos", tr("Move -Z"), editor_, SLOT(moveByZPositive()), QKeySequence("Up"), QIcon(":/icons/move-z-pos.png"));
  actionManager_->createAction("edit/move_z_neg", tr("Move +Z"), editor_, SLOT(moveByZNegative()), QKeySequence("Down"), QIcon(":/icons/move-z-neg.png"));
  
  actionManager_->createAction("edit/rotate_x_cw", tr("Rotate +X"), editor_, SLOT(rotateByXClockwise()), QKeySequence("Ctrl+Up"), QIcon(":/icons/rotate-x-pos.png"));
  actionManager_->createAction("edit/rotate_x_ccw", tr("Rotate -X"), editor_, SLOT(rotateByXCounterClockwise()), QKeySequence("Ctrl+Down"), QIcon(":/icons/rotate-x-neg.png"));
  actionManager_->createAction("edit/rotate_y_cw", tr("Rotate +Y"), editor_, SLOT(rotateByYClockwise()), QKeySequence("Ctrl+Right"), QIcon(":/icons/rotate-y-pos.png"));
  actionManager_->createAction("edit/rotate_y_ccw", tr("Rotate -Y"), editor_, SLOT(rotateByYCounterClockwise()), QKeySequence("Ctrl+Left"), QIcon(":/icons/rotate-y-neg.png"));
  actionManager_->createAction("edit/rotate_z_cw", tr("Rotate +Z"), editor_, SLOT(rotateByZClockwise()), QKeySequence("Ctrl+Shift+Right"), QIcon(":/icons/rotate-z-pos.png"));
  actionManager_->createAction("edit/rotate_z_ccw", tr("Rotate -Z"), editor_, SLOT(rotateByZCounterClockwise()), QKeySequence("Ctrl+Shift+Left"), QIcon(":/icons/rotate-z-neg.png"));

  // View
  actionManager_->createAction("view/reset_zoom", tr("Reset &Zoom"), this, SLOT(resetZoom()));
  actionManager_->createAction("view/reset_3d_view", tr("Re&set 3D View"), this, SLOT(resetDisplay()));
  
  // Submodel
  actionManager_->createAction("submodel/new", tr("&New Submodel..."), this, SLOT(newSubmodel()), QKeySequence(), Utils::icon("document-new"));
  actionManager_->createAction("submodel/delete", tr("&Delete Submodel"), this, SLOT(deleteSubmodel()), QKeySequence(), Utils::icon("edit-delete"));
  actionManager_->createAction("submodel/edit", tr("&Model Properties..."), this, SLOT(modelProperties()), QKeySequence(), Utils::icon("document-properties"));
  
  // Render
  actionManager_->createAction("render/render", tr("R&ender..."), this, SLOT(render()), QKeySequence("Ctrl+F11"), Utils::icon("view-preview"));
  actionManager_->createAction("render/export", tr("E&xport..."), this, SLOT(renderExport()), QKeySequence(), Utils::icon("document-export"));
  actionManager_->createAction("render/setup", tr("&Configure Renderer..."), this, SLOT(notImplemented()), QKeySequence(), Utils::icon("configure"));
  
  /*actionRenderSteps_ = ac->addAction("render_steps");
    actionRenderSteps_->setText(tr("Render by &Steps..."));
    actionRenderSteps_->setShortcut("Ctrl+Shift+F11"));
    actionRenderSteps_->setIcon(Utils::icon("view-preview"));*/
  
  // Settings
  actionManager_->createAction("settings/configure", tr("&Configure..."), this, SLOT(showConfigDialog()), QKeySequence::Preferences, Utils::icon("configure"));

  // Help
  actionManager_->createAction("help/about", tr("&About Konstruktor..."), this, SLOT(about()));
  actionManager_->createAction("help/about_qt", tr("A&bout Qt..."), qApp, SLOT(aboutQt()));

  actionManager_->registerDocumentAction("file/close");
  actionManager_->registerDocumentAction("file/save");
  actionManager_->registerDocumentAction("file/save_as");
  actionManager_->registerDocumentAction("file/reset_zoom");
  actionManager_->registerDocumentAction("file/reset_3d_view");
  actionManager_->registerDocumentAction("submodel/new");
  actionManager_->registerDocumentAction("submodel/delete");
  actionManager_->registerDocumentAction("submodel/edit");
  actionManager_->registerDocumentAction("edit/unhide_all");
  actionManager_->registerDocumentAction("edit/rotation_pivot");
  actionManager_->registerDocumentAction("render/export");

  actionManager_->registerSelectionAction("edit/cut");
  actionManager_->registerSelectionAction("edit/copy");
  actionManager_->registerSelectionAction("edit/hide");
  actionManager_->registerSelectionAction("edit/select_color");
  actionManager_->registerSelectionAction("edit/delete");
  actionManager_->registerSelectionAction("edit/move_x_pos");
  actionManager_->registerSelectionAction("edit/move_x_neg");
  actionManager_->registerSelectionAction("edit/move_y_pos");
  actionManager_->registerSelectionAction("edit/move_y_neg");
  actionManager_->registerSelectionAction("edit/move_z_pos");
  actionManager_->registerSelectionAction("edit/move_z_neg");
  actionManager_->registerSelectionAction("edit/rotate_x_cw");
  actionManager_->registerSelectionAction("edit/rotate_x_ccw");
  actionManager_->registerSelectionAction("edit/rotate_y_cw");
  actionManager_->registerSelectionAction("edit/rotate_y_ccw");
  actionManager_->registerSelectionAction("edit/rotate_z_cw");
  actionManager_->registerSelectionAction("edit/rotate_z_ccw");
}

void MainWindow::initConnections()
{
  connect(this, SIGNAL(actionEnabled(bool)), this, SLOT(activate(bool)));
  connect(this, SIGNAL(actionEnabled(bool)), actionManager_, SLOT(setModelState(bool)));
  connect(tabbar_, SIGNAL(currentChanged(int)), this, SLOT(activeDocumentChanged(int)));
  connect(contentsModel_, SIGNAL(viewChanged()), this, SLOT(updateViewports()));
  connect(contentsModel_, SIGNAL(hide(const QModelIndex &)), contentList_, SLOT(hide(const QModelIndex &)));
  connect(contentsModel_, SIGNAL(unhide(const QModelIndex &)), contentList_, SLOT(unhide(const QModelIndex &)));
  connect(this, SIGNAL(activeModelChanged(ldraw::model *)), contentList_, SLOT(modelChanged(ldraw::model *)));
  connect(this, SIGNAL(activeModelChanged(ldraw::model *)), editor_, SLOT(modelChanged(ldraw::model *)));
  connect(this, SIGNAL(activeModelChanged(ldraw::model *)), submodelList_, SLOT(modelChanged(ldraw::model *)));
  connect(this, SIGNAL(activeModelChanged(ldraw::model *)), this, SLOT(modelChanged(ldraw::model *)));
  connect(this, SIGNAL(viewChanged()), SLOT(updateViewports()));
  connect(submodelList_, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(submodelViewDoubleClicked(const QModelIndex &)));
  connect(contentList_, SIGNAL(selectionChanged(const QSet<int> &)), this, SLOT(selectionChanged(const QSet<int> &)));
  connect(contentList_, SIGNAL(selectionChanged(const QSet<int> &)), editor_, SLOT(selectionChanged(const QSet<int> &)));
  connect(editor_, SIGNAL(selectionIndexModified(const QSet<int> &)), this, SLOT(modelModified(const QSet<int> &)));
  connect(editor_, SIGNAL(needRepaint()), this, SLOT(updateViewports()));
  connect(editor_, SIGNAL(modified()), this, SLOT(modelModified()));
  connect(editor_, SIGNAL(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)), contentsModel_, SLOT(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)));
  connect(editor_, SIGNAL(colorListChanged()), this, SLOT(resetColorToolBar()));
  connect(editor_, SIGNAL(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)), contentList_, SLOT(rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &)));
  connect(colorActionGroup_, SIGNAL(triggered(QAction *)), this, SLOT(colorActionTriggered(QAction *)));
  connect(rotationPivotActionGroup_, SIGNAL(triggered(QAction *)), this, SLOT(rotationPivotActionTriggered(QAction *)));
  connect(this, SIGNAL(colorSelected(const ldraw::color &)), editor_, SLOT(setColor(const ldraw::color &)));
  connect(qApp->clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
  
  for (int i = 0; i < 4; ++i) {
    connect(editor_, SIGNAL(modified()), renderWidget_[i], SLOT(anchorChanged()));
    connect(this, SIGNAL(activeModelChanged(ldraw::model *)), renderWidget_[i], SLOT(modelChanged(ldraw::model *)));
    connect(contentList_, SIGNAL(selectionChanged(const QSet<int> &)), renderWidget_[i], SLOT(selectionChanged(const QSet<int> &)));
    connect(renderWidget_[i], SIGNAL(madeSelection(const std::list<int> &, RenderWidget::SelectionMethod)), contentList_, SLOT(updateSelection(const std::list<int> &, RenderWidget::SelectionMethod)));
    connect(renderWidget_[i], SIGNAL(translateObject(const ldraw::matrix &)), editor_, SLOT(translate(const ldraw::matrix &)));
    connect(renderWidget_[i], SIGNAL(objectDropped(const QString &, const ldraw::matrix &, const ldraw::color &)), editor_, SLOT(insert(const QString &, const ldraw::matrix &, const ldraw::color &)));
  }
}

void MainWindow::initMenus()
{
  QMenu *menuFile = menuBar()->addMenu(tr("&File"));
  menuFile->addAction(actionManager_->query("file/new"));
  menuFile->addAction(actionManager_->query("file/open"));
  menuFile->addSeparator();
  menuFile->addAction(actionManager_->query("file/save"));
  menuFile->addAction(actionManager_->query("file/save_as"));
  menuFile->addSeparator();
  menuFile->addAction(actionManager_->query("file/close"));
  menuFile->addSeparator();
  menuFile->addAction(actionManager_->query("file/quit"));

  QMenu *menuEdit = menuBar()->addMenu(tr("&Edit"));
  menuEdit->addAction(actionManager_->query("edit/undo"));
  menuEdit->addAction(actionManager_->query("edit/redo"));
  menuEdit->addSeparator();
  menuEdit->addAction(actionManager_->query("edit/cut"));
  menuEdit->addAction(actionManager_->query("edit/copy"));
  menuEdit->addAction(actionManager_->query("edit/paste"));
  menuEdit->addSeparator();
  menuEdit->addAction(actionManager_->query("edit/delete"));
  menuEdit->addSeparator();
  menuEdit->addAction(actionManager_->query("edit/select_all"));
  menuEdit->addAction(actionManager_->query("edit/select_none"));
  menuEdit->addAction(actionManager_->query("edit/hide"));
  menuEdit->addAction(actionManager_->query("edit/unhide_all"));
  menuEdit->addSeparator();
  menuEdit->addMenu(rotationPivotMenu_);
  
  QMenu *menuView = menuBar()->addMenu(tr("&View"));
  menuView->addAction(actionManager_->query("view/reset_zoom"));
  menuView->addAction(actionManager_->query("view/reset_3d_view"));

  QMenu *menuSubmodel = menuBar()->addMenu(tr("S&ubmodel"));
  menuSubmodel->addAction(actionManager_->query("submodel/new"));
  menuSubmodel->addAction(actionManager_->query("submodel/delete"));
  menuSubmodel->addAction(actionManager_->query("submodel/edit"));

  QMenu *menuRender = menuBar()->addMenu(tr("&Render"));
  menuRender->addAction(actionManager_->query("render/render"));
  menuRender->addAction(actionManager_->query("render/export"));
  menuRender->addAction(actionManager_->query("render/setup"));

  QMenu *menuSettings = menuBar()->addMenu(tr("&Settings"));
  menuSettings->addAction(actionManager_->query("settings/configure"));

  QMenu *menuHelp = menuBar()->addMenu(tr("&Help"));
  menuHelp->addAction(actionManager_->query("help/about"));
  menuHelp->addAction(actionManager_->query("help/about_qt"));
}

void MainWindow::initToolBars()
{
  QToolBar *toolBarFile = addToolBar(tr("File"));
  toolBarFile->setObjectName("toolbar_file");
  toolBarFile->addAction(actionManager_->query("file/new"));
  toolBarFile->addAction(actionManager_->query("file/open"));
  toolBarFile->addAction(actionManager_->query("file/save"));
  
  QToolBar *toolBarEdit = addToolBar(tr("Edit"));
  toolBarEdit->setObjectName("toolbar_edit");
  toolBarEdit->addAction(actionManager_->query("edit/undo"));
  toolBarEdit->addAction(actionManager_->query("edit/redo"));
  toolBarEdit->addSeparator();
  toolBarEdit->addAction(actionManager_->query("edit/cut"));
  toolBarEdit->addAction(actionManager_->query("edit/copy"));
  toolBarEdit->addAction(actionManager_->query("edit/paste"));
  toolBarEdit->addSeparator();
  toolBarEdit->addAction(actionManager_->query("edit/select_color"));
  
  QToolButton *toolButton = new QToolButton();
  toolButton->setMenu(rotationPivotMenu_);
  toolButton->setPopupMode(QToolButton::InstantPopup);
  toolButton->setIcon(Utils::icon("transform-rotate"));
  toolBarEdit->addWidget(toolButton);

  toolBarEdit->addSeparator();
  toolBarEdit->addAction(actionManager_->query("edit/grid_1"));
  toolBarEdit->addAction(actionManager_->query("edit/grid_2"));
  toolBarEdit->addAction(actionManager_->query("edit/grid_3"));
  toolBarEdit->addAction(actionManager_->query("edit/grid_4"));
  toolBarEdit->addSeparator();
  toolBarEdit->addAction(actionManager_->query("edit/move_x_pos"));
  toolBarEdit->addAction(actionManager_->query("edit/move_x_neg"));
  toolBarEdit->addAction(actionManager_->query("edit/move_y_pos"));
  toolBarEdit->addAction(actionManager_->query("edit/move_y_neg"));
  toolBarEdit->addAction(actionManager_->query("edit/move_z_pos"));
  toolBarEdit->addAction(actionManager_->query("edit/move_z_neg"));
  toolBarEdit->addSeparator();
  toolBarEdit->addAction(actionManager_->query("edit/rotate_x_cw"));
  toolBarEdit->addAction(actionManager_->query("edit/rotate_x_ccw"));
  toolBarEdit->addAction(actionManager_->query("edit/rotate_y_cw"));
  toolBarEdit->addAction(actionManager_->query("edit/rotate_y_ccw"));
  toolBarEdit->addAction(actionManager_->query("edit/rotate_z_cw"));
  toolBarEdit->addAction(actionManager_->query("edit/rotate_z_ccw"));

  colorToolBar_ = new QToolBar(tr("Colors"), this);
  colorToolBar_->setObjectName("toolbar_colors");
  colorToolBar_->setIconSize(QSize(16, 16));
  addToolBar(Qt::LeftToolBarArea, colorToolBar_);
  colorActionGroup_ = new QActionGroup(this);
  resetColorToolBar();

  QToolBar *toolBarView = addToolBar(tr("View"));
  toolBarView->setObjectName("toolbar_view");
  toolBarView->addAction(actionManager_->query("view/reset_zoom"));
  toolBarView->addAction(actionManager_->query("view/reset_3d_view"));
}

bool MainWindow::confirmQuit()
{
  QVector<QPair<QString, Document *> >::iterator it;
  for (it = documents_.begin(); it != documents_.end(); ++it) {
    if ((*it).second->canSave()) {
      switch (QMessageBox::question(this, tr("Confirm"), tr("The document \"%1\" has been modified. Do you want to save it?").arg((*it).second->path()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel)) {
        case QMessageBox::Yes:
          if (!doSave((*it).second, false))
            return false;
          break;
        case QMessageBox::No:
          break;
        case QMessageBox::Cancel:
        default:
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
  
  if (document->path().isEmpty())
    newname = true;
  
  QString location;
  bool multipart;
  if (newname) {
    location = QFileDialog::getSaveFileName(this,
                                            tr("Save as"),
                                            QString(),
                                            tr("LDraw Model Files (*.ldr *.mpd *.dat)"));
    if (location.isEmpty())
      return false;
    
  } else {
    location = document->path();
  }
  
  multipart = location.endsWith(".mpd", Qt::CaseInsensitive);
  
  if (newname)
    document->contents()->main_model()->set_name(location.toLocal8Bit().data());

  QFile f(location);
  if (!f.open(QFile::WriteOnly)) {
    QMessageBox::critical(this, tr("Error"), tr("Could not open temporary file '%1' for writing.").arg(location));
    return false;
  }
  QTextStream outstream(&f);
  outstream << document->save(multipart);
  outstream.flush();
  f.close();
  
  document->setSaveable(false);
  if (document == activeDocument_)
    actionManager_->query("file/save")->setEnabled(false);
  
  // Relocate
  if (newname) {
    int newidx = -1;
    QString fname = location;
    
    if (!document->path().isEmpty())
      openedUrls_.remove(document->path());
    for (int i = 0; i < documents_.size(); ++i) {
      if (documents_[i].second == document) {
        newidx = i;
        break;
      }
    }
    if (newidx != -1) {
      documents_[newidx].first = fname;
      tabbar_->setTabText(newidx, fname);
      tabbar_->setTabIcon(newidx, Utils::icon("text-plain"));
    }
    openedUrls_.insert(location);
    document->setPath(location);	
  } else {
    int i = 0;
    for (QVector<QPair<QString, Document *> >::ConstIterator it = documents_.constBegin(); it != documents_.constEnd(); ++it) {
      if ((*it).second == document) {
        tabbar_->setTabIcon(i, Utils::icon("text-plain"));
        break;
      }
      ++i;
    }
  }
  
  changeCaption();
  
  setStatusMessage(tr("Document '%1' saved.").arg(location));
  
  return true;
}

void MainWindow::notImplemented()
{
  QMessageBox::critical(this, tr("Sorry"), tr("Not implemented yet."));
}

}
