#include <QtGui>

#include "aardview.h"
#include "imagewidget.h"

AardView::AardView(){
  ui.setupUi(this);

  this->setWindowIcon(QPixmap(":/images/aardview-icon.png"));
  bool initialized=settings.value("main/initialized").toBool();

  if (!initialized){
    qDebug() << "Setting initial settings...";
    settings.beginGroup("main");
    settings.setValue("focusFollowsMouse", true);
    settings.setValue("initialized", true);
    settings.setValue("externalEditor", "/usr/bin/gimp");
    settings.setValue("showStatusbar", false);
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
    settings.endGroup();
    // do something on first start
  }

// remove printer items if qt comes without printer support #ifndef QT_NO_PRINTER

  widget=new ImageWidget();
  widget->installEventFilter(this);
  dirViewModel = new QDirModel();
  tnViewModel = new TnViewModel(QDir::currentPath());
  dirViewModelProxy = new QSortFilterProxyModel();
  tnViewModelProxy = new QSortFilterProxyModel();

  setCentralWidget(widget);

  // add toggle actions for docks
  ui.menuView->addAction(ui.dockDirectoryTree->toggleViewAction());
  ui.menuView->addAction(ui.dockTreeView->toggleViewAction());
  ui.menuView->addAction(ui.dockTaggedItems->toggleViewAction());
  ui.dockTaggedItems->hide();
  ui.dockStatusInfo->hide();

  // set the model
  dirViewModelProxy->setSourceModel(dirViewModel);
  ui.dirView->setModel(dirViewModelProxy);
  ui.dirView->setRootIndex(dirViewModelProxy->mapFromSource(
                          dirViewModel->index(QDir::rootPath())));
  ui.dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
                             dirViewModel->index(QDir::currentPath())));

  tnViewModelProxy->setSourceModel(tnViewModel);
  ui.tnView->setModel(tnViewModelProxy);  
  tnViewModel->setDirectory(QDir::currentPath());

  settingsDialog=new SettingsDialog;

  // finally connect everything we didn't connect by designer already
  connect(ui.actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
  connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  connect(settingsDialog, SIGNAL(configurationChanged()),
          this, SLOT(reconfigure()));
  connect(settingsDialog, SIGNAL(configurationChanged()),
          widget, SLOT(reconfigure()));
  connect(ui.dirView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(dirIndexChanged()));
  connect(ui.tnView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &,
                                  const QItemSelection &)),
          this, SLOT(thumbIndexChanged()));

  reconfigure();
}

void AardView::reconfigure(){
  qDebug() << "Checking configuration settings (main)";
  statusBar()->setVisible(settings.value("main/showStatusbar").toBool());
  // dirview options
  if (settings.value("dirview/showOnlyDirs", true).toBool())
    dirViewModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
  ui.dirView->setColumnHidden(1, !settings.value("dirview/showSizeCol", false).toBool());
  ui.dirView->setColumnHidden(2, !settings.value("dirview/showTypeCol", false).toBool());
  ui.dirView->setColumnHidden(3, !settings.value("dirview/showLastModifiedCol", false).toBool());
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
    ui.dirView->selectionModel()->currentIndex());
  qDebug() << "Path" << dirViewModel->filePath(idx);
  if (dirViewModel->isDir(idx)){
    qDebug() << "Selected item is a directory";
    tnViewModel->setDirectory(dirViewModel->filePath(idx));
    ui.tnView->scrollToTop();
  } else {
    widget->load(dirViewModel->filePath(idx));
  }
}

void AardView::thumbIndexChanged(){
  QModelIndex idx = tnViewModelProxy->mapToSource(
    ui.tnView->selectionModel()->currentIndex());
  qDebug() << "Path" << tnViewModel->filePath(idx);
  if (tnViewModel->isDir(idx)){
    qDebug() << "Selected item is a directory";
  } else {
    widget->load(tnViewModel->filePath(idx));
  }
  qDebug () << "Selected index: " << ui.tnView->selectionModel()->currentIndex().row()
            << "Proxy index: " << idx.row();
}

QString AardView::getSelectedFilename(){
  QModelIndex idx = tnViewModelProxy->mapToSource(
    ui.tnView->selectionModel()->currentIndex());
  if (tnViewModel->isDir(idx)){
    return QString();
  } else {
    return tnViewModel->filePath(idx);
  }
}

void AardView::handlePaste(){
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
    ui.dirView->setCurrentIndex(dirViewModelProxy->mapFromSource(
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

void AardView::selectNext(){
  // TODO
  // configurable: overwrap as jump to top, or jump to next directory
  int index;

  QModelIndex idx = ui.tnView->selectionModel()->currentIndex();

  if (idx.row()+1 == tnViewModelProxy->rowCount())
    index = 0;
  else index = idx.row()+1;

  ui.tnView->setCurrentIndex(tnViewModelProxy->index(index, 0));

  qDebug() << "Previous row: " << idx.row() 
           << "Selected row: " << idx.row()+1
           << "Index: " << index
           << " Rowcount: " << tnViewModel->rowCount()
           << " Proxyrowcount: " << tnViewModelProxy->rowCount();
}

void AardView::selectPrev(){
  int index;

  QModelIndex idx = ui.tnView->selectionModel()->currentIndex();

  if (idx.row() <= 0)
    index = tnViewModelProxy->rowCount()-1;
  else index = idx.row()-1;

  ui.tnView->setCurrentIndex(tnViewModelProxy->index(index, 0));

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

void AardView::contextMenuEvent(QContextMenuEvent *event){
  QMenu menu(this);
  menu.addMenu(ui.menuFile);
  menu.addMenu(ui.menuEdit);
  menu.addMenu(ui.menuView);
  menu.addMenu(ui.menuHelp);
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
