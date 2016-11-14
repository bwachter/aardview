/**
 * @file aardviewshim.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#include <QMessageBox>
#include <QFileDialog>
#include <QWidgetAction>

#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#include <aprintpreviewdialog.h>
#endif

#include "aardviewshim.h"
#include "aardview.h"
#include "afileinfo.h"

AardviewShim::AardviewShim(const QStringList &arguments){
  qDebug() << "Linked in plugins: " << QPluginLoader::staticInstances();

  m_windowModel = new WindowModel();
  m_settingsDialog=SettingsDialog::instance();

  if (useTray){
    createTrayIcon();
    m_trayIcon->show();
  }

  // if tray icon exists start a first window, and manage window list
  // if not, just start a single window
  if (useTray && m_trayIcon->isVisible())
    QApplication::setQuitOnLastWindowClosed(false);

  connect(m_settingsDialog, SIGNAL(configurationChanged()),
          this, SLOT(reconfigure()));

  reconfigure();

  addWindow(arguments);
}

void AardviewShim::about(){
  QString supportedWriteFormats;
  QString supportedReadFormats;
  QStringList features;

  for (int i = 0; i < QImageReader::supportedImageFormats().count(); ++i){
    supportedReadFormats += " " +
      QString(QImageReader::supportedImageFormats().at(i)).toLower();
  }
  for (int i = 0; i < QImageWriter::supportedImageFormats().count(); ++i){
    supportedWriteFormats += " " +
      QString(QImageWriter::supportedImageFormats().at(i)).toLower();
  }

#ifdef HAS_EXIF
  features.append("exif");
#endif
#ifdef HAS_SSH
  features.append("ssh");
#endif
#ifndef QT_NO_PRINTER
  features.append("printing");
#endif

  QMessageBox::about(0, tr("About Menu"),
                     tr("<h1>About Aardview</h1><br />"
                        "A simple image viewer written by Bernd Wachter. You can visit the <a href=\"http://bwachter.lart.info/projects/aardview/\">project homepage</a> for more information.<br /><br />"
                        "Aardwork has been contributed by prism.<br /><br />"
                        "For bug reports and suggestions please <a href=\"https://mantis.lart.info\">visit my mantis installation</a>."
                        "<h3>Supported formats</h3>"
                        "Reading: %1<br />"
                        "Writing: %2<br />"
                        "<p align=\"right\">Extra features: %3</p>"
                       )
                     .arg(supportedReadFormats)
                     .arg(supportedWriteFormats)
                     .arg(features.join(", "))
    );
}

void AardviewShim::addWindow(const QString &argument){
  QStringList list;

  if (argument == "")
    list.append(QDir::currentPath());
  else
    list.append(argument);

  addWindow(list);
}

void AardviewShim::addWindow(const QStringList &argumentList){
  QUuid uid=QUuid::createUuid();
  SettingsDialog *settings = SettingsDialog::instance();
  QString initialItem;
  AardView *win;

  /**
   * @todo take window uids in account for session restoring,
   *       which requires proper session saving
   */
  if (settings->value("viewer/loadAction").toInt()==1) {
    AFileInfo info(settings->value("viewer/lastImage").toString());
    if (info.exists())
      initialItem = settings->value("viewer/lastImage").toString();
  } else
    initialItem = QDir::currentPath();

  if (argumentList.count() == 1){
    QString argument=argumentList.at(0);
#ifdef DEBUG_OPTIONS
    qDebug() << "Using " << argument << "as argument";
#endif

    AFileInfo info(argument);

    /**
     * @todo once proper switching between single window state and browsing
     *       state exists this should reuse existing windows for windows
     *       with filename in title in browsing state as well, if only
     *       a directory was requested
     */
    if ((win = m_windowModel->getWindow(info.friendlyFilePath())) != 0){
#ifdef DEBUG_WINDOWS
      qDebug() << "Reusing existing window for " << info.absoluteFilePath();
#endif
      win->show();
      return;
    }

    if (info.exists())
      initialItem = info.absoluteFilePath();
  } else {
    qDebug() << "Multiple arguments are currently not supported";
    return;
  }

  win = new AardView(uid, initialItem);

  connect(win, SIGNAL(openEditor(const QString&)),
                      this, SLOT(edit(const QString&)));
  connect(win, SIGNAL(requestClose(QUuid)), this, SLOT(deleteWindow(QUuid)));
  connect(win, SIGNAL(requestDestroy(QUuid, bool)),
          this, SLOT(deleteWindow(QUuid, bool)));
  connect(win, SIGNAL(showOpen()), this, SLOT(open()));
  connect(win, SIGNAL(showOpen(QUuid)), this, SLOT(open(QUuid)));
  connect(win, SIGNAL(showPrint(const QPixmap&)),
          this, SLOT(print(const QPixmap&)));
  connect(win, SIGNAL(showPrintPreview(const QPixmap&)),
          this, SLOT(printPreview(const QPixmap&)));
  connect(win, SIGNAL(showAbout()), this, SLOT(about()));
  connect(win, SIGNAL(showSettings()), m_settingsDialog, SLOT(show()));
  connect(m_settingsDialog, SIGNAL(configurationChanged()),
          win, SLOT(reconfigure()));

  m_windowModel->addWindow(uid, win);

  win->show();
}


void AardviewShim::createTrayIcon()
{
  QMenu *trayIconMenu = new QMenu();

  trayIconMenu->setObjectName("trayIconMenu");
  trayIconMenu->installEventFilter(this);

  QWidgetAction *trayMenuWidget = new QWidgetAction(this);
  m_windowListWidget = new QListView();
  m_windowListWidget->setDragDropMode(QAbstractItemView::DragOnly);
  m_windowListWidget->setResizeMode(QListView::Adjust);
  m_windowListWidget->setModel(m_windowModel);
  m_windowListWidget->adjustSize();

  trayMenuWidget->setDefaultWidget(m_windowListWidget);
  connect(m_windowListWidget, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(toggleWindow(const QModelIndex &)));
  connect(m_windowListWidget, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(toggleWindow(const QModelIndex &)));

  trayIconMenu->addAction(trayMenuWidget);

  QAction *actionNewWindow = new QAction(tr("New window"), 0);
  connect(actionNewWindow, SIGNAL(triggered()), this, SLOT(addWindow()));

  QAction *actionExit = new QAction(tr("E&xit"), 0);
  connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

  QAction *actionSettings = new QAction(tr("Settings"), 0);
  connect(actionSettings, SIGNAL(triggered()), m_settingsDialog, SLOT(show()));

  trayIconMenu->addAction(actionNewWindow);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(actionSettings);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(actionExit);

  m_trayIcon = new QSystemTrayIcon(this);
  m_trayIcon->setContextMenu(trayIconMenu);
  m_trayIcon->setIcon(QIcon(":/images/aardview-icon.png"));
}

void AardviewShim::deleteWindow(QUuid uid, bool force){
  if (!m_windowModel->contains(uid)){
    qDebug() << "Request closing of invalid window " << uid;
    return;
  }

  if (useTray && m_trayIcon->isVisible()){
    AardView *win = m_windowModel->getWindow(uid);
    if (force){
      m_windowModel->deleteWindow(uid);
      return;
    } else {
      win->hide();
#ifdef DEBUG_WINDOWS
      qDebug() << "Closing window " << uid;
#endif
    }
  } else {
    QApplication::quit();
  }
}

bool AardviewShim::eventFilter(QObject *obj, QEvent *event){
  if (obj->objectName() == "trayIconMenu"){
    // disable right-click on tray icon menu
    // this prevents accidentally closing by right-click on exit,
    // but might be too restrictive later on
    if (event->type() == QEvent::MouseButtonPress){
      QMouseEvent *me = dynamic_cast<QMouseEvent*>(event);
      if (me && me->button() == Qt::RightButton){
        me->ignore();
        return true;
      }
    }
  }

  return QObject::eventFilter(obj, event);
}

void AardviewShim::receivedMessage(int instanceId, QByteArray message){
#ifdef DEBUG_INSTANCE
  qDebug() << "Received message from instance: " << instanceId;
  qDebug() << "Message Text: " << message;
#endif

  QList<QByteArray> argumentList = message.split(' ');
  QStringList arguments;

  foreach (const QByteArray arg, argumentList)
    arguments.append(QUrl::fromPercentEncoding(arg));

  addWindow(arguments);
}

void AardviewShim::toggleWindow(const QModelIndex &index){
  QObject *object = qvariant_cast<QObject*>(m_windowModel->data(index, Qt::UserRole));
  AardView *win = qobject_cast<AardView*>(object);
  win->setVisible(!win->isVisible());
}

void AardviewShim::reconfigure(){
  // pixmapcache limit is in KB, we save in MB
  QPixmapCache::setCacheLimit(
    m_settingsDialog->value("main/pixmapCache").toInt() * 1024);
}

void AardviewShim::edit(const QString &filename){
  SettingsDialog *settings = SettingsDialog::instance();

  QString program = settings->value("main/externalEditor").toString();
  if (program !=""){
    qDebug() << "Editing " << filename << " with " << program;
    QStringList arguments;
    arguments << filename;
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
  } else {
    qDebug() << "No editor set, skipping " << filename;
  }
}

void AardviewShim::open(QUuid uid){
  QString fileName =
    QFileDialog::getOpenFileName(0,
                                 tr("Open File"), QDir::currentPath());

  if (!fileName.isEmpty()) {
    if (uid == QUuid())
      addWindow(fileName);
    else {
      AardView *win = m_windowModel->getWindow(uid);
      win->load(fileName);
    }
  }
}

void AardviewShim::paintToPrinter(QPrinter *printer, const QPixmap &pixmap){
#ifndef QT_NO_PRINTER
  QPainter painter(printer);
  QRect rect = painter.viewport();
  QSize size = pixmap.size();

  // if picture is larger than paper scale down, otherwise keep as is
  if (size.height() >= rect.height() ||
      size.width() >= rect.width())
    size.scale(rect.size(), Qt::KeepAspectRatio);

  painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
  painter.setWindow(pixmap.rect());
  painter.drawPixmap(0, 0, pixmap);
#endif
}

void AardviewShim::print(const QPixmap &pixmap){
#ifndef QT_NO_PRINTER
  QPrintDialog dialog(&m_printer);
  if (dialog.exec()) {
    paintToPrinter(&m_printer, pixmap);
  }
#endif
}

void AardviewShim::printPreview(const QPixmap &pixmap){
#ifndef QT_NO_PRINTER
  APrintPreviewDialog dialog(&m_printer, pixmap);
  connect(&dialog,
          SIGNAL(paintRequested(QPrinter *, const QPixmap&)),
          this,
          SLOT(paintToPrinter(QPrinter *, const QPixmap&)));
  dialog.exec();
#endif
}
