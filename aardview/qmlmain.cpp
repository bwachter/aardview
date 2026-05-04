/**
 * @file qmlmain.cpp
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2026
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "qmlcontext.h"
#include "qmlimageprovider.h"
#include "settingsdialog.h"
#include "aardviewlog.h"
#include "version.h"
#include "tnviewmodel.h"
#include "thumbnailfilesystemmodel.h"
#include "exifviewmodel.h"

#ifdef HAS_SSH
#include <libssh/callbacks.h>
#endif

Q_IMPORT_PLUGIN(ANIPlugin)
Q_IMPORT_PLUGIN(PCXPlugin)
Q_IMPORT_PLUGIN(PSDPlugin)
Q_IMPORT_PLUGIN(XCFPlugin)

int main(int argc, char** argv){
  QApplication app(argc, argv);

  qInstallMessageHandler(AardviewLog::messageHandler);

  QCoreApplication::setOrganizationName("Aardsoft");
  QCoreApplication::setOrganizationDomain("aardsoft.fi");
  QCoreApplication::setApplicationName("Aardview");
  QCoreApplication::setApplicationVersion(AARDVIEW_VERSION);

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::path(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator aardviewTranslator;
  aardviewTranslator.load("aardview_" + QLocale::system().name());
  app.installTranslator(&aardviewTranslator);

#ifdef HAS_SSH
  ssh_threads_set_callbacks(ssh_threads_get_pthread());
  ssh_init();
#endif

  QQmlApplicationEngine engine;

  // Register types for QML
  qmlRegisterUncreatableType<TnViewModel>("Aardview", 1, 0, "TnViewModel",
      "Created from C++");
  qmlRegisterUncreatableType<ThumbnailFileSystemModel>("Aardview", 1, 0, "ThumbnailFileSystemModel",
      "Created from C++");
  qmlRegisterUncreatableType<ExifViewModel>("Aardview", 1, 0, "ExifViewModel",
      "Created from C++");

  // Create context object and image provider
  QmlContext *context = new QmlContext(&engine);
  engine.rootContext()->setContextProperty("context", context);
  engine.addImageProvider("aardview", new QmlImageProvider());

  // Connect settings changes
  QObject::connect(SettingsDialog::instance(), &SettingsDialog::configurationChanged,
    context, &QmlContext::reconfigure);

  const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
  QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreated,
    &app, [url](QObject *obj, const QUrl &objUrl) {
      if (!obj && url == objUrl)
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

  engine.load(url);

  // Handle initial path from command line
  QStringList args = app.arguments();
  if (args.size() > 1) {
    QString arg = args.at(1);
    if (QFile::exists(arg) || QDir(arg).exists()) {
      context->loadFile(arg);
    }
  } else {
    // Default startup path
    QString initialPath = QDir::homePath();
    QString cwd = QDir::currentPath();
    if (cwd != "/" && !cwd.startsWith("/System/") && !cwd.startsWith("/usr/") &&
        !cwd.startsWith("/bin") && !cwd.startsWith("/sbin") &&
        !cwd.startsWith("/Applications/") && !cwd.startsWith("/Library/"))
      initialPath = cwd;
    context->loadFile(initialPath);
  }

  return app.exec();
}
