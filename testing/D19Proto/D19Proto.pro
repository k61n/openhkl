#-------------------------------------------------
#
# Project created by QtCreator 2014-07-23T10:58:01
#
#-------------------------------------------------

QT       += core widgets printsupport
CONFIG   += debug_and_release
QMAKE_CXXFLAGS += -std=c++0x -fopenmp
TARGET = D19Proto2
TEMPLATE = app


RESOURCES = D19Proto.qrc \
    D19Proto.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    Data.cpp \
    DetectorView.cpp \
    ColorMap.cpp \
    qcustomplot.cpp \
    Plotter1D.cpp \
    PeakPlotter.cpp \
    slicerect.cpp \
    PeakTableView.cpp \
    Logger.cpp

HEADERS  += mainwindow.h \
    DetectorView.h \
    DialogPeakFind.h \
    Data.h \
    Logger.h \
    ColorMap.h \
    qcustomplot.h \
    Plotter1D.h \
    PeakPlotter.h \
    slicerect.h \
    DialogProgress.h \
    PeakTableView.h \
    DialogUnitCell.h

FORMS    += mainwindow.ui \
    dialog_PeakFind.ui \
    plotter1d.ui \
    peakplotter.ui \
    dialog_UnitCell.ui \
    dialog_Progress.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/NSXTool/ -lNSXTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/NSXTool/ -lNSXTool
else:unix: LIBS += -L/usr/local/NSXTool/ -lNSXTool -lgomp -lboost_date_time

INCLUDEPATH += $$PWD/ /usr/local/include/NSXTool
INCLUDEPATH += $$PWD/ /usr/local/include/eigen3
DEPENDPATH += $$PWD/ /usr/local/include/NSXTool


INCLUDEPATH += $$PWD/../../../../../../usr/include/eigen3
DEPENDPATH += $$PWD/../../../../../../usr/include/eigen3

