OBJECTS_DIR = ../../build
MOC_DIR = ../../build
UI_DIR = ../../build
UI_HEADERS_DIR = ../../build
UI_SOURCES_DIR = ../../build
RCC_DIR = ../../build

DESTDIR = ../../build
CONFIG += static

TARGET = $$qtLibraryTarget(ani)
CONFIG += qt plugin
HEADERS = ani_p.h
SOURCES = ani.cpp
QT += gui
TEMPLATE = lib
