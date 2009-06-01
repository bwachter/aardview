CONFIG += uitools
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

win32 {
RC_FILE = win32.rc
}