TEMPLATE = subdirs
SUBDIRS = aardview

contains(QT_VERSION, ^4\.[0-3]\..*) {
  error("Can't build with Qt version $${QT_VERSION}. Use at least Qt 4.4.")
}