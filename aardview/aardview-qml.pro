CONFIG += debug link_pkgconfig

HEADERS = qmlcontext.h \
        qmlimageprovider.h \
        afileinfo.h \
        exifviewmodel.h \
        imageloader.h \
        localthumbnailprovider.h \
        settingsdialog.h \
        thumbnailfilesystemmodel.h \
        thumbnailprovider.h \
        tnviewmodel.h \
        aardviewlog.h \
        version.h

SOURCES = qmlmain.cpp \
        qmlcontext.cpp \
        qmlimageprovider.cpp \
        imageloader.cpp \
        exifviewmodel.cpp \
        localthumbnailprovider.cpp \
        settingsdialog.cpp \
        thumbnailfilesystemmodel.cpp \
        tnviewmodel.cpp \
        aardviewlog.cpp

RESOURCES = aardview.qrc \
            qml.qrc

TARGET = aardview-qml
TEMPLATE = app
LANGUAGE = C++
TRANSLATIONS = aardview_de.ts
OBJECTS_DIR = ../build
MOC_DIR = ../build
RCC_DIR = ../build
DESTDIR = ../build

QT += quick quickcontrols2 multimedia concurrent widgets printsupport
CONFIG += c++17

LIBS += -L../build -lani -lpcx -lpsd -lxcf
QTPLUGIN += ani pcx psd xcf

unix:!android {
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

macx {
     ICON = ../images/aardview.icns
}
