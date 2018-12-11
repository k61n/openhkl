#include <QFileInfo>

#include <core/DataSet.h>

#include "MetaTypes.h"
#include "WidgetRefinerFit.h"

#include "ui_WidgetRefinerFit.h"

WidgetRefinerFit::WidgetRefinerFit(const std::map<nsx::sptrDataSet, nsx::Refiner>& refiners)
    : _ui(new Ui::WidgetRefinerFit), _refiners(refiners)
{
    _ui->setupUi(this);

    for (auto p : refiners) {
        auto data = p.first;
        QFileInfo fileinfo(QString::fromStdString(data->filename()));

        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole, QVariant::fromValue(data));
        _ui->selected_data->addItem(item);
    }

    connect(
        _ui->selected_data, SIGNAL(currentRowChanged(int)), this,
        SLOT(slotSelectedDataChanged(int)));

    connect(
        _ui->selected_batch, SIGNAL(valueChanged(int)), this, SLOT(slotSelectedBatchChanged(int)));

    connect(
        _ui->selected_frame, SIGNAL(valueChanged(int)), this, SLOT(slotSelectedFrameChanged(int)));
    connect(
        _ui->selected_frame_slider, SIGNAL(valueChanged(int)), this,
        SLOT(slotSelectedFrameChanged(int)));
}

WidgetRefinerFit::~WidgetRefinerFit()
{
    delete _ui;
}

void WidgetRefinerFit::slotSelectedDataChanged(int selected_data)
{
    Q_UNUSED(selected_data)

    auto current_item = _ui->selected_data->currentItem();

    auto data = current_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    auto&& refiner = _refiners.at(data);
    auto&& batches = refiner.batches();

    _ui->selected_batch->setMinimum(0);
    _ui->selected_batch->setMaximum(batches.size() - 1);

    auto max_frame = data->nFrames() - 1;

    _ui->selected_frame->setMinimum(0);
    _ui->selected_frame->setMaximum(max_frame);

    _ui->selected_frame_slider->setMinimum(0);
    _ui->selected_frame_slider->setMaximum(max_frame);

    slotSelectedBatchChanged(0);
    slotSelectedFrameChanged(0);
}

void WidgetRefinerFit::slotSelectedBatchChanged(int selected_batch)
{
    _ui->plot->clearGraphs();

    // If no data is selected, return
    auto current_data_item = _ui->selected_data->currentItem();
    if (!current_data_item) {
        return;
    }

    // If no refiner is set for this data, return
    auto data = current_data_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    // If no batches are set for this refiner, return
    auto&& refiner = _refiners.at(data);
    auto&& batches = refiner.batches();

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    _ui->plot->addGraph();
    _ui->plot->graph(0)->setPen(pen);

    // Get the cost function for this batch
    auto&& batch = batches[selected_batch];
    auto&& cost_function = batch.costFunction();

    std::vector<double> iterations(cost_function.size());
    std::iota(iterations.begin(), iterations.end(), 0);

    QVector<double> x_values = QVector<double>::fromStdVector(iterations);
    QVector<double> y_values = QVector<double>::fromStdVector(cost_function);

    _ui->plot->graph(0)->addData(x_values, y_values);

    _ui->plot->xAxis->setLabel("# iterations");
    _ui->plot->yAxis->setLabel("Cost function");

    _ui->plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _ui->plot->xAxis->setTickLabelFont(font);
    _ui->plot->yAxis->setTickLabelFont(font);

    _ui->plot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    _ui->plot->rescaleAxes();

    _ui->plot->replot();
}

void WidgetRefinerFit::slotSelectedFrameChanged(int selected_frame)
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
    _ui->refined->setStyleSheet(
        selected_state.refined ? "QLabel {color : blue;}" : "QLabel {color : red;}");
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

    double wavelength_offset = wavelength * (ni.norm() - 1.0);
    _ui->wavelength_offset->setValue(wavelength_offset);

    Eigen::Matrix3d sample_orientation_matrix = selected_state.sampleOrientationMatrix();
    _ui->sample_orientation_00->setValue(sample_orientation_matrix(0, 0));
    _ui->sample_orientation_01->setValue(sample_orientation_matrix(0, 1));
    _ui->sample_orientation_02->setValue(sample_orientation_matrix(0, 2));
    _ui->sample_orientation_10->setValue(sample_orientation_matrix(1, 0));
    _ui->sample_orientation_11->setValue(sample_orientation_matrix(1, 1));
    _ui->sample_orientation_12->setValue(sample_orientation_matrix(1, 2));
    _ui->sample_orientation_20->setValue(sample_orientation_matrix(2, 0));
    _ui->sample_orientation_21->setValue(sample_orientation_matrix(2, 1));
    _ui->sample_orientation_22->setValue(sample_orientation_matrix(2, 2));

    _ui->detector_orientation_00->setValue(selected_state.detectorOrientation(0, 0));
    _ui->detector_orientation_01->setValue(selected_state.detectorOrientation(0, 1));
    _ui->detector_orientation_02->setValue(selected_state.detectorOrientation(0, 2));
    _ui->detector_orientation_10->setValue(selected_state.detectorOrientation(1, 0));
    _ui->detector_orientation_11->setValue(selected_state.detectorOrientation(1, 1));
    _ui->detector_orientation_12->setValue(selected_state.detectorOrientation(1, 2));
    _ui->detector_orientation_20->setValue(selected_state.detectorOrientation(2, 0));
    _ui->detector_orientation_21->setValue(selected_state.detectorOrientation(2, 1));
    _ui->detector_orientation_22->setValue(selected_state.detectorOrientation(2, 2));
}
