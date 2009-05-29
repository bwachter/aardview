#include <QApplication>

#include "aardview.h"

int main(int argc, char** argv){
  //Q_INIT_RESOURCE();
  QApplication app(argc, argv);
  AardView mw;
  mw.show();
  return app.exec();
}
