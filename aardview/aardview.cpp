#include <QtGui>

#include "aardview.h"
#include "imagewidget.h"

AardView::AardView(){
  setupUi(this);

  this->setWindowIcon(QPixmap(":/images/aardview-icon.png"));
  bool initialized=settings.value("main/initialized").toBool();

  if (!initialized){
    qDebug() << "Setting initial settings...";
    settings.beginGroup("main");
    settings.setValue("focusFollowsMouse", true);
    settings.setValue("initialized", true);
    settings.setValue("externalEditor", "/usr/bin/gimp");
    settings.setValue("showStatusbar", false);
    settings.setValue("initialX", 640);
    settings.setValue("initialY", 480);
    settings.endGroup();
    settings.beginGroup("dirview");
    settings.setValue("showOnlyDirs", true);
    settings.setValue("showSizeCol", false);
    settings.setValue("showTypeCol", false);
    settings.setValue("showLastModifiedCol", false);
    settings.endGroup();
    settings.beginGroup("tnview");
    settings.setValue("showOnlyFiles", true);
    settings.setValue("caseInsensitiveMatching", true);
    settings.setValue("filterFiles", true);
    settings.setValue("fileMask", ".*(bmp|gif|ico|jpg|jpeg|mng|png|pbm|pgm|ppm|svg|tif|tiff|xbm|xpm)$");
    settings.endGroup();
    settings.beginGroup("viewer");
    settings.setValue("hideInfoArea", true);
    settings.setValue("resetFtwOnChange", true);
    settings.setValue("fitToWindow", true);
    settings.setValue("shrinkOnly", true);
    settings.setValue("padding", 5);
    settings.setValue("loadAction", 0);
    settings.endGroup();
    // do something on first start
  }

#ifdef QT_NO_PRINTER
  // disable printer items if qt comes without printing support
  actionPrint->setEnabled(false);
  actionPrint->setVisible(false);
  actionPrintPreview->setEnabled(false);
  actionPrintPreview->setVisible(false);
#endif

  widget->installEventFilter(this);

  // hide currently unused docks
  dockTaggedItems->hide();
  dockStatusInfo->hide();

  QString initialPath=QDir::currentPath();

  dirViewModel = new QDirModel();
  tnViewModel = new TnViewModel(initialPath);
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
  dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
                             dirViewModel->index(initialPath)));

  tnViewModelProxy->setSourceModel(tnViewModel);
  tnView->setModel(tnViewModelProxy);
  tnViewModel->setDirectory(initialPath);

  settingsDialog=new SettingsDialog;

  // finally connect everything we didn't connect by designer already
  connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
  connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  connect(settingsDialog, SIGNAL(configurationChanged()),
          this, SLOT(reconfigure()));
  connect(settingsDialog, SIGNAL(configurationChanged()),
          widget, SLOT(reconfigure()));
  connect(dirView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(dirIndexChanged()));
  connect(tnView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(thumbIndexChanged()));

  this->resize(settings.value("main/initialX").toInt(),
               settings.value("main/initialY").toInt());
  reconfigure();

#ifdef QT46
  grabGesture(Qt::TapAndHoldGesture);
  grabGesture(Qt::PanGesture);
  grabGesture(Qt::SwipeGesture);
#endif

  // using a timer we make sure this get's called once the UI
  // is already set up, avoiding annoying resize problems
  QTimer::singleShot(0, this, SLOT(handleArguments()));
}

AardView::~AardView(){
  if (settings.value("main/saveSizeOnExit").toBool()){
    qDebug() << "Saving current window size" << width();
    settings.setValue("main/initialX", width());
    settings.setValue("main/initialY", height());
  }
}

void AardView::handleArguments(){
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
        widget->load(arguments.at(1));
      }
    }
  } // >2 FIXME, iterate through arguments and add them to the tag box
}

void AardView::reconfigure(){
  qDebug() << "Checking configuration settings (main)";
  QMainWindow::statusBar()->setVisible(settings.value("main/showStatusbar").toBool());
  // dirview options
  if (settings.value("dirview/showOnlyDirs", true).toBool())
    dirViewModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
  dirView->setColumnHidden(1, !settings.value("dirview/showSizeCol", false).toBool());
  dirView->setColumnHidden(2, !settings.value("dirview/showTypeCol", false).toBool());
  dirView->setColumnHidden(3, !settings.value("dirview/showLastModifiedCol", false).toBool());
  // tnview options
  if (settings.value("tnview/showOnlyFiles", true).toBool())
    tnViewModel->setFilter(QDir::Files);
  if (settings.value("tnview/fileMask").toString() != "" &&
      settings.value("tnview/filterFiles").toBool()){
    qDebug() << "Setting filter: " << settings.value("tnview/fileMask").toString();
    tnViewModelProxy->setFilterRegExp(settings.value("tnview/fileMask").toString());
    if (settings.value("tnview/caseInsensitiveMatching").toBool())
      tnViewModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
  } else {
    tnViewModelProxy->setFilterRegExp("");
  }
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
    widget->load(dirViewModel->filePath(idx));
  }
}

void AardView::thumbIndexChanged(){
  QModelIndex idx = tnViewModelProxy->mapToSource(
    tnView->selectionModel()->currentIndex());
  qDebug() << "Path" << tnViewModel->filePath(idx);
  if (tnViewModel->isDir(idx)){
    qDebug() << "Selected item is a directory";
  } else {
    widget->load(tnViewModel->filePath(idx));
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
    qDebug() << "Changing to " << dir.absolutePath();
    dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
                               dirViewModel->index(dir.absolutePath())));
  } else if (QFile::exists(selection)){
    widget->load(selection);
  }
}

void AardView::openEditor(){
  QString program = settings.value("main/externalEditor").toString();
  if (program !=""){

    QStringList arguments;
    arguments << widget->currentFilename();
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
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

void AardView::showSettings(){
  settingsDialog->show();
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

void AardView::about(){
  QString supportedWriteFormats;
  QString supportedReadFormats;
  for (int i = 0; i < QImageReader::supportedImageFormats().count(); ++i){
    supportedReadFormats += " " +
      QString(QImageReader::supportedImageFormats().at(i)).toLower();
  }
  for (int i = 0; i < QImageWriter::supportedImageFormats().count(); ++i){
    supportedWriteFormats += " " +
      QString(QImageWriter::supportedImageFormats().at(i)).toLower();
  }

  QMessageBox::about(this, tr("About Menu"),
                     tr("<h1>About Aardview</h1><br />"
                        "A simple image viewer written by Bernd Wachter. You can visit the <a href=\"http://bwachter.lart.info/projects/aardview/\">project homepage</a> for more information.<br /><br />"
                        "Aardwork has been contributed by prism.<br /><br />"
                        "For bug reports and suggestions please <a href=\"https://mantis.lart.info\">visit my mantis installation</a>."
                        "<h3>Supported formats</h3>"
                        "Reading: %2<br />"
                        "Writing: %3<br />"
#ifdef EXIF
                        "<p align=\"right\">Build key: %1, EXIF</p>"
#else
                        "<p align=\"right\">Build key: %1</p>"
#endif
                       )
                     .arg(QLibraryInfo::buildKey())
                     .arg(supportedReadFormats)
                     .arg(supportedWriteFormats)
    );
}

bool AardView::event(QEvent *event){
#ifdef QT46
  if (event->type() == QEvent::Gesture)
    return gestureEvent(static_cast<QGestureEvent*>(event));
#endif
  return QWidget::event(event);
}

#ifdef QT46
bool AardView::gestureEvent(QGestureEvent *event){
  if (event->gesture(Qt::TapAndHoldGesture)){
  //if (event->gesture(Qt::SwipeGesture)){
    qDebug() << "Tap'n'hold";
  } else if (event->gesture(Qt::PanGesture)){
    qDebug() << "Pan gesture";
  }
  return true;
}
#endif

void AardView::contextMenuEvent(QContextMenuEvent *event){
  QMenu menu(this);
  menu.addMenu(menuFile);
  menu.addMenu(menuEdit);
  menu.addMenu(menuView);
  menu.addMenu(menuHelp);
  menu.exec(event->globalPos());
}

bool AardView::eventFilter(QObject *obj, QEvent *event){
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
  if (obj == widget){
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
            widget->normalSize();
            break;
          case Qt::Key_R:
            widget->rotate();
            break;
          case Qt::Key_Z:
            widget->toggleFtw();
            break;
          case Qt::Key_Space:
            this->selectNext();
            break;
          case Qt::Key_Minus:
            widget->zoomOut();
            break;
          case Qt::Key_Plus:
            widget->zoomIn();
            break;
          default:
            return QWidget::eventFilter(obj, event);
        };
      }
      // if we got here we consumed the event
      return true;
    }
  }
  // if we got here we don't care about the event
  return QWidget::eventFilter(obj, event);
}
