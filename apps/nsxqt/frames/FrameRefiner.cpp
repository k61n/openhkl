#include <numeric>

#include <QFileInfo>
#include <QItemSelectionModel>
#include <QPen>
#include <QSpinBox>

#include <nsxlib/DataSet.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Units.h>

#include "CollectedPeaksModel.h"
#include "DoubleItemDelegate.h"
#include "ExperimentItem.h"
#include "FrameRefiner.h"
#include "MetaTypes.h"
#include "PeaksItem.h"
#include "SXPlot.h"

#include "ui_FrameRefiner.h"

FrameRefiner* FrameRefiner::_instance = nullptr;

FrameRefiner* FrameRefiner::create(ExperimentItem *experiment_item, const nsx::PeakList &peaks)
{
    if (!_instance) {
        _instance = new FrameRefiner(experiment_item, peaks);
    }

    return _instance;
}

FrameRefiner* FrameRefiner::Instance()
{
    return _instance;
}

FrameRefiner::FrameRefiner(ExperimentItem* experiment_item, const nsx::PeakList &peaks)
: NSXQFrame(),
  _ui(new Ui::FrameRefiner),
  _experiment_item(experiment_item)
{
    _ui->setupUi(this);

    CollectedPeaksModel *peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),peaks);
    _ui->peaks->setModel(peaks_model);
    _ui->peaks->selectAll();

    _ui->tabWidget->setCurrentIndex(0);

    std::set<nsx::sptrDataSet> data;
    // get list of datasets
    for (auto p: peaks) {
        auto d = p->data();
        if (!d) {
            continue;
        }
        data.insert(d);
    }

    for (auto d : data) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));

        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole,QVariant::fromValue(d));
        _ui->selected_data->addItem(item);
    }

    connect(_ui->selected_data,SIGNAL(currentRowChanged(int)),this,SLOT(slotSelectedDataChanged(int)));

    connect(_ui->selected_batch,SIGNAL(valueChanged(int)),this,SLOT(slotSelectedBatchChanged()));

    connect(_ui->selected_frame,SIGNAL(valueChanged(int)),this,SLOT(slotSelectedFrameChanged(int)));
    connect(_ui->selected_frame_slider,SIGNAL(valueChanged(int)),this,SLOT(slotSelectedFrameChanged(int)));

    connect(_ui->actions,SIGNAL(clicked(QAbstractButton*)),this,SLOT(slotActionClicked(QAbstractButton*)));

    DoubleItemDelegate* sample_axis_parameters_delegate = new DoubleItemDelegate();
    _ui->sample_orientation->setItemDelegateForColumn(1,sample_axis_parameters_delegate);
    _ui->sample_orientation->setItemDelegateForColumn(2,sample_axis_parameters_delegate);
    auto sample_axes = _experiment_item->experiment()->diffractometer()->sample()->gonio()->axes();
    _ui->sample_orientation->setRowCount(sample_axes.size());
    for (size_t i = 0; i < sample_axes.size(); ++i) {
        auto axes = sample_axes[i];
        _ui->sample_orientation->setItem(i,0, new QTableWidgetItem(QString::fromStdString(axes->name())));

    }

    DoubleItemDelegate* detector_axis_parameters_delegate = new DoubleItemDelegate();
    _ui->detector_orientation->setItemDelegateForColumn(1,detector_axis_parameters_delegate);
    _ui->detector_orientation->setItemDelegateForColumn(2,detector_axis_parameters_delegate);
    auto detector_axes = _experiment_item->experiment()->diffractometer()->detector()->gonio()->axes();
    _ui->detector_orientation->setRowCount(detector_axes.size());
    for (size_t i = 0; i < detector_axes.size(); ++i) {
        auto axes = detector_axes[i];
        _ui->detector_orientation->setItem(i,0, new QTableWidgetItem(QString::fromStdString(axes->name())));
    }

    _ui->selected_data->setCurrentRow(0);
}

FrameRefiner::~FrameRefiner()
{
    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void FrameRefiner::slotActionClicked(QAbstractButton *button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        refine();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        close();
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

void FrameRefiner::slotSelectedDataChanged(int selected_data)
{
    Q_UNUSED(selected_data)

    auto current_item = _ui->selected_data->currentItem();

    auto data = current_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    _ui->selected_batch->setMinimum(0);

    // No refiner set for this data, return.
    auto it = _refiners.find(data);
    if (it == _refiners.end()) {
        auto&& refiner = it->second;

        auto&& batches =  refiner.batches();

        // If this refiner has some batches defined then plot the cost function for the first of them
        if (!batches.empty()) {
            _ui->selected_batch->setMaximum(batches.size() - 1);
            _ui->selected_batch->setValue(0);
        }
    }

    auto max_frame = data->nFrames() - 1;

    _ui->selected_frame->setMinimum(0);
    _ui->selected_frame->setMaximum(max_frame);

    _ui->selected_frame_slider->setMinimum(0);
    _ui->selected_frame_slider->setMaximum(max_frame);

    slotSelectedFrameChanged(0);
}

void FrameRefiner::slotSelectedBatchChanged()
{
    _ui->plot->clearGraphs();

    // If no data is selected, return
    auto current_data_item = _ui->selected_data->currentItem();
    if (!current_data_item) {
        return;
    }

    // If no refiner is set for this data, return
    auto data = current_data_item->data(Qt::UserRole).value<nsx::sptrDataSet>();
    auto it = _refiners.find(data);
    if (it == _refiners.end()) {
        return;
    }

    // If no batches are set for this refiner, return
    auto&& refiner = it->second;
    auto&& batches =  refiner.batches();
    if (batches.empty()) {
        return;
    }

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    _ui->plot->addGraph();
    _ui->plot->graph(0)->setPen(pen);

    // Get the cost function for this batch
    auto selected_batch = _ui->selected_batch->value();
    auto&& batch = batches[selected_batch];
    auto&& cost_function = batch.costFunction();

    std::vector<double> iterations(cost_function.size());
    std::iota(iterations.begin(),iterations.end(),0);

    QVector<double> x_values = QVector<double>::fromStdVector(iterations);
    QVector<double> y_values = QVector<double>::fromStdVector(cost_function);

    _ui->plot->graph(0)->addData(x_values,y_values);

    _ui->plot->xAxis->setLabel("# iterations");
    _ui->plot->yAxis->setLabel("Cost function");

    _ui->plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _ui->plot->xAxis->setTickLabelFont(font);
    _ui->plot->yAxis->setTickLabelFont(font);

    _ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    _ui->plot->rescaleAxes();

    _ui->plot->replot();
}

void FrameRefiner::slotSelectedFrameChanged(int selected_frame)
{
    _ui->selected_frame->setValue(selected_frame);
    _ui->selected_frame_slider->setValue(selected_frame);

    auto current_item = _ui->selected_data->currentItem();

    // No data selected, return
    if (!current_item) {
        return;
    }

    auto data = current_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    auto&& instrument_states = data->instrumentStates();

    const auto& selected_state = instrument_states[selected_frame];

    QFont font;
    font.setBold(true);
    _ui->refined->setStyleSheet(selected_state.refined ? "QLabel {color : blue;}" : "QLabel {color : red;}");
    _ui->refined->setFont(font);
    _ui->refined->setText(selected_state.refined ? "Refined" : "Not refined");

    // Set the sample position values
    const auto& sample_position = selected_state.samplePosition;
    _ui->sample_position_x->setValue(sample_position[0]);
    _ui->sample_position_y->setValue(sample_position[1]);
    _ui->sample_position_z->setValue(sample_position[2]);

    // Set the sample position values
    const auto& detector_position_offset = selected_state.detectorPositionOffset;
    _ui->detector_position_offset_x->setValue(detector_position_offset[0]);
    _ui->detector_position_offset_y->setValue(detector_position_offset[1]);
    _ui->detector_position_offset_z->setValue(detector_position_offset[2]);

    // Set the normalized incoming beam
    const auto& ni = selected_state.ni;
    _ui->ni_x->setValue(ni[0]);
    _ui->ni_y->setValue(ni[1]);
    _ui->ni_z->setValue(ni[2]);

    const auto& wavelength = selected_state.wavelength;
    _ui->wavelength->setValue(wavelength);

    double wavelength_offset = wavelength*(ni.norm() - 1.0);
    _ui->wavelength_offset->setValue(wavelength_offset);

    auto&& sample_states = data->dataReader()->sampleStates();
    auto&& sample_state = sample_states[selected_frame];
    for (size_t i = 0; i < sample_state.size(); ++i) {
        auto item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,QVariant::fromValue(sample_state[i]/nsx::deg));
        _ui->sample_orientation->setItem(i,1,item);
    }

    auto&& detector_states = data->dataReader()->detectorStates();
    auto&& detector_state = detector_states[selected_frame];
    for (size_t i = 0; i < detector_state.size(); ++i) {
        auto item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,QVariant::fromValue(detector_state[i]/nsx::deg));
        _ui->detector_orientation->setItem(i,1,item);
    }
}

void FrameRefiner::refine()
{
    // Check and construct the peak selection
    auto selection_model = _ui->peaks->selectionModel();
    QModelIndexList selected_rows = selection_model->selectedRows();
    if (selected_rows.size() < 100) {
        nsx::error()<<"No or not enough peaks selected for refining";
        return;
    }

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_ui->peaks->model());
    auto&& peaks = peaks_model->peaks();
    nsx::PeakList selected_peaks;
    for (auto r : selected_rows) {
        selected_peaks.push_back(peaks[r.row()]);
    }

    auto unit_cell = selected_peaks[0]->unitCell();

    if (!unit_cell) {
        nsx::error()<<"No unit cell set for the selected peaks";
        return;
    }

    auto&& frames_per_batch = _ui->nframes_per_batch->value();

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

    _refiners.clear();

    std::set<nsx::sptrDataSet> data;
    // get list of datasets
    for (auto p: peaks) {
        data.insert(p->data());
    }

    for (auto d : data) {

        nsx::PeakList reference_peaks, predicted_peaks;

        // Keep the peak that belong to this data and split them between the found and predicted ones
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

        nsx::info() << reference_peaks.size() << " splitted over " << nbatches(reference_peaks)<<"refining batches.";

        std::vector<nsx::InstrumentState>& states = d->instrumentStates();

        auto&& p =_refiners.emplace(d,nsx::Refiner(states, unit_cell, reference_peaks, nbatches(reference_peaks)));

        auto&& refiner = p.first->second;

        if (_ui->refine_ub->isChecked()) {
            refiner.refineUB();
            nsx::info() << "Refining UB matrix";
        }

        if (_ui->refine_sample_position->isChecked()) {
            refiner.refineSamplePosition();
            nsx::info() << "Refinining sample position";
        }

        if (_ui->refine_detector_position->isChecked()) {
            refiner.refineDetectorOffset();
            nsx::info() << "Refinining detector position";
        }

        if (_ui->refine_sample_orientation->isChecked()) {
            refiner.refineSampleOrientation();
            nsx::info() << "Refinining sample orientation";
        }

        if (_ui->refine_ki->isChecked()) {
            refiner.refineKi();
            nsx::info() << "Refining Ki";
        }

        bool success = refiner.refine();

        if (success) {
            nsx::info() << "Successfully refined parameters for numor " << d->filename();
            int updated = refiner.updatePredictions(predicted_peaks);
            nsx::info() << "done; updated " << updated << " peaks";
        }  else {
            nsx::info() << "Failed to refine parameters for numor " << d->filename();
        }
    }

    slotSelectedBatchChanged();

    // Update the peak table view
    QModelIndex topLeft = peaks_model->index(0, 0);
    QModelIndex bottomRight = peaks_model->index(peaks_model->rowCount(QModelIndex())-1, peaks_model->columnCount(QModelIndex())-1);
    emit peaks_model->dataChanged(topLeft,bottomRight);
}

void FrameRefiner::accept()
{
    auto peaks_item = _experiment_item->peaksItem();

    emit _experiment_item->model()->itemChanged(peaks_item);

    close();
}
