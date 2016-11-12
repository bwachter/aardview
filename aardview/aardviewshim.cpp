/**
 * @file aardviewshim.cpp
 * @copyright 2016
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2016
 */

#include <QMessageBox>
#include "aardviewshim.h"
#include "aardview.h"

AardviewShim::AardviewShim(const QStringList &arguments){
  qDebug() << "Linked in plugins: " << QPluginLoader::staticInstances();

  m_windowModel = new WindowModel();
  m_settingsDialog=SettingsDialog::instance();

  if (useTray){
    createTrayIcon();
    trayIcon->show();
  }

  // if tray icon exists start a first window, and manage window list
  // if not, just start a single window
  if (useTray && trayIcon->isVisible())
    QApplication::setQuitOnLastWindowClosed(false);

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

void AardviewShim::addWindow(const QStringList &argumentList){
  QUuid uid=QUuid::createUuid();
  SettingsDialog *settings = SettingsDialog::instance();
  QString initialItem;

  // TODO: take into account uids
  if (settings->value("viewer/loadAction").toInt()==1) {
    QFileInfo info(settings->value("viewer/lastImage").toString());
    if (info.exists())
      initialItem = settings->value("viewer/lastImage").toString();
  } else
    initialItem = QDir::currentPath();

  if (argumentList.count() == 1){
    QString argument=argumentList.at(0);
    qDebug() << "Using " << argument << "as argument";
    QFileInfo info(argument);

    if (info.exists()){
      // QFileInfo::absolutePath() treats pathnames as files when not ending
      // in /, even though QFileInfo is aware about a path being a directory
      if (info.isDir() && argument.at(argument.size()-1) != '/'){
        initialItem = argument + "/";
      } else {
        initialItem = argument;
      }
    }
  }

  AardView *win = new AardView(uid, initialItem);

  connect(win, SIGNAL(requestClose(QUuid)), this, SLOT(deleteWindow(QUuid)));
  connect(win, SIGNAL(showAbout()), this, SLOT(about()));
  connect(win, SIGNAL(showSettings()), m_settingsDialog, SLOT(show()));
  connect(m_settingsDialog, SIGNAL(configurationChanged()),
          win, SLOT(reconfigure()));

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
  connect(m_windowListWidget, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(toggleWindow(const QModelIndex &)));

  trayIconMenu->addAction(trayMenuWidget);

  QAction *actionNewWindow = new QAction(tr("New window"));
  connect(actionNewWindow, SIGNAL(triggered()), this, SLOT(addWindow()));

  QAction *actionExit = new QAction(tr("E&xit"));
  connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

  QAction *actionSettings = new QAction(tr("Settings"));
  connect(actionSettings, SIGNAL(triggered()), m_settingsDialog, SLOT(show()));

  trayIconMenu->addAction(actionNewWindow);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(actionSettings);
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

void AardviewShim::receivedMessage(int instanceId, QByteArray message){
  qDebug() << "Received message from instance: " << instanceId;
  qDebug() << "Message Text: " << message;

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
