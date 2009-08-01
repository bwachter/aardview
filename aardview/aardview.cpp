#include <QtGui>

#include "aardview.h"
#include "imagewidget.h"

AardView::AardView(){
  this->setWindowIcon(QPixmap(":/images/aardview-icon.png"));
  bool initialized=settings.value("main/initialized").toBool();

  if (!initialized){
    qDebug() << "Setting initial settings...";
    settings.beginGroup("main");
    settings.setValue("focusFollowsMouse", false);
    settings.setValue("initialized", true);
    settings.setValue("externalEditor", "/usr/bin/gimp");
    settings.endGroup();
    settings.beginGroup("dirview");
    settings.setValue("showOnlyDirs", true);
    settings.endGroup();
    settings.beginGroup("tnview");
    settings.setValue("showOnlyFiles", true);
    settings.setValue("caseInsensitiveMatching", true);
    settings.setValue("filterFiles", true);
    settings.setValue("fileMask", ".*(bmp|gif|jpg|jpeg|png|pbm|pgm|ppm|svg|tif|tiff|xbm|xpm)$");
    settings.endGroup();
    settings.beginGroup("viewer");
    settings.setValue("hideInfoArea", true);
    settings.setValue("resetFtwOnChange", true);
    settings.setValue("fitToWindow", true);
    settings.setValue("shrinkOnly", true);
    settings.setValue("padding", 5);
    settings.endGroup();
    // do something on first start
  }

  widget=new ImageWidget();
  widget->installEventFilter(this);
  dirViewModel = new QDirModel();
  tnViewModel = new TnViewModel(QDir::currentPath());
  dirViewModelProxy = new QSortFilterProxyModel();
  tnViewModelProxy = new QSortFilterProxyModel();

  setCentralWidget(widget);
  
  createActions();
  createMenus();
  createDocks();
  settingsDialog=new SettingsDialog;
  createPopupMenu();
  connect(settingsDialog, SIGNAL(configurationChanged()),
          this, SLOT(reconfigure()));
  connect(settingsDialog, SIGNAL(configurationChanged()),
          widget, SLOT(reconfigure()));
}

void AardView::reconfigure(){
  qDebug() << "Checking configuration settings (main)";
  if (settings.value("dirView/showOnlyDirs", true).toBool())
    dirViewModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
  if (settings.value("tnView/showOnlyFiles", true).toBool())
    tnViewModel->setFilter(QDir::Files);
  if (settings.value("tnView/fileMask").toString() != "" && 
      settings.value("tnView/filterFiles").toBool()){
    qDebug() << "Setting filter: " << settings.value("tnView/fileMask").toString();
    tnViewModelProxy->setFilterRegExp(settings.value("tnView/fileMask").toString());
    if (settings.value("tnview/caseInsensitiveMatching").toBool())
      tnViewModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
  } else {
    tnViewModelProxy->setFilterRegExp("");
  }
}

void AardView::createActions(){
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
        
  aboutQtAct = new QAction(tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+X"));
  exitAct->setStatusTip(tr("Exit Aardview"));
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

  editAct = new QAction(tr("&Edit current image"), this);
  editAct->setShortcut(tr("Ctrl+E"));
  editAct->setStatusTip(tr("Edit current image in external editor"));
  connect(editAct, SIGNAL(triggered()), this, SLOT(openEditor()));

  settingsAct = new QAction(tr("P&references"), this);
  connect(settingsAct, SIGNAL(triggered()), this, SLOT(showSettings()));

  // context menus
  minimizeAct = new QAction(tr("Mi&nimize"), this);
  connect(minimizeAct, SIGNAL(triggered()), this, SLOT(hide()));

  maximizeAct = new QAction(tr("Ma&ximize"), this);
  connect(maximizeAct, SIGNAL(triggered()), this, SLOT(showMaximized()));

  restoreAct = new QAction(tr("&Restore"), this);
  connect(restoreAct, SIGNAL(triggered()), this, SLOT(showNormal()));
}

void AardView::createMenus(){
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(tr("Open"), widget,
                      SLOT(open()), QKeySequence(tr("Ctrl+O")));
#ifndef QT_NO_PRINTER
  fileMenu->addSeparator();
  fileMenu->addAction(tr("Print"), widget,
                      SLOT(print()), QKeySequence(tr("Ctrl+P")));
  fileMenu->addAction(tr("Print Preview"), widget,
                      SLOT(printPreview()));
#endif
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(editAct);
  editMenu->addSeparator();
  editMenu->addAction(settingsAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(tr("Zoom in"), widget,
                      SLOT(zoomIn()), QKeySequence(tr("Ctrl++")));
  viewMenu->addAction(tr("Zoom out"), widget,
                      SLOT(zoomOut()), QKeySequence(tr("Ctrl+-")));
  viewMenu->addAction(tr("Toggle 'fit to window'"), widget,
                      SLOT(toggleFtw()), QKeySequence(tr("Ctrl+Z")));
  viewMenu->addSeparator();
  viewMenu->addAction(tr("Toggle menu bar"), this, 
                      SLOT(toggleMenuBar()), QKeySequence(tr("Ctrl+M")));
  viewMenu->addSeparator();
  
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void AardView::createDocks(){
  // create the dock items...
  QDockWidget *dock = new QDockWidget(tr("Directory tree"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dirView = new ATreeView(dock);
  dock->setWidget(dirView);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Thumbnail view"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  tnView = new AListView(dock);
  dock->setWidget(tnView);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Tagged items"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  tagView = new AListView(dock);
  dock->setWidget(tagView);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());
  dock->hide();

  // and set the model
  reconfigure();  

  dirViewModelProxy->setSourceModel(dirViewModel);
  dirView->setModel(dirViewModelProxy);
  dirView->setRootIndex(dirViewModelProxy->mapFromSource(
                          dirViewModel->index(QDir::rootPath())));
  dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
                             dirViewModel->index(QDir::currentPath())));

  // TODO maybe make this configurable
  // size
  dirView->hideColumn(1);
  // type
  dirView->hideColumn(2);
  // last modified
  dirView->hideColumn(3);

  tnViewModelProxy->setSourceModel(tnViewModel);
  tnView->setModel(tnViewModelProxy);  
  tnViewModel->setDirectory(QDir::currentPath());

  connect(dirView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(dirIndexChanged()));
  connect(tnView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(thumbIndexChanged()));
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
  qDebug() << "Paste handler not yet implemented";
  QDir dir;
  QClipboard *clipboard = QApplication::clipboard();

  if (clipboard->supportsSelection()){
    qDebug() << "Running X11, checking selection buffer";
    qDebug() << "Selection contains " << clipboard->text(QClipboard::Selection);
    dir.setPath(clipboard->text(QClipboard::Selection));
  } 
  // TODO check if this works on windows as expected
  if (!clipboard->supportsSelection() || !dir.exists()) {
    qDebug() << "Clipboard contains " << clipboard->text();
    dir.setPath(clipboard->text());
  }

  if (dir.exists()){
    qDebug() << "Changing to " << dir.absolutePath();
    dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
                               dirViewModel->index(dir.absolutePath())));
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

void AardView::showSettings(){
  settingsDialog->show();
}

void AardView::toggleMenuBar(){
  if (menuBarVisible){
    //menuBar()->setFixedHeight(2);
    menuBar()->hide();
    menuBarVisible=false;
    this->adjustSize();
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
                        "A simple image viewer written by Bernd Wachter<br />"
                        "Aardwork contributed by prism<br />"
                        "<h3>Supported formats</h3>"
                        "Reading: %2<br />"
                        "Writing: %3<br />"
                        "<p align=\"right\">Build key: %1</p>"
                       )
                     .arg(QLibraryInfo::buildKey())
                     .arg(supportedReadFormats)
                     .arg(supportedWriteFormats)
    );
}

void AardView::contextMenuEvent(QContextMenuEvent *event){
  QMenu menu(this);
  menu.addMenu(fileMenu);
  menu.addMenu(editMenu);
  menu.addMenu(viewMenu);
  menu.addMenu(helpMenu);
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

  // most likely we don't neet the widget check anymore, though it might 
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
            // prev picture
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
            // next picture
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
