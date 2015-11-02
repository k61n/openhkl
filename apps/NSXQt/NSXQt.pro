#-------------------------------------------------
#
# Project created by QtCreator 2014-07-23T10:58:01
#
#-------------------------------------------------

QT       += core widgets printsupport opengl

CONFIG   += debug_and_release

QMAKE_CXXFLAGS += -DNDEBUG -DEIGEN_FFTW_DEFAULT

unix:!macx {
    QMAKE_CXXFLAGS += -std=c++0x -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

macx: {
    CONFIG += c++11
    QMAKE_CXXFLAGS += -std=c++11
}

win32: {
}

TARGET = nsxtool

TEMPLATE = app

RESOURCES = NSXQt.qrc

SOURCES += src/Main.cpp \
    src/MainWindow.cpp \
    src/ColorMap.cpp \
    src/PeakTableView.cpp \
    src/DialogTransformationMatrix.cpp \
    src/DetectorGraphicsView.cpp \
    src/DetectorScene.cpp \
    src/DialogExperiment.cpp \
    src/Logger.cpp \
    src/NoteBook.cpp \
    src/Absorption/AbsorptionDialog.cpp \
    src/Absorption/CrystalScene.cpp \
    src/Absorption/CrystalFaceItem.cpp \
    src/Absorption/CrystalNodeItem.cpp \
    src/Absorption/PinItem.cpp \
    src/Absorption/RulerItem.cpp \
    src/Absorption/CalibrateDistanceDialog.cpp \
    src/Chemistry/IsotopeDatabaseDialog.cpp \
    src/Chemistry/DragElementModel.cpp \
    src/Chemistry/NewElementTableView.cpp \
    src/Chemistry/IsotopesListWidget.cpp \
    src/Chemistry/ElementsListWidget.cpp \
    src/Chemistry/ElementManagerDialog.cpp \
    src/Externals/qcustomplot.cpp \
    src/Plot/PeakPlot.cpp \
    src/Plot/PlotFactory.cpp \
    src/Plot/SimplePlot.cpp \
    src/Plot/SXPlot.cpp \
    src/GraphicsItems/CutLineGraphicsItem.cpp \
    src/GraphicsItems/CutSliceGraphicsItem.cpp \
    src/GraphicsItems/CutterGraphicsItem.cpp \
    src/GraphicsItems/MaskGraphicsItem.cpp \
    src/GraphicsItems/PeakGraphicsItem.cpp \
    src/GraphicsItems/PlottableGraphicsItem.cpp \
    src/GraphicsItems/SXGraphicsItem.cpp \
    src/Tree/DataItem.cpp \
    src/Tree/DetectorItem.cpp \
    src/Tree/DetectorPropertyWidget.cpp \
    src/Tree/EmptyPropertyWidget.cpp \
    src/Tree/ExperimentItem.cpp \
    src/Tree/ExperimentTree.cpp \
    src/Tree/InspectableTreeItem.cpp \
    src/Tree/InstrumentItem.cpp \
    src/Tree/NumorItem.cpp \
    src/Tree/NumorPropertyWidget.cpp \
    src/Tree/PeakListItem.cpp \
    src/Tree/SampleItem.cpp \
    src/Tree/SamplePropertyWidget.cpp \
    src/Tree/SampleShapeItem.cpp \
    src/Tree/SampleShapePropertyWidget.cpp \
    src/Tree/SourceItem.cpp \
    src/Tree/TreeItem.cpp \
    src/Tree/UnitCellItem.cpp \
    src/Tree/UnitCellPropertyWidget.cpp \
    src/GraphicsItems/SliceRectGraphicsItem.cpp \
    src/DialogFindUnitCell.cpp \
    src/Tree/SourcePropertyWidget.cpp \
    src/DialogRefineUnitCell.cpp \
    src/Tree/NormalizePeaksItem.cpp \
    src/NumorsConversionDialog.cpp \
    src/Absorption/DialogMCAbsorption.cpp \
    src/GraphicsItems/PeakCalcGraphicsItem.cpp \
    src/DoubleTableItemDelegate.cpp \
    src/Tree/PeakListPropertyWidget.cpp

HEADERS  += include/MainWindow.h \
    include/DialogPeakFind.h \
    include/ColorMap.h \
    include/Externals/qcustomplot.h \
    include/PeakTableView.h \
    include/DialogTransformationMatrix.h \
    include/NoteBook.h \
    include/Logger.h \
    include/DialogExperiment.h \
    include/DetectorGraphicsView.h \
    include/DetectorScene.h \
    include/Absorption/AbsorptionDialog.h \
    include/Absorption/CrystalScene.h \
    include/Absorption/CrystalFaceItem.h \
    include/Absorption/CrystalNodeItem.h \
    include/Absorption/PinItem.h \
    include/Absorption/RulerItem.h \
    include/Absorption/CalibrateDistanceDialog.h \
    include/Chemistry/IsotopeDatabaseDialog.h \
    include/Chemistry/DragElementModel.h \
    include/Chemistry/NewElementTableView.h \
    include/Chemistry/IsotopesListWidget.h \
    include/Chemistry/ElementsListWidget.h \
    include/Chemistry/ElementManagerDialog.h \
    include/GraphicsItems/CutLineGraphicsItem.h \
    include/GraphicsItems/CutSliceGraphicsItem.h \
    include/GraphicsItems/CutterGraphicsItem.h \
    include/GraphicsItems/MaskGraphicsItem.h \
    include/GraphicsItems/PeakGraphicsItem.h \
    include/GraphicsItems/PlottableGraphicsItem.h \
    include/GraphicsItems/SXGraphicsItem.h \
    include/Plot/PeakPlot.h \
    include/Plot/PlotFactory.h \
    include/Plot/SimplePlot.h \
    include/Plot/SXPlot.h \
    include/Tree/DataItem.h \
    include/Tree/DetectorItem.h \
    include/Tree/DetectorPropertyWidget.h \
    include/Tree/EmptyPropertyWidget.h \
    include/Tree/ExperimentItem.h \
    include/Tree/ExperimentTree.h \
    include/Tree/InspectableTreeItem.h \
    include/Tree/InstrumentItem.h \
    include/Tree/NumorItem.h \
    include/Tree/NumorPropertyWidget.h \
    include/Tree/PeakListItem.h \
    include/Tree/SampleItem.h \
    include/Tree/SamplePropertyWidget.h \
    include/Tree/SampleShapeItem.h \
    include/Tree/SampleShapePropertyWidget.h \
    include/Tree/SourceItem.h \
    include/Tree/TreeItem.h \
    include/Tree/UnitCellItem.h \
    include/Tree/UnitCellPropertyWidget.h \
    include/GraphicsItems/SliceRectGraphicsItem.h \
    include/DialogFindUnitCell.h \
    include/Tree/SourcePropertyWidget.h \
    include/DialogRefineUnitCell.h \
    include/Tree/NormalizePeaksItem.h \
    include/NumorsConversionDialog.h \
    include/Absorption/DialogMCAbsorption.h \
    include/GraphicsItems/PeakCalcGraphicsItem.h \
    include/DoubleTableItemDelegate.h \
    include/Tree/PeakListPropertyWidget.h

FORMS    += ui/mainwindow.ui \
    ui/dialog_PeakFind.ui \
    ui/dialogtransformationmatrix.ui \
    ui/DialogExperiment.ui \
    ui/Absorption/AbsorptionDialog.ui \
    ui/Absorption/CalibrateDistanceDialog.ui \
    ui/Chemistry/IsotopeDatabaseDialog.ui \
    ui/Chemistry/ElementManagerDialog.ui \
    ui/Tree/DetectorPropertyWidget.ui \
    ui/Tree/EmptyPropertyWidget.ui \
    ui/Tree/NumorPropertyWidget.ui \
    ui/Tree/SamplePropertyWidget.ui \
    ui/Tree/SampleShapePropertyWidget.ui \
    ui/Tree/UnitCellPropertyWidget.ui \
    ui/DialogFindUnitCell.ui \
    ui/Tree/SourcePropertyWidget.ui \
    ui/DialogRefineUnitCell.ui \
    ui/NumorsConversionDialog.ui \
    ui/Absorption/DialogMCAbsorption.ui \
    ui/Tree/PeakListPropertyWidget.ui

win32: {
    CONFIG(release, debug|release): LIBS += -lNSXTool
}
else:unix: LIBS += -lNSXTool -lboost_date_time -lboost_system -lfftw3

unix:
{
INCLUDEPATH += $$PWD include externals/include
INCLUDEPATH += /usr/local/include/NSXTool /usr/include/eigen3
}
