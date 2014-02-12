#-------------------------------------------------
#
# Project created by QtCreator 2013-12-02T15:10:58
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++0x -fopenmp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = D19plotter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/NSXTool/release/ -lNSXTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/NSXTool/debug/ -lNSXTool
else:unix: LIBS += -L/usr/local/NSXTool/ -lNSXTool -lgomp

INCLUDEPATH += $$PWD/ /usr/local/include/NSXTool
DEPENDPATH += $$PWD/ /usr/local/include/NSXTool
