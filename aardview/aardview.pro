#CONFIG += debug
HEADERS = aardview.h \
	imagewidget.h \
	settingsdialog.h \
	tnviewmodel.h
SOURCES = aardview.cpp \
	imagewidget.cpp \
	main.cpp \
	settingsdialog.cpp \
	tnviewmodel.cpp
FORMS = settingsdialog.ui
RESOURCES = aardview.qrc
TARGET = aardview
TEMPLATE = app
LANGUAGE = C++
TRANSLATIONS = aardview_de.ts
PKGCONFIG += libexif
OBJECTS_DIR = ../build
MOC_DIR = ../build
#UI_DIR = ../build

system(pkg-config --exists libexif):DEFINES += EXIF

contains(DEFINES, EXIF){
  QMAKE_CXXFLAGS += $$system(pkg-config --cflags libexif)
  QMAKE_CFLAGS += $$system(pkg-config --cflags libexif)
  LIBS += $$system(pkg-config --libs libexif)
}

unix { 
     target.path = /usr/bin
     INSTALLS += target
}
win32:debug { CONFIG += console }
win32 { RC_FILE = win32.rc }
