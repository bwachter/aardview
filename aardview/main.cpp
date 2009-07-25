#include <QApplication>

#include "aardview.h"

int main(int argc, char** argv){
  if (argc != 1) 
    qDebug() << "Aardview currently has no support for commandline arguments\n" 
             << "Ignored " << argc << " arguments";
    
      
  //Q_INIT_RESOURCE();
  QApplication app(argc, argv);
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

  AardView mw;
  mw.show();
  return app.exec();
}
