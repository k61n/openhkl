#include <sstream>

#include <QCheckBox>
#include <QLayout>
#include <QStatusBar>

#include <nsxlib/Axis.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Minimizer.h>
#include <nsxlib/Monochromator.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Refiner.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "DialogRefineUnitCell.h"
#include "DoubleTableItemDelegate.h"
#include "externals/QCustomPlot.h"
#include "ui_DialogRefineUnitCell.h"

DialogRefineUnitCell::DialogRefineUnitCell(nsx::sptrExperiment experiment,
                                           nsx::sptrUnitCell unitCell,
                                           nsx::PeakList peaks,
                                           QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogRefineUnitCell),
    _experiment(std::move(experiment)),
    _unitCell(std::move(unitCell)),
    _peaks(std::move(peaks))
{
    ui->setupUi(this);

    // get list of datasets
    for (auto p: _peaks) {
        _data.insert(p->data());
    }  
    connect(ui->pushButtonRefine, SIGNAL(clicked()), this, SLOT(refineParameters()));
}

DialogRefineUnitCell::~DialogRefineUnitCell()
{
    delete ui;
}

void DialogRefineUnitCell::refineParameters()
{
    const unsigned int nbatches = ui->spinBoxBatches->value();
    nsx::Refiner r(_unitCell, _peaks, nbatches);    

    if (ui->checkBoxRefineLattice->isChecked()) {
        r.refineB();
        nsx::info() << "Refining B matrix";
    }

    for (auto d: _data) {
        std::vector<nsx::InstrumentState>& states = d->getInstrumentStates();
        const int nsample = states[0].sample._offsets.size();
        const int ndetector = states[0].detector._offsets.size();
        
        if (ui->checkBoxRefineSample->isChecked()) {
            for (auto i = 0; i < nsample; ++i) {
                r.refineSampleState(states, i);
            }
            nsx::info() << "Refinining " << nsample << " sample axes";
        }

        if (ui->checkBoxRefineDetector->isChecked()) {
            for (auto i = 0; i < ndetector; ++i) {
                r.refineDetectorState(states, i);
            }
            nsx::info() << "Refinining " << ndetector << " detector axes";
        }
    }

    bool success = r.refine();
    if (!success) {
        nsx::info() << "Failed to refine parameters!";
        return;
    } 

    nsx::info() << "Succeeded to refine parameters!";
    nsx::info() << "updating predicted peaks...";
    int updated = r.updatePredictions(_peaks);
    nsx::info() << "done; updated " << updated << " peaks";
}
