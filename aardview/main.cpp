#include <QApplication>
#include <singleapplication.h>
#include <QtGui>

#ifdef HAS_SSH
#include <libssh/callbacks.h>
#endif

#include "aardviewshim.h"

// static plugin
Q_IMPORT_PLUGIN(XCFPlugin)
// dynamic plugin might require something like
// QCoreApplication::addLibraryPath("<path>");

int main(int argc, char** argv){
  //Q_INIT_RESOURCE();
  SingleApplication app(argc, argv, true);

  // the secondary instance can't influence the working directory of the app:
  // make sure all arguments are passed through with absolute filenames. Also,
  // if no or invalid arguments are passed, add the instances working directory
  // as argument.
  if (app.isSecondary()){
    qDebug() << "Started new secondary instance";
    QStringList list;

    foreach(const QString arg, app.arguments()){
      QDir dir;
      if (dir.exists(arg)){
        // urlencoding hack to easily pass arguments with spaces
        // without touching singleinstance
        qDebug() << arg << dir.absolutePath() << dir.absoluteFilePath(arg);
        list.append(QUrl::toPercentEncoding(dir.absoluteFilePath(arg).toUtf8()));
      }
    }

    if (list.size() <= 1)
      list.append(QUrl::toPercentEncoding(QDir::currentPath()));

    app.sendMessage(list.join(' ').toUtf8());
    app.exit(0);
    return 0;
  }

  QCoreApplication::setOrganizationName("AardSoft");
  QCoreApplication::setOrganizationDomain("aardsoft.de");
  QCoreApplication::setApplicationName("Aardview");

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator aardviewTranslator;
  aardviewTranslator.load("aardview_" + QLocale::system().name());
  app.installTranslator(&aardviewTranslator);

#ifdef HAS_SSH
  ssh_threads_set_callbacks(ssh_threads_get_pthread());
  ssh_init();
#endif

  AardviewShim mw;
  QObject::connect(&app, &SingleApplication::receivedMessage,
                   &mw, &AardviewShim::receivedMessage);

  return app.exec();
}
