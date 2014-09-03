TEMPLATE = subdirs
SUBDIRS = aardview

lessThan(QT_MAJOR_VERSION, 5){
  error("Can't build with Qt version $${QT_VERSION}. Use at least Qt 5.0.")
}
