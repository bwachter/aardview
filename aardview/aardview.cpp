#include <QtGui>
#include <QMessageBox>
#include <QScrollBar>
#include <QFileDialog>

#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#endif

#include "aardview.h"
#include "settingsdialog.h"

AardView::AardView(QUuid uid){
  setupUi(this);

  m_uid = uid;
  this->setWindowIcon(QPixmap(":/images/aardview-icon.png"));

#ifdef QT_NO_PRINTER
  // disable printer items if qt comes without printing support
  actionPrint->setEnabled(false);
  actionPrint->setVisible(false);
  actionPrintPreview->setEnabled(false);
  actionPrintPreview->setVisible(false);
#endif

  loader = new ImageLoader();
  centralwidget->installEventFilter(this);

  // hide currently unused docks
  dockTaggedItems->hide();
  dockStatusInfo->hide();

  dirViewModel = new ADirModel();
  tnViewModel = new TnViewModel();
  dirViewModelProxy = new QSortFilterProxyModel();
  tnViewModelProxy = new QSortFilterProxyModel();

  // add toggle actions for docks
  dockDirectoryTree->toggleViewAction()->setShortcut(QKeySequence(tr("Ctrl+D")));
  dockTreeView->toggleViewAction()->setShortcut(QKeySequence(tr("Ctrl+T")));

  menuView->addAction(dockDirectoryTree->toggleViewAction());
  menuView->addAction(dockTreeView->toggleViewAction());
  menuView->addAction(dockTaggedItems->toggleViewAction());

  // set the model
  dirViewModelProxy->setSourceModel(dirViewModel);
  dirView->setModel(dirViewModelProxy);
  dirView->setRootIndex(dirViewModelProxy->mapFromSource(
                          dirViewModel->index(QDir::rootPath())));

  tnViewModelProxy->setSourceModel(tnViewModel);
  tnView->setModel(tnViewModelProxy);

  // finally connect everything we didn't connect by designer already
  connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
  connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  connect(actionAbout, SIGNAL(triggered()), this, SIGNAL(showAbout()));
  connect(actionPreferences, SIGNAL(triggered()), this, SIGNAL(showSettings()));

  connect(dirView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(dirIndexChanged()));
  connect(tnView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(thumbIndexChanged()));

  connect(actionToggleFitToWindow, SIGNAL(triggered()), loader, SLOT(toggleFtw()));
  //connect(actionRotate, SIGNAL(triggered()), loader, SLOT(rotate()));
  connect(actionZoomIn, SIGNAL(triggered()), loader, SLOT(zoomIn()));
  connect(actionZoomOut, SIGNAL(triggered()), loader, SLOT(zoomOut()));
  connect(this, SIGNAL(requestPixmap(const QString &, const QSize &)),
          loader, SLOT(load(const QString &, const QSize &)));
  connect(loader, SIGNAL(pixmapReady(const QPixmap &)), this, SLOT(displayPixmap(const QPixmap &)));

  // TODO: this should be uid pecidic state restore
  /*
  this->resize(settings->value("main/initialX").toInt(),
               settings->value("main/initialY").toInt());
  */
  reconfigure();

  qDebug() << "Current size: " << centralwidget->size();
  qDebug() << "Linked in plugins: " << QPluginLoader::staticInstances();

  grabGesture(Qt::TapAndHoldGesture);
  grabGesture(Qt::PanGesture);
  grabGesture(Qt::SwipeGesture);

  // using a timer we make sure this get's called once the UI
  // is already set up, avoiding annoying resize problems
  QTimer::singleShot(0, this, SLOT(handleArguments()));
}

AardView::~AardView(){
  // TODO: this should become uid-specific state saving
  /*
  if (settings->value("main/saveSizeOnExit").toBool()){
    qDebug() << "Saving current window size" << width();
    settings->setValue("main/initialX", width());
    settings->setValue("main/initialY", height());
  }
  */
}

// TODO: move argument handling to shim
void AardView::handleArguments(){
  SettingsDialog *settings = SettingsDialog::instance();
  QString initialPath=QDir::currentPath();
  QStringList arguments=qApp->arguments();
  if (arguments.count() == 2){
    QDir argDir=QDir(arguments.at(1));
    if (argDir.exists())
      // argument is a dir, jump to this directory
      initialPath=arguments.at(1);
    else {
      // argument is a file, display the file
      if (QFile::exists(arguments.at(1))){
        dockDirectoryTree->hide();
        dockTreeView->hide();
        loadPixmap(arguments.at(1), centralwidget->size());
      }
    }
  } else {// >2 FIXME, iterate through arguments and add them to the tag box
    if (settings->value("viewer/loadAction").toInt()==0)
      loadPixmap(":/images/aardview.png");
    else if (settings->value("viewer/loadAction").toInt()==1)
      loadPixmap(settings->value("viewer/lastImage").toString());
  }

  dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
                             dirViewModel->index(initialPath)));
}

void AardView::reconfigure(){
  SettingsDialog *settings = SettingsDialog::instance();

  loader->reconfigure();
  qDebug() << "Checking configuration settings (main)";
  QMainWindow::statusBar()->setVisible(settings->value("main/showStatusbar").toBool());
  // dirview options

  /** @TODO this probably needs to go through the proxy, with recent Qt this crashes */
  if (settings->value("dirview/showOnlyDirs", true).toBool())
    dirViewModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot|QDir::AllDirs);

  dirView->setColumnHidden(1, !settings->value("dirview/showSizeCol", false).toBool());
  dirView->setColumnHidden(2, !settings->value("dirview/showTypeCol", false).toBool());
  dirView->setColumnHidden(3, !settings->value("dirview/showLastModifiedCol", false).toBool());
  // tnview options
  if (settings->value("tnview/showOnlyFiles", true).toBool())
    tnViewModel->setFilter(QDir::Files);
  if (settings->value("tnview/fileMask").toString() != "" &&
      settings->value("tnview/filterFiles").toBool()){
    qDebug() << "Setting filter: " << settings->value("tnview/fileMask").toString();
    tnViewModelProxy->setFilterRegExp(settings->value("tnview/fileMask").toString());
    if (settings->value("tnview/caseInsensitiveMatching").toBool())
      tnViewModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
  } else {
    tnViewModelProxy->setFilterRegExp("");
  }
}

void AardView::closeEvent(QCloseEvent *event){
  event->ignore();
  emit requestClose(m_uid);
}

void AardView::dirIndexChanged(){
  QModelIndex idx = dirViewModelProxy->mapToSource(
    dirView->selectionModel()->currentIndex());
  qDebug() << "Path" << dirViewModel->filePath(idx);
  if (dirViewModel->isDir(idx)){
    qDebug() << "Selected item is a directory";
    tnViewModel->setDirectory(dirViewModel->filePath(idx));
    tnView->scrollToTop();
  } else {
    loadPixmap(dirViewModel->filePath(idx));
  }
}

void AardView::displayPixmap(const QPixmap &pixmap){
  // TODO: divert this to alternate image containers
  imageContainer->setPixmap(pixmap);
  imageArea->verticalScrollBar()->setValue(0);
  imageArea->horizontalScrollBar()->setValue(0);
}

void AardView::loadPixmap(const QString &filename, const QSize viewSize){
  if (viewSize == QSize())
    emit requestPixmap(filename, centralwidget->size());
  else
    emit requestPixmap(filename, viewSize);
}

void AardView::thumbIndexChanged(){
  QModelIndex idx = tnViewModelProxy->mapToSource(
    tnView->selectionModel()->currentIndex());
  qDebug() << "Path" << tnViewModel->filePath(idx);
  if (tnViewModel->isDir(idx)){
    qDebug() << "Selected item is a directory";
  } else {
    loadPixmap(tnViewModel->filePath(idx));
  }
  qDebug () << "Selected index: " << tnView->selectionModel()->currentIndex().row()
            << "Proxy index: " << idx.row();
}

QString AardView::getSelectedFilename(){
  QModelIndex idx = tnViewModelProxy->mapToSource(
    tnView->selectionModel()->currentIndex());
  if (tnViewModel->isDir(idx)){
    return QString();
  } else {
    return tnViewModel->filePath(idx);
  }
}

void AardView::handlePaste(){
  QString selection;
  QDir dir;
  QClipboard *clipboard = QApplication::clipboard();

  if (clipboard->supportsSelection()){
    qDebug() << "Running X11, checking selection buffer";
    qDebug() << "Selection contains " << clipboard->text(QClipboard::Selection);
    selection=clipboard->text(QClipboard::Selection);
    dir.setPath(selection);
  }
  // FIXME check if this works on windows as expected
  if (!clipboard->supportsSelection() || (!dir.exists() && !QFile::exists(selection))) {
    qDebug() << "Clipboard contains " << clipboard->text();
    selection=clipboard->text();
    dir.setPath(selection);
  }

  if (dir.exists()){
    QModelIndex idx = dirViewModel->index(dir.absolutePath());
    if (idx.isValid()){
      qDebug() << "Changing to " << dir.absolutePath();
      dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(idx));
    } else {
      qDebug() << "Index is not valid for: " << dir.absolutePath();
    }
  } else if (QFile::exists(selection)){
    loadPixmap(selection);
  }
}

void AardView::open(){
  QString fileName =
    QFileDialog::getOpenFileName(this,
                                 tr("Open File"), QDir::currentPath());
  if (!fileName.isEmpty()) {
    loadPixmap(fileName);
  }
}

void AardView::openEditor(){
  SettingsDialog *settings = SettingsDialog::instance();

  QString program = settings->value("main/externalEditor").toString();
  if (program !=""){

    QStringList arguments;
    arguments << loader->currentFilename();
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
  }
}

void AardView::paintToPrinter(QPrinter *printer){
#ifndef QT_NO_PRINTER
  QPainter painter(printer);
/*
// TODO: ability to get full pixmap from imagewidget
//      check if we can just print the QImage
//      provide scaling functionality
  QRect rect = painter.viewport();
  QSize size = displayedPixmap.size();
  size.scale(rect.size(), Qt::KeepAspectRatio);

  painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
  painter.setWindow(displayedPixmap.rect());
  painter.drawPixmap(0, 0, displayedPixmap);
*/
#endif
}

void AardView::print(){
#ifndef QT_NO_PRINTER
  QPrintDialog dialog(&printer, this);
  if (dialog.exec()) {
    paintToPrinter(&printer);
  }
#endif
}

void AardView::printPreview(){
#ifndef QT_NO_PRINTER
  QPrintPreviewDialog dialog(&printer, this);
  connect(&dialog, SIGNAL(paintRequested(QPrinter *)),
          this, SLOT(paintToPrinter(QPrinter *)));
  dialog.exec();
#endif
}

void AardView::selectNext(){
  // FIXME configurable: overwrap as jump to top, or jump to next directory
  int index;

  QModelIndex idx = tnView->selectionModel()->currentIndex();

  if (idx.row()+1 == tnViewModelProxy->rowCount())
    index = 0;
  else index = idx.row()+1;

  tnView->setCurrentIndex(tnViewModelProxy->index(index, 0));

  qDebug() << "Previous row: " << idx.row()
           << "Selected row: " << idx.row()+1
           << "Index: " << index
           << " Rowcount: " << tnViewModel->rowCount()
           << " Proxyrowcount: " << tnViewModelProxy->rowCount();
}

void AardView::selectPrev(){
  int index;

  QModelIndex idx = tnView->selectionModel()->currentIndex();

  if (idx.row() <= 0)
    index = tnViewModelProxy->rowCount()-1;
  else index = idx.row()-1;

  tnView->setCurrentIndex(tnViewModelProxy->index(index, 0));

  qDebug() << "Previous row: " << idx.row()
           << "Selected row: " << idx.row()+1
           << "Index: " << index
           << " Rowcount: " << tnViewModel->rowCount()
           << " Proxyrowcount: " << tnViewModelProxy->rowCount();
}

void AardView::toggleMenuBar(){
  if (menuBarVisible){
    //menuBar()->setFixedHeight(2);
    menuBar()->hide();
    menuBarVisible=false;
  } else {
    menuBar()->show();
    menuBarVisible=true;
  }
}

bool AardView::event(QEvent *event){
  if (event->type() == QEvent::Gesture)
    return gestureEvent(static_cast<QGestureEvent*>(event));

  return QWidget::event(event);
}

bool AardView::gestureEvent(QGestureEvent *event){
  if (event->gesture(Qt::TapAndHoldGesture)){
  //if (event->gesture(Qt::SwipeGesture)){
    qDebug() << "Tap'n'hold";
  } else if (event->gesture(Qt::PanGesture)){
    qDebug() << "Pan gesture";
  }
  return true;
}

void AardView::contextMenuEvent(QContextMenuEvent *event){
  QMenu menu(this);
  menu.addMenu(menuFile);
  menu.addMenu(menuEdit);
  menu.addMenu(menuView);
  menu.addMenu(menuHelp);
  menu.exec(event->globalPos());
}

bool AardView::eventFilter(QObject *obj, QEvent *event){
  SettingsDialog *settings = SettingsDialog::instance();

  if (event->type() == QEvent::MouseButtonPress){
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    switch(mouseEvent->button()){
      case Qt::MidButton:
        this->handlePaste();
        break;
      default:
        return QWidget::eventFilter(obj, event);
    }
  }

  // most likely we don't need the widget check anymore, though it might
  // come in handy later to bind the default keys only to some widget.
  // for now we don't need the keys without modifier on any widget other
  // than the image widget
  if (obj == centralwidget){
    if (event->type() == QEvent::KeyPress){
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      if (keyEvent->matches(QKeySequence::Paste)) {
        this->handlePaste();
      } else {
        switch(keyEvent->key()){
          case Qt::Key_B:
            this->selectPrev();
            break;
          case Qt::Key_M:
            this->toggleMenuBar();
            break;
          case Qt::Key_N:
            loader->normalSize();
            break;
          case Qt::Key_R:
            loader->rotate();
            break;
          case Qt::Key_Z:
            loader->toggleFtw();
            break;
          case Qt::Key_Space:
            this->selectNext();
            break;
          case Qt::Key_Minus:
            loader->zoomOut();
            break;
          case Qt::Key_Plus:
            loader->zoomIn();
            break;
          default:
            return QWidget::eventFilter(obj, event);
        };
      }
      // if we got here we consumed the event
      return true;
    } else if (event->type() == QEvent::Paint){
      // this messes up scrollbars
      //loader->repaint(centralwidget->size());
      return QWidget::eventFilter(obj, event);
    } else if (event->type() == QEvent::Enter){
      if (settings->value("main/focusFollowsMouse").toBool())
        imageArea->setFocus();
      return QWidget::eventFilter(obj, event);
    }
  }

  // if we got here we don't care about the event
  return QWidget::eventFilter(obj, event);
}
