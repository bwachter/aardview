/**
 * @file main.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

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
  QCommandLineParser parser;

  parser.parse(app.arguments());

  // the secondary instance can't influence the working directory of the app:
  // make sure all arguments are passed through with absolute filenames. Also,
  // if no or invalid arguments are passed, add the instances working directory
  // as argument.
  //
  // the same filtering is applied to main instance arguments as well, just in
  // case.
  QStringList absoluteArguments;
  foreach(const QString arg, parser.positionalArguments()){
    QDir dir;
    if (dir.exists(arg)){
      // urlencoding hack to easily pass arguments with spaces
      // without touching singleinstance
      if (app.isSecondary())
        absoluteArguments.append(QUrl::toPercentEncoding(
                                   dir.absoluteFilePath(arg).toUtf8()));
      else
        absoluteArguments.append(dir.absoluteFilePath(arg).toUtf8());
    }
  }

  if (absoluteArguments.size() <= 0){
    if (app.isSecondary())
      absoluteArguments.append(QUrl::toPercentEncoding(QDir::currentPath()));
    else
      absoluteArguments.append(QDir::currentPath());
  }

  if (app.isSecondary()){
#ifdef DEBUG_INSTANCE
    qDebug() << "Started new secondary instance";
#endif

    app.sendMessage(absoluteArguments.join(' ').toUtf8());
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

  AardviewShim mw(absoluteArguments);
  QObject::connect(&app, &SingleApplication::receivedMessage,
                   &mw, &AardviewShim::receivedMessage);

  return app.exec();
}
