OBJECTS_DIR = ../../build
MOC_DIR = ../../build
UI_DIR = ../../build
UI_HEADERS_DIR = ../../build
UI_SOURCES_DIR = ../../build
RCC_DIR = ../../build

DESTDIR = ../../build
CONFIG += static

TARGET = $$qtLibraryTarget(psd)
CONFIG += qt plugin
HEADERS = ../rle_p.h psd_p.h
SOURCES = psd.cpp
QT += gui
TEMPLATE = lib
