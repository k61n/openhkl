#include <QCheckBox>
#include <QLayout>

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
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Refiner.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "CollectedPeaksModel.h"
#include "DialogRefiner.h"
#include "ExperimentItem.h"
#include "PeaksItem.h"
#include "ui_DialogRefiner.h"

DialogRefiner* DialogRefiner::_instance = nullptr;

DialogRefiner* DialogRefiner::create(ExperimentItem *experiment_item, nsx::sptrUnitCell unit_cell, const nsx::PeakList &peaks, QWidget *parent)
{
    if (!_instance) {
        _instance = new DialogRefiner(experiment_item, unit_cell, peaks, parent);
    }

    return _instance;
}

DialogRefiner* DialogRefiner::Instance()
{
    return _instance;
}

DialogRefiner::DialogRefiner(ExperimentItem *experiment_item, nsx::sptrUnitCell unit_cell, const nsx::PeakList &peaks, QWidget *parent):
    QDialog(parent),
    _ui(new Ui::DialogRefiner),
    _experiment_item(experiment_item),
    _unit_cell(unit_cell){
    _ui->setupUi(this);

    setModal(false);

    setWindowModality(Qt::NonModal);

    setAttribute(Qt::WA_DeleteOnClose);

    QStringList labels({"axis name","offset value (deg)"});

    auto diffractometer = _experiment_item->experiment()->diffractometer();

    auto detector = diffractometer->detector();
    auto detector_gonio = detector->gonio();

    auto detector_axis_names = detector_gonio->physicalAxesNames();
    _ui->detector_offsets->setRowCount(detector_axis_names.size());
    _ui->detector_offsets->setColumnCount(2);
    _ui->detector_offsets->setHorizontalHeaderLabels(labels);
    _ui->detector_offsets->horizontalHeader()->setStretchLastSection(true);

    for (auto i = 0; i < detector_axis_names.size(); ++i) {
        auto item = new QTableWidgetItem();
        item->setText(QString::fromStdString(detector_axis_names[i]));
        _ui->detector_offsets->setItem(i,0,item);
    }

    auto sample = diffractometer->sample();
    auto sample_gonio = sample->gonio();

    auto sample_axis_names = sample_gonio->physicalAxesNames();
    _ui->sample_offsets->setRowCount(sample_axis_names.size());
    _ui->sample_offsets->setColumnCount(2);
    _ui->sample_offsets->setHorizontalHeaderLabels(labels);
    _ui->sample_offsets->horizontalHeader()->setStretchLastSection(true);

    for (auto i = 0; i < sample_axis_names.size(); ++i) {
        auto item = new QTableWidgetItem();
        item->setText(QString::fromStdString(sample_axis_names[i]));
        _ui->sample_offsets->setItem(i,0,item);
    }

    _peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),peaks);
    _ui->peaks->setModel(_peaks_model);
}

DialogRefiner::~DialogRefiner()
{
    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void DialogRefiner::slotActionClicked(QAbstractButton *button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        refine();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        reject();
        break;
    }
    case QDialogButtonBox::StandardButton::Ok: {
        accept();
        break;
    }
    default: {
        return;
    }
    }
}
void DialogRefiner::refine()
{
    auto selection_model = _ui->peaks->selectionModel();

    auto selected_rows = selection_model->selectedRows();

    if (selected_rows.size() < 10) {
        nsx::error()<<"Not enough peak selecting for refining";
        return;
    }

    auto&& peaks = _peaks_model->peaks();

    nsx::PeakList selected_peaks;
    for (auto r : selected_rows) {
        selected_peaks.push_back(peaks[r.row()]);
    }

    std::set<nsx::sptrDataSet> data;
    // get list of datasets
    for (auto p: selected_peaks) {
        data.insert(p->data());
    }

    const unsigned int frames_per_batch = _ui->spinBoxFramesPerBatch->value();

    // used to compute optimal number of batches
    auto nbatches = [=](const nsx::PeakList& peaks) {        
        std::numeric_limits<double> lim;
        double fmin = lim.max();
        double fmax = lim.min();

        for (auto&& peak: peaks) {
            auto center = peak->shape().center();
            fmin = std::min(fmin, center[2]);
            fmax = std::max(fmax, center[2]);
        }

        int batches = int((fmax-fmin)/frames_per_batch);
        if (batches > 0) {
            return batches;
        } else {
            return 1;
        }
    };
      
    for (auto d : data) {

        nsx::PeakList reference_peaks, predicted_peaks;

        for (auto peak: selected_peaks) {
            if (peak->data() != d) {
                continue;
            }
            if (peak->predicted()) {
                predicted_peaks.push_back(peak);
            } else {
                reference_peaks.push_back(peak);
            }
        }

        nsx::info() << reference_peaks.size() << " available for refinement.";

        nsx::Refiner r(_unit_cell, reference_peaks, nbatches(reference_peaks));

        if (_ui->checkBoxRefineLattice->isChecked()) {
            r.refineUB();
            nsx::info() << "Refining B matrix";
        }

        std::vector<nsx::InstrumentState>& states = d->instrumentStates();
        
        if (_ui->checkBoxRefineSamplePosition->isChecked()) {
            r.refineSamplePosition(states); 
            nsx::info() << "Refinining sample position";
        }

        if (_ui->checkBoxRefineSampleOrientation->isChecked()) {
            nsx::info() << "Refinining sample orientation";
            r.refineSampleOrientation(states);
        }

        if (_ui->checkBoxRefineDetectorOffset->isChecked()) {
            r.refineDetectorOffset(states);
            nsx::info() << "Refinining detector offset";
        }

        if (_ui->checkBoxRefineKi->isChecked()) {
            nsx::info() << "Refining Ki";
            r.refineKi(states);
        }

        bool success = r.refine();

        if (!success) {
            nsx::info() << "Failed to refine parameters for numor " << d->filename();
        }  else {
            nsx::info() << "Successfully refined parameters for numor " << d->filename();

            int updated = r.updatePredictions(predicted_peaks);
            nsx::info() << "done; updated " << updated << " predicted peaks";
        }
    }

    // Update the peak table view
    QModelIndex topLeft = _peaks_model->index(0, 0);
    QModelIndex bottomRight = _peaks_model->index(_peaks_model->rowCount(QModelIndex())-1, _peaks_model->columnCount(QModelIndex())-1);
    emit _peaks_model->dataChanged(topLeft,bottomRight);
}

void DialogRefiner::accept()
{
    auto peaks_item = _experiment_item->peaksItem();

    emit _experiment_item->model()->itemChanged(peaks_item);

    QDialog::accept();
}
