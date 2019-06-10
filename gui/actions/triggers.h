//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/triggers.h
//! @brief     Defines class Actions
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_ACTIONS_TRIGGERS_H
#define GUI_ACTIONS_TRIGGERS_H

#include <QCR/widgets/actions.h>

//! Collection of trigger actions, for use as member of MainWin.

class Actions {
public:
    Actions();
    // Files:
    QcrTrigger addExperiment {"add Experiment", "add Experiment"};
    QcrTrigger removeExperiment {"remove Experiment", "remove Experiment"};
    QcrTrigger quit {"quit", "quit"};
    // Experiment/Data:
    QcrTrigger convertHDF5 {"convert", "convert to HDF5"};
    QcrTrigger dataProperties {"data properties", "properties"};
    QcrTrigger findPeaks {"find peaks", "find peaks"};
    QcrTrigger importRaw {"import raw", "import raw data"};
    QcrTrigger instrumentStates {"instrument states", "instrument states dialog"};
    QcrTrigger loadData {"load data", "load data"};
    QcrTrigger removeData {"remove data", "remove data"};
    // Experiment/Instrument/Detector:
    QcrTrigger detectorProperties {"detector properties", "properties"};
    QcrTrigger goniometer {"detector goniometer", "goniometer global offsets"};
    // Experiment/Instrument/Sample:
    QcrTrigger isotopesDatabase {"isotopes database", "open isotopes database"};
    QcrTrigger sampleGoniometer {"sample goniometer", "goniometer global offsets"};
    QcrTrigger sampleProperties {"sample properties", "properties"};
    QcrTrigger shapeProperties {"shape properties", "properties"};
    QcrTrigger shapeLoadMovie {"crystal movie", "load crystal movie"};

    QcrTrigger monochromaticSourceProperties {"monochramtic properties", "properties"};
    // Experiments/Peaks:
    QcrTrigger buildShapeLibrary {"build shape library", "build shape library"};
    QcrTrigger correctAbsorption {"correct", "correct for absorption"};
    QcrTrigger filterPeaks {"filter peaks", "filter peaks"};
    QcrTrigger integratepeaks {"integrate peaks", "integrate peaks"};
    QcrTrigger normalize {"normalize", "normalize"};
    QcrTrigger refine {"refine", "refine lattice and instrument parameters"};
    QcrTrigger show3d {"show 3d", "show 3d"};
    QcrTrigger peaksProperties {"peaks properties", "properties"};
    // Experiments/Peaks/Indexing:
    QcrTrigger autoIndexer {"FFT", "FFT auto indexer"};
    QcrTrigger assignUnitCell {"assign unit cell", "assign unit cell"};
    QcrTrigger userDefinedIndexer {"userdefined indexer", "userdefined cellparameter indexer"};
    // Experiment:
    QcrTrigger removeUnusedUnitCells {"remove unused cells", "remove unused unit cells"};
    QcrTrigger predictPeaks {"predict peaks", "predict peaks"};
    // Help
    QcrTrigger about {"about", "about"};
    // View
    QcrTrigger reset {"reset", "reset"};
    QcrToggle viewExperiment {"adhoc_viewExperiment", "show experiment", true};
    QcrToggle viewProperties {"adhoc_viewProperties", "show Properties", true};
    QcrToggle viewLogger {"adhoc_viewLogger", "show Logger", true};
    QcrToggle viewImage {"adhoc_viewImage", "show Image", true};
    QcrToggle viewPlotter {"adhoc_viewPlotter", "show Plot", true};
    // Options
    QcrTrigger pixelPosition {"pixel position", "pixel position"};
    QcrTrigger gammaNu {"gamma nu", "gamma nu"};
    QcrTrigger twoTheta {"2 theta", "2 theta"};
    QcrTrigger dSpacing {"d-spacing", "d-spacing"};
    QcrTrigger millerIndices {"miller indices", "miller indices"};
    QcrTrigger fromSample {"from sample", "from sample"};
    QcrTrigger behindDetector {"behind detector", "behind detector"};
    QcrToggle showLabels {"show labels", "show labels", false};
    QcrToggle showAreas {"show areas", "show areas", false};
    QcrToggle drawPeakArea {"draw peak area", "draw peak integration area", false};
    QcrToggle logarithmicScale {"logarithmic scale", "logarithmic scale", false};
    // Export
    QcrTrigger exportPlot {"exportPlot", "export plot"};

private:
    void setupFiles();
    void setupData();
    void setupInstrument();
    void setupPeaks();
    void setupExperiment();
    void setupOptions();
    void setupRest();
};

#endif // GUI_ACTIONS_TRIGGERS_H
