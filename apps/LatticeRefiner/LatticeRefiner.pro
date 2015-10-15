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
        MainWindow.cpp \
    ConstraintsModel.cpp \
    CheckableComboBox.cpp \
    CheckableComboBoxDelegate.cpp

HEADERS  += MainWindow.h \
    ConstraintsModel.h \
    CheckableComboBox.h \
    CheckableComboBoxDelegate.h

FORMS    += MainWindow.ui
