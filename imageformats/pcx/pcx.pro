OBJECTS_DIR = ../../build
MOC_DIR = ../../build
UI_DIR = ../../build
UI_HEADERS_DIR = ../../build
UI_SOURCES_DIR = ../../build
RCC_DIR = ../../build

DESTDIR = ../../build
CONFIG += static

TARGET = $$qtLibraryTarget(pcx)
CONFIG += qt plugin
HEADERS = pcx_p.h
SOURCES = pcx.cpp
QT += gui
TEMPLATE = lib
