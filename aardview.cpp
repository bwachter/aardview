#include <QtGui>

#include "aardview.h"
#include "imagewidget.h"

AardView::AardView(){
  widget=new ImageWidget(this);
  model = new QDirModel();
  dirViewModel = new QSortFilterProxyModel();
  tnViewModel = new QSortFilterProxyModel();

  settings.beginGroup("main");
  bool initialized=settings.value("initialized").toBool();
  settings.endGroup();

  if (!initialized){
    settings.setValue("initialized", true);
    settings.beginGroup("dirview");
    settings.setValue("showOnlyDirs", true);
    settings.setValue("fileMask", "*.jpeg *.jpg *.png");
    settings.endGroup();
    settings.beginGroup("tnview");
    settings.setValue("showOnlyFiles", true);
    settings.endGroup();
    // do something on first start
  }

  setCentralWidget(widget);
  
  createActions();
  createMenus();
  createDocks();
  settingsDialog=new SettingsDialog;
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
  exitAct->setStatusTip(tr("Exit Aardbei"));
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

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
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));

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

  // and set the model
  dirViewModel->setSourceModel(model);
  dirView->setModel(dirViewModel);
  dirView->setRootIndex(dirViewModel->mapFromSource(
                          model->index(QDir::rootPath())));
  dirView->setCurrentIndex(dirViewModel->mapFromSource(
                             model->index(QDir::currentPath())));

  tnViewModel->setSourceModel(model);
  tnViewModel->setFilterRole(QDir::Files);
  tnView->setModel(tnViewModel);  
  tnView->setRootIndex(tnViewModel->mapFromSource(
                         model->index(QDir::currentPath())));

  //if (settings.value("tnView/showOnlyFiles", true).toBool)
  //QDirModel *md=(QDirModel*)tnView->model();
  //md->setFilter(QDir::Files);

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
  QModelIndex idx = dirViewModel->mapToSource(
    dirView->selectionModel()->currentIndex());
  qDebug() << "Path" << model->filePath(idx);
  if (model->isDir(idx)){
    qDebug() << "Selected item is a directory";
    tnView->setRootIndex(tnViewModel->mapFromSource(idx));
  } else {
    widget->load(model->filePath(idx));
  }
}

void AardView::thumbIndexChanged(){
  QModelIndex idx = tnViewModel->mapToSource(
    tnView->selectionModel()->currentIndex());
  qDebug() << "Path" << model->filePath(idx);
  if (model->isDir(idx)){
    qDebug() << "Selected item is a directory";
  } else {
    widget->load(model->filePath(idx));
  }
}

void AardView::showSettings(){
  settingsDialog->show();
}
