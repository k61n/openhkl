#-------------------------------------------------
#
# Project created by QtCreator 2014-07-23T10:58:01
#
#-------------------------------------------------

QT       += core widgets printsupport opengl
CONFIG   += debug_and_release
QMAKE_CXXFLAGS += -std=c++0x -fopenmp -DNDEBUG -DEIGEN_FFTW_DEFAULT -g
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
    src/CutSliceGraphicsItem.cpp \
    src/SXPlot.cpp \
    src/PlottableGraphicsItem.cpp \
    src/CutLineGraphicsItem.cpp \
    src/CutterGraphicsItem.cpp \
    src/PlotFactory.cpp \
    src/SimplePlot.cpp \
    src/MaskGraphicsItem.cpp \
    src/SXGraphicsItem.cpp \
    src/PeakPlot.cpp \
    src/Absorption/AbsorptionDialog.cpp \
    src/Absorption/CrystalScene.cpp \
    src/Absorption/CrystalFaceItem.cpp \
    src/Absorption/CrystalNodeItem.cpp \
    src/Absorption/PinItem.cpp \
    src/Absorption/RulerItem.cpp \
    src/Absorption/CalibrateDistanceDialog.cpp

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
    include/CutSliceGraphicsItem.h \
    include/PlottableGraphicsItem.h \
    include/CutLineGraphicsItem.h \
    include/CutterGraphicsItem.h \
    include/PlotFactory.h \
    include/SXPlot.h \
    include/MaskGraphicsItem.h \
    include/SXGraphicsItem.h \
    include/SimplePlot.h \
    include/PeakPlot.h \
    include/Absorption/AbsorptionDialog.h \
    include/Absorption/CrystalScene.h \
    include/Absorption/CrystalFaceItem.h \
    include/Absorption/CrystalNodeItem.h \
    include/Absorption/PinItem.h \
    include/Absorption/RulerItem.h \
    include/Absorption/CalibrateDistanceDialog.h

FORMS    += ui/mainwindow.ui \
    ui/dialog_PeakFind.ui \
    ui/dialog_UnitCell.ui \
    ui/dialogtransformationmatrix.ui \
    ui/dialogunitcellsolutions.ui \
    ui/DialogExperiment.ui \
    ui/Absorption/AbsorptionDialog.ui \
    ui/Absorption/CalibrateDistanceDialog.ui

win32:CONFIG(release, debug|release): LIBS += -lNSXTool
else:unix: LIBS += -lNSXTool -lboost_date_time -lfftw3

INCLUDEPATH += $$PWD include externals/include

