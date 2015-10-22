#-------------------------------------------------
#
# Project created by QtCreator 2015-10-13T11:19:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++0x -fopenmp

TARGET = LatticeRefiner
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/release/ -lNSXTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/debug/ -lNSXTool
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lNSXTool

INCLUDEPATH += $$PWD/../../../../../../usr/local/include/eigen3

INCLUDEPATH += $$PWD/../../../../../../usr/local/include/NSXTool
INCLUDEPATH += $$PWD/../../../../../../usr/include/eigen3
DEPENDPATH += $$PWD/../../../../../../usr/local/include/NSXTool

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/release/ -lNSXTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/debug/ -lNSXTool
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lNSXTool

INCLUDEPATH += $$PWD/../../../../../../usr/local/include/NSXTool
DEPENDPATH += $$PWD/../../../../../../usr/local/include/NSXTool
