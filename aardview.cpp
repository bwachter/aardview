#include <QtGui>

#include "aardview.h"
#include "imagewidget.h"

AardView::AardView(){
  widget=new ImageWidget(this);
  model = new QDirModel();

  setCentralWidget(widget);
  
  createActions();
  createMenus();
  createDocks();
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
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void AardView::createDocks(){
  QDockWidget *dock = new QDockWidget(tr("Directory tree"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  tree = new QTreeView(dock);
  dock->setWidget(tree);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  
  dock = new QDockWidget(tr("Thumbnail view"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  list = new QListView(dock);
  dock->setWidget(list);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  tree->setModel(model);
  tree->setRootIndex(model->index(QDir::rootPath()));
  tree->setCurrentIndex(model->index(QDir::currentPath()));
  list->setModel(model);  
  list->setRootIndex(model->index(QDir::rootPath()));
  list->setCurrentIndex(model->index(QDir::currentPath()));

  connect(tree->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(dirIndexChanged()));
}

void AardView::dirIndexChanged(){
  qDebug("Selected new item");
  int row = tree->selectionModel()->currentIndex().row();
  int column = tree->selectionModel()->currentIndex().column();
  //QAbstractItemModel *am = tree->model();
  //QDirModel *am = tree->model();
  //QVariant data = am->data(tree->selectionModel()->currentIndex());
  QModelIndex idx = tree->selectionModel()->currentIndex();
  //qDebug() << "String value of item = " << data.toString();
  qDebug() << "Path" << model->filePath(idx);
  if (model->isDir(idx)){
    qDebug() << "Selected item is a directory";
    list->setRootIndex(tree->selectionModel()->currentIndex());
  } else {
    widget->load(model->filePath(idx));
  }
}
