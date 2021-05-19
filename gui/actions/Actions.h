//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/Actions.h
//! @brief     Defines class Actions
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_ACTIONS_ACTIONS_H
#define NSX_GUI_ACTIONS_ACTIONS_H

#include <QAction>
#include <QObject>

//! Collection of trigger and toggle actions, for use as member of MainWin.
class Actions : public QObject {
    Q_OBJECT

 public:
    Actions();

    void removeData();

    // Files:
    QAction* new_experiment;
    QAction* load_experiment;
    QAction* save_experiment;
    QAction* save_all_experiment;
    QAction* remove_experiment;
    QAction* quit;

    // View:
    QAction* detector_window;

    // Data
    QAction* add_data;
    QAction* remove_data;
    QAction* add_raw;
    QAction* add_hdf5;
    QAction* add_nexus;

    // Peaks
    QAction* add_peaks;
    QAction* remove_peaks;

    // Unit Cells
    QAction* add_cell;
    QAction* remove_cell;

    // Help
    QAction* about; 

    //  // Experiment/Data:
    //  QAction convertHDF5;// {"convert", "convert to HDF5"};
    //  QAction dataProperties;// {"data properties", "properties"};
    //  QAction findPeaks;//{"find peaks", "find peaks"};
    //  QAction importRaw;//{"import raw", "import raw data"};
    //  QAction instrumentStates;//{"instrument states", "instrument states dialog"};
    //  QAction loadData;//{"load data", "load data"};
    //  QAction removeData;//{"remove data", "remove data"};
    //  // Experiment/Instrument/Detector:
    //  QAction detectorPropertie;//{"detector properties", "properties"};
    //  QAction goniometer;//{"detector goniometer", "goniometer global offsets"};
    //  // Experiment/Instrument/Sample:
    //  QAction isotopesDatabase;//{"isotopes database", "open isotopes database"};
    //  QAction sampleGoniometer;//{"sample goniometer", "goniometer global offsets"};
    //  QAction sampleProperties;//{"sample properties", "properties"};
    //  QAction shapeProperties;//{"shape properties", "properties"};
    //  QAction shapeLoadMovie;//{"crystal movie", "load crystal movie"};

    //  QAction monochromaticSourceProperties;//{"monochramtic properties", "properties"};
    //  // Experiments/Peaks:
    //  QAction buildShapeCollection;//{"build shape library", "build shape library"};
    //  QAction correctAbsorption;//{"correct", "correct for absorption"};
    //  QAction filterPeaks;//{"filter peaks", "filter peaks"};
    //  QAction integratepeaks;//{"integrate peaks", "integrate peaks"};
    //  QAction normalize;//{"normalize", "normalize"};
    //  QAction refine;//{"refine", "refine lattice and instrument parameters"};
    //  QAction show3d;//{"show 3d", "show 3d"};
    //  QAction peaksProperties;//{"peaks properties", "properties"};
    //  QAction statistics;//{"statistics", "Statistics"};
    //  // Experiments/Peaks/Indexing:
    //  QAction autoIndexer;//{"FFT", "FFT auto indexer"};
    //  QAction assignUnitCell;//{"assign unit cell", "assign unit cell"};
    //  QAction userDefinedIndexer;//{"userdefined indexer", "userdefined cellparameter indexer"};
    //  // Experiment:
    //  QAction removeUnusedUnitCells;//{"remove unused cells", "remove unused unit cells"};
    //  QAction predictPeaks;//{"predict peaks", "predict peaks"};
    //  // Help
    //  QAction about;//{"about", "about"};
    //  QAction helpExperiment;//{"experiment help", "experiment"};
    //  QAction helpData;//{"data help", "data"};
    //  QAction helpPeakFinder;//{"peak finding help", "peak finding"};
    //  QAction helpPeakFilter;//{"peakFilterHelp", "peak filtering"};
    //  // View
    //  QAction reset;//{"reset", "reset"};
    //  QAction viewExperiment;//{"adhoc_viewExperiment", "show experiment", true};
    //  QAction viewProperties;//{"adhoc_viewProperties", "show Properties", true};
    //  QAction viewLogger;//{"adhoc_viewLogger", "show Logger", true};
    //  QAction viewImage;//{"adhoc_viewImage", "show Image", true};
    //  QAction viewPlotter;//{"adhoc_viewPlotter", "show Plot", true};
    //  // Options
    //  QAction pixelPosition;//{"pixel position", "pixel position"};
    //  QAction gammaNu;//{"gamma nu", "gamma nu"};
    //  QAction twoTheta;//{"2 theta", "2 theta"};
    //  QAction dSpacing;//{"d-spacing", "d-spacing"};
    //  QAction millerIndices;//{"miller indices", "miller indices"};
    //  QAction fromSample;//{"from sample", "from sample"};
    //  QAction behindDetector;//{"behind detector", "behind detector"};
    //  QAction showLabels;//{"show labels", "show labels", false};
    //  QAction showAreas;//{"show areas", "show areas", false};
    //  QAction drawPeakArea;//{"draw peak area", "draw peak integration area", false};
    //  QAction logarithmicScale;//{"logarithmic scale", "logarithmic scale", false};
    //  // Export
    //  QAction exportPlot;//{"exportPlot", "export plot"};

 private:
    void setupFiles();
    void setupView();
    void setupData();
    void setupInstrument();
    void setupPeaks();
    void setupExperiment();
    void setupOptions();
    void setupRest();
    void setupCell();
};

#endif // NSX_GUI_ACTIONS_ACTIONS_H
