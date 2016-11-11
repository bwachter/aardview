CONFIG += debug link_pkgconfig
HEADERS = aardview.h \
        aardviewshim.h \
        alistview.h \
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

unix {
     system(pkg-config --exists libexif){
            message("Found libexif")
            DEFINES += HAS_EXIF
            PKGCONFIG += libexif
     }
     system(pkg-config --exists libssh){
            message("Found libssh")
            DEFINES += HAS_SSH
            PKGCONFIG += libssh libssh_threads
     }
}

unix {
     target.path = /usr/bin
     INSTALLS += target
     CONFIG += static
}
win32:debug { CONFIG += console }
win32 {
      RC_FILE = win32.rc
      CONFIG += embed_manifest_exe
}
