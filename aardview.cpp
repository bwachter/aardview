#include <QtGui>

#include "aardview.h"
#include "imagewidget.h"

AardView::AardView(){
  bool initialized=settings.value("main/initialized").toBool();

  if (!initialized){
    qDebug() << "Setting initial settings...";
    settings.beginGroup("main");
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
    settings.setValue("fileMask", ".*(bmp|gif|jpg|jpeg|png|pbm|pgm|ppm|tif|tiff|xbm|xpm)");
    settings.endGroup();
    settings.beginGroup("viewer");
    settings.setValue("hideInfoArea", true);
    settings.setValue("resetFtwOnChange", true);
    settings.setValue("fitToWindow", true);
    settings.setValue("padding", 5);
    settings.endGroup();
    // do something on first start
  }

  widget=new ImageWidget();
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

  editAct = new QAction(tr("&Edit"), this);
  editAct->setShortcut(tr("Ctrl+E"));
  editAct->setStatusTip(tr("Edit in external editor"));
  connect(editAct, SIGNAL(triggered()), this, SLOT(openEditor()));

  settingsAct = new QAction(tr("Settings"), this);
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
  fileMenu->addAction(settingsAct);
  fileMenu->addAction(editAct);
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(tr("Show/hide menu bar"), this, 
                      SLOT(toggleMenuBar()), QKeySequence(tr("Ctrl+M")));

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void AardView::createDocks(){
  // create the dock items...
  QDockWidget *dock = new QDockWidget(tr("Directory tree"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dirView = new QTreeView(dock);
  dock->setWidget(dirView);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Thumbnail view"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  tnView = new QListView(dock);
  dock->setWidget(tnView);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Tagged items"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  tagView = new QListView(dock);
  dock->setWidget(tagView);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());
  dock->hide();

  // and set the model
  dirViewModelProxy->setSourceModel(dirViewModel);
  dirView->setModel(dirViewModelProxy);
  dirView->setRootIndex(dirViewModelProxy->mapFromSource(
                          dirViewModel->index(QDir::rootPath())));
  dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
                             dirViewModel->index(QDir::currentPath())));

  tnViewModelProxy->setSourceModel(tnViewModel);
  tnView->setModel(tnViewModelProxy);  
  tnViewModel->setDirectory(QDir::currentPath());

  if (settings.value("tnView/fileMask").toString() != "" && 
      settings.value("tnView/filterFiles").toBool()){
    qDebug() << "Setting filter: " << settings.value("tnView/fileMask").toString();
    tnViewModelProxy->setFilterRegExp(settings.value("tnView/fileMask").toString());
    tnViewModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
  }

  if (settings.value("tnView/showOnlyFiles", true).toBool())
    tnViewModel->setFilter(QDir::Files);

  if (settings.value("dirView/showOnlyDirs", true).toBool())
    dirViewModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    
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

void AardView::openEditor(){
  QString program = settings.value("main/externalEditor").toString();
  if (program !=""){
    
    QStringList arguments;
    arguments << getSelectedFilename();
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
  }
}

void AardView::showSettings(){
  settingsDialog->show();
}

void AardView::toggleMenuBar(){
  if (menuBarVisible){
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
                        "FIXME<br />"
                        "<h2>Supported formats</h2>"
                        "Reading: %12<br />"
                        "Writing: %13<br />"
                        "<h2>Build information</h2>"
                        "Licensed to: %1<br />"
                        "Licensed products: %2<br />"
                        "Build key: %3<br />"
                        "<h3>Path names</h3>"
                        "Documentation: %4<br />"
                        "Headers: %5<br />"
                        "Libraries: %6<br />"
                        "Binaries: %7<br />"
                        "Plugins: %8<br />"
                        "Data: %9<br />"
                        "Translations: %10<br />"
                        "Settings: %11<br />"
                       )
                     .arg(QLibraryInfo::licensee())
                     .arg(QLibraryInfo::licensedProducts())
                     .arg(QLibraryInfo::buildKey())
                     .arg(QLibraryInfo::location(QLibraryInfo::DocumentationPath))
                     .arg(QLibraryInfo::location(QLibraryInfo::HeadersPath))
                     .arg(QLibraryInfo::location(QLibraryInfo::LibrariesPath))
                     .arg(QLibraryInfo::location(QLibraryInfo::BinariesPath))
                     .arg(QLibraryInfo::location(QLibraryInfo::PluginsPath))
                     .arg(QLibraryInfo::location(QLibraryInfo::DataPath))
                     .arg(QLibraryInfo::location(QLibraryInfo::TranslationsPath))
                     .arg(QLibraryInfo::location(QLibraryInfo::SettingsPath))
                     .arg(supportedReadFormats)
                     .arg(supportedWriteFormats)
    );
}
