CONFIG += debug link_pkgconfig
HEADERS = aardview.h \
        aardviewshim.h \
        aardviewlog.h \
        afileinfo.h \
        alistview.h \
        aprintpreviewdialog.h \
        atreeview.h \
        exifviewmodel.h \
        imageloader.h \
        settingsdialog.h \
        windowmodel.h \
        tnviewmodel.h \
        version.h
SOURCES = aardview.cpp \
        aardviewshim.cpp \
        aardviewlog.cpp \
        imageloader.cpp \
        exifviewmodel.cpp \
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

unix:android {
}

unix:!android {
     include(../singleapplication/singleapplication.pri)
     DEFINES += USE_SINGLEAPPLICATION=1 QAPPLICATION_CLASS=QApplication
     PRE_TARGETDEPS = version-dummy
     QMAKE_EXTRA_TARGETS += version-dummy
     version-dummy.target = version-dummy
     version-dummy.commands = ../write-version-header.sh

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
     system(pkg-config --exists libsystemd){
            message("Found libsystemd")
            DEFINES += HAS_SYSTEMD
            PKGCONFIG += libsystemd
     }
}

unix {
     target.path = /usr/bin
     INSTALLS += target
     CONFIG += static
}
win32:debug { CONFIG += console }
win32 {
     include(../singleapplication/singleapplication.pri)
     DEFINES += USE_SINGLEAPPLICATION=1
     LIBS += -lAdvapi32
     RC_FILE = win32.rc
     CONFIG += embed_manifest_exe
}
