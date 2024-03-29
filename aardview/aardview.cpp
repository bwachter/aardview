/**
 * @file aardview.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#include <QtGui>
#include <QMessageBox>
#include <QScrollBar>
#include <QFileDialog>

#include "aardview.h"
#include "afileinfo.h"
#include "settingsdialog.h"
#include "aardviewlog.h"

AardView::AardView(QUuid uid, QString initialPath){
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

  exifViewModel = new ExifViewModel();
  dirViewModel = new QFileSystemModel();
  tnViewModel = new TnViewModel();
  tnViewModelProxy = new QSortFilterProxyModel();

  // add toggle actions for docks
  dockDirectoryTree->toggleViewAction()->setShortcut(QKeySequence(tr("Ctrl+D")));
  dockTreeView->toggleViewAction()->setShortcut(QKeySequence(tr("Ctrl+T")));

  menuView->addAction(dockDirectoryTree->toggleViewAction());
  menuView->addAction(dockTreeView->toggleViewAction());
  menuView->addAction(dockTaggedItems->toggleViewAction());

  dirViewModel->setRootPath(QDir::rootPath());
  dirView->setModel(dirViewModel);
  dirView->setRootIndex(dirViewModel->index(QDir::rootPath()));

  exifView->setModel(exifViewModel);

  tnViewModelProxy->setSourceModel(tnViewModel);
  tnView->setModel(tnViewModelProxy);

  // finally connect everything we didn't connect by designer already
  // file menu actions
  connect(actionOpen, SIGNAL(triggered()), this, SLOT(forwardOpen()));
  connect(actionOpenNew, SIGNAL(triggered()), this, SIGNAL(showOpen()));
  connect(actionPrint, SIGNAL(triggered()), this, SLOT(forwardPrint()));
  connect(actionPrintPreview, SIGNAL(triggered()),
          this, SLOT(forwardPrintPreview()));

  connect(actionEditCurrentImage, SIGNAL(triggered()),
          this, SLOT(forwardEdit()));
  connect(actionExit, SIGNAL(triggered()), this, SLOT(forwardQuit()));
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
  // TODO: unlike QDirModel QFileSystemModel doesn't have a refresh slot
  //       it seems it shouldn't be needed if root directory gets moved correctly,
  //       but needs testing
  connect(actionRefreshDirs, SIGNAL(triggered()), dirViewModel, SLOT(refresh()));
  connect(this, SIGNAL(requestPixmap(const QString &, const QSize &)),
          loader, SLOT(load(const QString &, const QSize &)));
  connect(loader, SIGNAL(pixmapReady(const QPixmap &)), this, SLOT(displayPixmap(const QPixmap &)));
  connect(loader, SIGNAL(exifDataReady(const QMap<QString, QString> &)),
          exifViewModel, SLOT(setMap(const QMap<QString, QString> &)));

  // TODO: this should be uid pecidic state restore
  /*
  this->resize(settings->value("main/initialX").toInt(),
               settings->value("main/initialY").toInt());
  */
  reconfigure();

  // FIXME: make use of it
  imageName->hide();

  qDebug() << "Current size: " << centralwidget->size();

  // grabGesture(Qt::TapGesture);
  grabGesture(Qt::TapAndHoldGesture);
  grabGesture(Qt::PanGesture);
  grabGesture(Qt::PinchGesture);
  grabGesture(Qt::SwipeGesture);
  //grabGesture(Qt::CustomGesture);

  // store initial locations, initial drawing happens through
  // polish event
  setPath(initialPath);
}

AardView::~AardView(){
  qDebug() << "Destroying window " << m_uid;

  // TODO: this should become uid-specific state saving
  /*
  if (settings->value("main/saveSizeOnExit").toBool()){
    qDebug() << "Saving current window size" << width();
    settings->setValue("main/initialX", width());
    settings->setValue("main/initialY", height());
  }
  */
}

void AardView::load(const QString &path){
  SettingsDialog *settings = SettingsDialog::instance();

  setPath(path);

  AFileInfo info(path);

  dirView->setCurrentIndex(dirViewModel->index(info.absolutePath()));

  // TODO: proper switching between single image and directory mode
  if (info.isFile()){
    dockDirectoryTree->hide();
    dockTreeView->hide();
    loadPixmap(path, centralwidget->size());
    setLoadedPath(path);
  } else if (settings->value("viewer/loadAction").toInt()==0)
    loadPixmap(":/images/aardview.png");
}

void AardView::reconfigure(){
  SettingsDialog *settings = SettingsDialog::instance();

  loader->reconfigure();
  qDebug() << "Checking configuration settings (main)";
  QMainWindow::statusBar()->setVisible(settings->value("main/showStatusbar").toBool());

  // logging options
  AardviewLog::setPriority(settings->value("log/loglevel", LOG_INFO).toInt());
  AardviewLog::setConsoleLogging(settings->value("log/console", 0).toInt());

  // dirview options
  /** @TODO this probably needs to go through the proxy, with recent Qt this crashes */
  if (settings->value("dirview/showOnlyDirs", true).toBool()){
    dirViewModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot|QDir::AllDirs|QDir::Drives);
  } else {
    dirViewModel->setFilter(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::AllDirs);
  }


  dirView->setColumnHidden(1, !settings->value("dirview/showSizeCol", false).toBool());
  dirView->setColumnHidden(2, !settings->value("dirview/showTypeCol", false).toBool());
  dirView->setColumnHidden(3, !settings->value("dirview/showLastModifiedCol", false).toBool());
  // tnview options
  if (settings->value("tnview/showOnlyFiles", true).toBool())
    tnViewModel->setFilter(QDir::Files);
  if (settings->value("tnview/fileMask").toString() != "" &&
      settings->value("tnview/filterFiles").toBool()){
    qDebug() << "Setting filter: " << settings->value("tnview/fileMask").toString();
    tnViewModelProxy->setFilterRegularExpression(settings->value("tnview/fileMask").toString());
    if (settings->value("tnview/caseInsensitiveMatching").toBool())
      tnViewModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
  } else {
    tnViewModelProxy->setFilterRegularExpression("");
  }
}

void AardView::closeEvent(QCloseEvent *event){
  event->ignore();
  emit requestClose(m_uid);
}

void AardView::dirIndexChanged(){
  QModelIndex idx = dirView->selectionModel()->currentIndex();

  qDebug() << "Path" << dirViewModel->filePath(idx);

  if (dirViewModel->isDir(idx)){
    qDebug() << "Selected item is a directory";
    tnViewModel->setDirectory(dirViewModel->filePath(idx));
    tnView->scrollToTop();
    setPath(dirViewModel->filePath(idx));
  } else {
    loadPixmap(dirViewModel->filePath(idx));
    setLoadedPath(dirViewModel->filePath(idx));
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
    setLoadedPath(tnViewModel->filePath(idx));
  }

  qDebug () << "Selected index: " << tnView->selectionModel()->currentIndex().row()
            << "Proxy index: " << idx.row();
}

QString AardView::path(){
  return m_path;
}

QString AardView::loadedPath(){
  return m_loadedPath;
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
      dirView->setCurrentIndex(idx);
    } else {
      qInfo() << "Index is not valid for: " << dir.absolutePath();
    }
  } else if (QFile::exists(selection)){
    loadPixmap(selection);
  }
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

void AardView::setPath(const QString &path){
  m_path = path;

  setWindowTitle(title());
}

void AardView::setLoadedPath(const QString &path){
  m_path = path;
  m_loadedPath = path;

  setWindowTitle(title());
}

QString AardView::title(){
  QString result="<unknown>";

  if (path() != QString()){
    result = path();
  } else if (loadedPath() != QString()){
    result = loadedPath();
  }

  AFileInfo info(result);
  return info.friendlyFilePath();
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
  if (event->type() == QEvent::PolishRequest){
    load(path());
  }
  if (event->type() == QEvent::Resize)
    loader->repaint(centralwidget->size());

  return QWidget::event(event);
}

bool AardView::gestureEvent(QGestureEvent *event){
  if (event->gesture(Qt::TapAndHoldGesture)){
  //if (event->gesture(Qt::SwipeGesture)){
    qDebug() << "Tap'n'hold";
  } else if (event->gesture(Qt::PanGesture)){
    qDebug() << "Pan gesture";
  } else if (event->gesture(Qt::PinchGesture)){
    qDebug() << "Pinch gesture";
  } else if (event->gesture(Qt::SwipeGesture)){
    qDebug() << "Swipe gesture";
  } else if (event->gesture(Qt::CustomGesture)){
    qDebug() << "Custom gesture";
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
      case Qt::MiddleButton:
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
          case Qt::Key_F:
            loader->toggleFlag();
            break;
          case Qt::Key_M:
            this->toggleMenuBar();
            break;
          case Qt::Key_N:
            loader->normalSize();
            break;
          case Qt::Key_P:
            loader->process();
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
  } else {
    // global events
    if (event->type() == QEvent::KeyPress){
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      switch(keyEvent->key()){
        case Qt::Key_P:
          loader->process();
          break;
        default:
          return QWidget::eventFilter(obj, event);
      };

      return true;
    }
  }

  // if we got here we don't care about the event
  return QWidget::eventFilter(obj, event);
}
