/**
 * @file aardviewshim.cpp
 * @copyright 2016
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2016
 */

#include <QMessageBox>
#include "aardviewshim.h"
#include "aardview.h"

AardviewShim::AardviewShim(){
    m_windowModel = new WindowModel();

  if (useTray){
    createTrayIcon();
    trayIcon->show();
  }

  // if tray icon exists start a first window, and manage window list
  // if not, just start a single window
  if (useTray && trayIcon->isVisible())
    QApplication::setQuitOnLastWindowClosed(false);

  addWindow();
}

void AardviewShim::about(){
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

  QMessageBox::about(0, tr("About Menu"),
                     tr("<h1>About Aardview</h1><br />"
                        "A simple image viewer written by Bernd Wachter. You can visit the <a href=\"http://bwachter.lart.info/projects/aardview/\">project homepage</a> for more information.<br /><br />"
                        "Aardwork has been contributed by prism.<br /><br />"
                        "For bug reports and suggestions please <a href=\"https://mantis.lart.info\">visit my mantis installation</a>."
                        "<h3>Supported formats</h3>"
                        "Reading: %1<br />"
                        "Writing: %2<br />"
#ifdef HAS_EXIF
                        "<p align=\"right\">Extra features: EXIF</p>"
#endif
                       )
                     .arg(supportedReadFormats)
                     .arg(supportedWriteFormats)
    );
}

void AardviewShim::addWindow(){
  QUuid uid=QUuid::createUuid();
  AardView *win = new AardView(uid);

  connect(win, SIGNAL(requestClose(QUuid)), this, SLOT(deleteWindow(QUuid)));
  connect(win, SIGNAL(showAbout()), this, SLOT(about()));

  m_windowModel->addWindow(uid, win);
  win->show();
}

void AardviewShim::createTrayIcon()
{
  trayIconMenu = new QMenu();

  trayMenuWidget = new QWidgetAction(this);
  m_windowListWidget = new QListView();
  m_windowListWidget->setDragDropMode(QAbstractItemView::DragOnly);
  m_windowListWidget->setResizeMode(QListView::Adjust);
  m_windowListWidget->setModel(m_windowModel);
  m_windowListWidget->adjustSize();
  trayMenuWidget->setDefaultWidget(m_windowListWidget);
  connect(m_windowListWidget, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(toggleWindow(const QModelIndex &)));

  trayIconMenu->addAction(trayMenuWidget);
  trayIconMenu->addSeparator();

  actionNewWindow = new QAction(tr("New window"));
  connect(actionNewWindow, SIGNAL(triggered()), this, SLOT(addWindow()));

  actionExit = new QAction(tr("E&xit"));
  connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

  trayIconMenu->addAction(actionNewWindow);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(actionExit);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->setIcon(QIcon(":/images/aardview-icon.png"));
}

void AardviewShim::deleteWindow(QUuid uid){
  if (!m_windowModel->contains(uid)){
    qDebug() << "Request closing of invalid window " << uid;
    return;
  }

  if (useTray && trayIcon->isVisible()){
    AardView *win = m_windowModel->getWindow(uid);
    win->hide();
    qDebug() << "Closing window " << uid;
  } else {
    QApplication::quit();
  }
}

void AardviewShim::toggleWindow(const QModelIndex &index){
  QObject *object = qvariant_cast<QObject*>(m_windowModel->data(index, Qt::UserRole));
  AardView *win = qobject_cast<AardView*>(object);
  win->setVisible(!win->isVisible());
}
