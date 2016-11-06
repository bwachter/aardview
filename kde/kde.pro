OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
UI_HEADERS_DIR = ../build
UI_SOURCES_DIR = ../build
RCC_DIR = ../build

# dynamic plugin
#DESTDIR = ../build/imageformats
# static plugin
DESTDIR = ../build
CONFIG += static

CONFIG += debug
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
TARGET = $$qtLibraryTarget(kde)
CONFIG += qt plugin
HEADERS = gimp.h xcf.h
SOURCES = xcf.cpp
QT += widgets gui
TEMPLATE = lib
