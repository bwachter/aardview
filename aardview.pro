CONFIG += uitools
CONFIG += debug
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

win32:debug { CONFIG += console }
win32 { RC_FILE = win32.rc }
