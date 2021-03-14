/**
 * @file main.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#include <QApplication>
#ifdef USE_SINGLEAPPLICATION
#include <singleapplication.h>
#endif
#include <QtGui>

#ifdef HAS_SSH
#include <libssh/callbacks.h>
#endif

#include "aardviewshim.h"
#include "aardviewlog.h"
#include "settingsdialog.h"
#include "version.h"

// static plugin
Q_IMPORT_PLUGIN(XCFPlugin)
// dynamic plugin might require something like
// QCoreApplication::addLibraryPath("<path>");

int main(int argc, char** argv){
  //Q_INIT_RESOURCE();
#ifdef USE_SINGLEAPPLICATION
  SingleApplication app(argc, argv, true);
#else
  QApplication app(argc, argv);
#endif
  QCommandLineParser parser;

  qInstallMessageHandler(AardviewLog::messageHandler);

  QCoreApplication::setOrganizationName("AardSoft");
  QCoreApplication::setOrganizationDomain("aardsoft.de");
  QCoreApplication::setApplicationName("Aardview");
  QCoreApplication::setApplicationVersion(AARDVIEW_VERSION);

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator aardviewTranslator;
  aardviewTranslator.load("aardview_" + QLocale::system().name());
  app.installTranslator(&aardviewTranslator);
  //AardviewLog::setPriority(10);

  parser.setApplicationDescription(
    QString("\nA simple image viewer\n\n"
            "Version: %1\n"
            "Features: %2\n"
            "Image formats (reading): %3\n"
            "Image formats (writing): %4\n"
      )
    .arg(AARDVIEW_VERSION)
    .arg(SettingsDialog::features().join(", "))
    .arg(SettingsDialog::readFormats().join(", "))
    .arg(SettingsDialog::writeFormats().join(", "))
    );

  parser.addVersionOption();
  parser.addHelpOption();

#ifdef USE_SINGLEAPPLICATION
  QCommandLineOption serviceOption(QStringList() << "s"
                                   << "service",
                                   "Start as service");
  parser.addOption(serviceOption);
#endif

  parser.process(app);

  // the secondary instance can't influence the working directory of the app:
  // make sure all arguments are passed through with absolute filenames. Also,
  // if no or invalid arguments are passed, add the instances working directory
  // as argument.
  //
  // the same filtering is applied to main instance arguments as well, just in
  // case.
  QStringList absoluteArguments, optionArguments;
  foreach(const QString arg, parser.optionNames()){
    optionArguments.append(arg);
  }

  foreach(const QString arg, parser.positionalArguments()){
    QDir dir;
    if (dir.exists(arg)){
      // urlencoding hack to easily pass arguments with spaces
      // without touching singleinstance
#ifdef USE_SINGLEAPPLICATION
      if (app.isSecondary())
        absoluteArguments.append(QUrl::toPercentEncoding(
                                   dir.absoluteFilePath(arg).toUtf8()));
      else
#endif
        absoluteArguments.append(dir.absoluteFilePath(arg).toUtf8());
    }
  }

  if (absoluteArguments.size() <= 0){
#ifdef USE_SINGLEAPPLICATION
    if (app.isSecondary())
      absoluteArguments.append(QUrl::toPercentEncoding(QDir::currentPath()));
    else
#endif
      absoluteArguments.append(QDir::currentPath());
  }

#ifdef USE_SINGLEAPPLICATION
  if (app.isSecondary()){
#ifdef DEBUG_INSTANCE
    qDebug() << "Started new secondary instance";
#endif

    app.sendMessage(absoluteArguments.join(' ').toUtf8());
    app.exit(0);
    return 0;
  }
#endif

#ifdef HAS_SSH
  ssh_threads_set_callbacks(ssh_threads_get_pthread());
  ssh_init();
#endif

  AardviewShim mw(absoluteArguments, optionArguments);
#ifdef USE_SINGLEAPPLICATION
  QObject::connect(&app, &SingleApplication::receivedMessage,
                   &mw, &AardviewShim::receivedMessage);
#endif

  return app.exec();
}
