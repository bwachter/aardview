# Some debug flags are available to configure debug output at build time:
#
# DEBUG_FILTERS: print info about file/directory filtering
# DEBUG_GESTURES: print gesture notifications
# DEBUG_INSTANCE: print messages about multi instance handling
# DEBUG_LAYOUT: print some widget sizes
# DEBUG_MODEL: print some info related to file/dir models and their proxy
# DEBUG_OPTIONS: print program options and arguments
# DEBUG_SETTINGS: print some info about applying/reloading settings
# DEBUG_WINDOWS: print debug output related to window handling


CONFIG += debug link_pkgconfig
HEADERS = aardview.h \
        aardviewshim.h \
        afileinfo.h \
        alistview.h \
        aprintpreviewdialog.h \
        atreeview.h \
        adirmodel.h \
        imageloader.h \
        settingsdialog.h \
        windowmodel.h \
        tnviewmodel.h
SOURCES = aardview.cpp \
        aardviewshim.cpp \
        imageloader.cpp \
        main.cpp \
        settingsdialog.cpp \
        windowmodel.cpp \
        tnviewmodel.cpp
FORMS = aardview.ui \
      settingsdialog.ui
RESOURCES = aardview.qrc
TARGET = aardview
TEMPLATE = app
LANGUAGE = C++
TRANSLATIONS = aardview_de.ts
OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
UI_HEADERS_DIR = ../build
UI_SOURCES_DIR = ../build
RCC_DIR = ../build
DESTDIR = ../build
QT += widgets printsupport
#CONFIG += plugin
LIBS += -L../build -lkde
QTPLUGIN += kde

include(../singleapplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication DEBUG_GESTURES=1

unix {
     system(pkg-config --exists libexif){
            message("Found libexif")
            DEFINES += HAS_EXIF
            PKGCONFIG += libexif
     }
     system(pkg-config --exists libssh){
            message("Found libssh")
            DEFINES += HAS_SSH
            PKGCONFIG += libssh
     }
     # libssh before 0.8 has a separate threads library
     system(pkg-config --exists libssh_threads){
            message("Found libssh_threads")
            DEFINES += HAS_SSH
            PKGCONFIG += libssh_threads
     }
}

unix {
     target.path = /usr/bin
     INSTALLS += target
     CONFIG += static
}
win32:debug { CONFIG += console }
win32 {
      LIBS += -lAdvapi32
      RC_FILE = win32.rc
      CONFIG += embed_manifest_exe
}
