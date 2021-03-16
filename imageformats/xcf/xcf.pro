OBJECTS_DIR = ../../build
MOC_DIR = ../../build
UI_DIR = ../../build
UI_HEADERS_DIR = ../../build
UI_SOURCES_DIR = ../../build
RCC_DIR = ../../build

DESTDIR = ../../build
CONFIG += static

TARGET = $$qtLibraryTarget(xcf)
CONFIG += qt plugin
HEADERS = gimp_p.h xcf_p.h
SOURCES = xcf.cpp
QT += gui
TEMPLATE = lib
