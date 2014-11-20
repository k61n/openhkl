#-------------------------------------------------
#
# Project created by QtCreator 2014-07-23T10:58:01
#
#-------------------------------------------------

QT       += core widgets printsupport opengl
CONFIG   += debug_and_release
QMAKE_CXXFLAGS += -std=c++0x -fopenmp -DNDEBUG -DEIGEN_FFTW_DEFAULT
QMAKE_LFLAGS += -fopenmp
TARGET = nsxtool
TEMPLATE = app

RESOURCES = NSXQt.qrc \

SOURCES += src/Main.cpp \
    src/MainWindow.cpp \
    src/ColorMap.cpp \
    externals/src/qcustomplot.cpp \
    src/PeakTableView.cpp \
    src/DialogTransformationMatrix.cpp \
    src/DialogUnitCell.cpp \
    src/NoteBook.cpp \
    src/Logger.cpp \
    src/DialogUnitCellSolutions.cpp \
    src/ExperimentTree.cpp \
    src/DialogExperiment.cpp \
    src/TreeItem.cpp \
    src/DataItem.cpp \
    src/DetectorItem.cpp \
    src/ExperimentItem.cpp \
    src/InstrumentItem.cpp \
    src/NumorItem.cpp \
    src/PeakListItem.cpp \
    src/SampleItem.cpp \
    src/SourceItem.cpp \
    src/DetectorGraphicsView.cpp \
    src/DetectorScene.cpp \
    src/PeakGraphicsItem.cpp \
    src/SliceGraphicsItem.cpp \
    src/SliceCutterCustomPlot.cpp \
    src/CutterCustomPlot.cpp \
    src/LineCutterCustomPlot.cpp \
    src/PeakCustomPlot.cpp \
    src/SXCustomPlot.cpp \
    src/PlottableGraphicsItem.cpp \
    src/LineCutGraphicsItem.cpp \
    src/CutterGraphicsItem.cpp

HEADERS  += include/MainWindow.h \
    include/DialogPeakFind.h \
    include/ColorMap.h \
    externals/include/qcustomplot.h \
    include/PeakTableView.h \
    include/DialogUnitCell.h \
    include/DialogTransformationMatrix.h \
    include/NoteBook.h \
    include/Logger.h \
    include/DialogUnitCellSolutions.h \
    include/ExperimentTree.h \
    include/DialogExperiment.h \
    include/TreeItem.h \
    include/DataItem.h \
    include/DetectorItem.h \
    include/ExperimentItem.h \
    include/InstrumentItem.h \
    include/NumorItem.h \
    include/PeakListItem.h \
    include/SampleItem.h \
    include/SourceItem.h \
    include/DetectorGraphicsView.h \
    include/DetectorScene.h \
    include/PeakGraphicsItem.h \
    include/SliceGraphicsItem.h \
    include/SliceCutterCustomPlot.h \
    include/CutterCustomPlot.h \
    include/LineCutterCustomPlot.h \
    include/PeakCustomPlot.h \
    include/SXCustomPlot.h \
    include/PlottableGraphicsItem.h \
    include/LineCutGraphicsItem.h \
    include/CutterGraphicsItem.h


FORMS    += ui/mainwindow.ui \
    ui/dialog_PeakFind.ui \
    ui/dialog_UnitCell.ui \
    ui/dialogtransformationmatrix.ui \
    ui/dialogunitcellsolutions.ui \
    ui/DialogExperiment.ui

win32:CONFIG(release, debug|release): LIBS += -lNSXTool
else:unix: LIBS += -lNSXTool -lboost_date_time -lfftw3

INCLUDEPATH += $$PWD include externals/include

