
#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <QAction>

//! Collection of trigger actions, for use as member of MainWin.

class Triggers {
public:
    Triggers();
//Files:
    QAction addExperiment {"add Experiment"};
    QAction removeExperiment {"remove Experiment"};
    QAction quit {"quit"};
//Experiment/Data:
    QAction convertHDF5 {"convert to HDF5"};
    QAction dataProperties {"properties"};
    QAction findPeaks {"find peaks"};
    QAction importRaw {"import raw data"};
    QAction instrumentStates {"instrument states dialog"};
    QAction loadData {"load data"};
    QAction removeData {"remove data"};
//Experiment/Instrument/Detector:
    QAction detectorProperties {"properties"};
    QAction goniometer {"goniometer"};
//Experiment/Instrument/Sample:
    QAction isotopesDatabase {"open isotopes database"};
    QAction sampleGoniometer {"goniometer global offsets"};
    QAction sampleProperties {"properties"};
    QAction shapeProperties {"properties"};
    QAction shapeLoadMovie {"load crystal movie"};

    QAction monochromaticSourceProperties {"properties"};
//Experiments/Peaks:
    QAction buildShapeLibrary {"build shape library"};
    QAction correctAbsorption {"correct for absorption"};
    QAction filterPeaks {"filter peaks"};
    QAction integratepeaks {"integrate peaks"};
    QAction normalize {"normalize"};
    QAction refine {"refine lattice and instrument parameters"};
    QAction show3d {"show 3d"};
    QAction peaksPropeerties {"properties"};
//Experiments/Peaks/Indexing:
    QAction autoIndexer {"FFT auto indexer"};
    QAction assignUnitCell {"assign unit cell"};
    QAction userDefinedIndexer {"userdefined cellparameter indexer"};
//Experiment:
    QAction removeUnusedUnitCells {"remove unused unit cells"};
    QAction predictPeaks {"predict peaks"};
//Help
    QAction about {"about"};
//View
    QAction reset {"reset"};
//Export
    QAction exportPlot {"export plot"};
};

#endif // TRIGGERS_H
