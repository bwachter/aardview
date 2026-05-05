TEMPLATE = subdirs
SUBDIRS = imageformats aardview_widget aardview_qml

aardview_widget.file = aardview/aardview.pro
aardview_qml.file = aardview/aardview-qml.pro
aardview_qml.depends = aardview_widget

lessThan(QT_MAJOR_VERSION, 5){
  lessThan(QT_MINOR_VERSION, 2){
    error("Can't build with Qt version $${QT_VERSION}. Use at least Qt 5.2.")
  }
}

dox.target = doc
dox.commands = doxygen Doxyfile

QMAKE_EXTRA_UNIX_TARGETS += dox

unix {
    desktop.files = desktop/aardview.desktop desktop/aardview-qml.desktop desktop/aardview-default.desktop
    desktop.path = /usr/share/applications
    INSTALLS += desktop

    icon.files = images/aardview.png
    icon.path = /usr/share/pixmaps
    INSTALLS += icon
}
