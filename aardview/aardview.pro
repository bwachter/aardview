#CONFIG += debug
HEADERS = aardview.h \
        alistview.h \
        atreeview.h \
        imagewidget.h \
        settingsdialog.h \
        tnviewmodel.h
SOURCES = aardview.cpp \
        imagewidget.cpp \
        main.cpp \
        settingsdialog.cpp \
        tnviewmodel.cpp
FORMS = aardview.ui \
      imagewidget.ui \
      settingsdialog.ui
RESOURCES = aardview.qrc
TARGET = aardview
TEMPLATE = app
LANGUAGE = C++
TRANSLATIONS = aardview_de.ts
PKGCONFIG += libexif
OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
UI_HEADERS_DIR = ../build
UI_SOURCES_DIR = ../build
RCC_DIR = ../build
DESTDIR = ../build

system(pkg-config --exists libexif):DEFINES += EXIF

contains(DEFINES, EXIF){
  QMAKE_CXXFLAGS += $$system(pkg-config --cflags libexif)
  QMAKE_CFLAGS += $$system(pkg-config --cflags libexif)
  LIBS += $$system(pkg-config --libs libexif)
}

contains(QT_VERSION, ^4\.6\..*) {
  message("Found Qt $${QT_VERSION}, enabling Qt 4.6 features")
  DEFINES += QT46
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
