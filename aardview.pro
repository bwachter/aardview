TEMPLATE = subdirs
SUBDIRS = kde aardview

lessThan(QT_MAJOR_VERSION, 5){
  lessThan(QT_MINOR_VERSION, 2){
    error("Can't build with Qt version $${QT_VERSION}. Use at least Qt 5.2.")
  }
}
