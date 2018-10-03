#include <QFileInfo>
#include <QFont>

#include <nsxlib/DataSet.h>
#include <nsxlib/InstrumentState.h>

#include "ui_DialogExploreInstrumentStates.h"
#include "DialogExploreInstrumentStates.h"
#include "MetaTypes.h"

DialogExploreInstrumentStates* DialogExploreInstrumentStates::_instance = nullptr;

DialogExploreInstrumentStates* DialogExploreInstrumentStates::create(const nsx::DataList& data, QWidget* parent)
{
    if (!_instance) {
        _instance = new DialogExploreInstrumentStates(data, parent);
    }

    return _instance;
}

DialogExploreInstrumentStates* DialogExploreInstrumentStates::Instance()
{
    return _instance;
}

DialogExploreInstrumentStates::DialogExploreInstrumentStates(const nsx::DataList& data, QWidget *parent)
: QDialog(parent),
  _ui(new Ui::DialogExploreInstrumentStates)
{
    _ui->setupUi(this);

    setModal(false);

    setWindowModality(Qt::NonModal);

    setAttribute(Qt::WA_DeleteOnClose);

    for (auto d : data) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));

        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole,QVariant::fromValue(d));
        _ui->data->addItem(item);
    }

    connect(_ui->data,SIGNAL(currentRowChanged(int)),this,SLOT(slotSelectedDataChanged(int)));
    connect(_ui->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(slotSelectedFrameChanged(int)));
    connect(_ui->frameIndex,SIGNAL(valueChanged(int)),this,SLOT(slotSelectedFrameChanged(int)));

    _ui->data->setCurrentRow(0);
}

DialogExploreInstrumentStates::~DialogExploreInstrumentStates()
{
    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void DialogExploreInstrumentStates::slotSelectedDataChanged(int selected_data)
{
    Q_UNUSED(selected_data)

    auto current_item = _ui->data->currentItem();

    auto data = current_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    _ui->frameIndex->setMinimum(0);
    _ui->frameIndex->setMaximum(data->nFrames()-1);
    _ui->frameIndex->setValue(0);

    _ui->frameSlider->setMinimum(0);
    _ui->frameSlider->setMaximum(data->nFrames()-1);
    _ui->frameSlider->setValue(0);

    slotSelectedFrameChanged(0);
}

void DialogExploreInstrumentStates::slotSelectedFrameChanged(int selected_frame)
{
    _ui->frameIndex->setValue(selected_frame);
    _ui->frameSlider->setValue(selected_frame);

    auto current_item = _ui->data->currentItem();

    auto data = current_item->data(Qt::UserRole).value<nsx::sptrDataSet>();

    auto&& instrument_states = data->instrumentStates();

    auto selected_state = instrument_states[selected_frame];

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

    // Set the sample orientation values
    auto&& sample_orientation = selected_state.sampleOrientation.normalized().toRotationMatrix();
    _ui->sample_orientation_00->setValue(sample_orientation(0,0));
    _ui->sample_orientation_01->setValue(sample_orientation(0,1));
    _ui->sample_orientation_02->setValue(sample_orientation(0,2));
    _ui->sample_orientation_10->setValue(sample_orientation(1,0));
    _ui->sample_orientation_11->setValue(sample_orientation(1,1));
    _ui->sample_orientation_12->setValue(sample_orientation(1,2));
    _ui->sample_orientation_20->setValue(sample_orientation(2,0));
    _ui->sample_orientation_21->setValue(sample_orientation(2,1));
    _ui->sample_orientation_22->setValue(sample_orientation(2,2));

    // Set the sample orientation offset values
    auto&& sample_orientation_offset = selected_state.sampleOrientationOffset.normalized().toRotationMatrix();
    _ui->sample_orientation_offset_00->setValue(sample_orientation_offset(0,0));
    _ui->sample_orientation_offset_01->setValue(sample_orientation_offset(0,1));
    _ui->sample_orientation_offset_02->setValue(sample_orientation_offset(0,2));
    _ui->sample_orientation_offset_10->setValue(sample_orientation_offset(1,0));
    _ui->sample_orientation_offset_11->setValue(sample_orientation_offset(1,1));
    _ui->sample_orientation_offset_12->setValue(sample_orientation_offset(1,2));
    _ui->sample_orientation_offset_20->setValue(sample_orientation_offset(2,0));
    _ui->sample_orientation_offset_21->setValue(sample_orientation_offset(2,1));
    _ui->sample_orientation_offset_22->setValue(sample_orientation_offset(2,2));

    // Set the detector orientation values
    const auto& detector_orientation = selected_state.detectorOrientation;
    _ui->detector_orientation_00->setValue(detector_orientation(0,0));
    _ui->detector_orientation_01->setValue(detector_orientation(0,1));
    _ui->detector_orientation_02->setValue(detector_orientation(0,2));
    _ui->detector_orientation_10->setValue(detector_orientation(1,0));
    _ui->detector_orientation_11->setValue(detector_orientation(1,1));
    _ui->detector_orientation_12->setValue(detector_orientation(1,2));
    _ui->detector_orientation_20->setValue(detector_orientation(2,0));
    _ui->detector_orientation_21->setValue(detector_orientation(2,1));
    _ui->detector_orientation_22->setValue(detector_orientation(2,2));

    // Set the normalized incoming beam
    const auto& ni = selected_state.ni;
    _ui->ni_x->setValue(ni[0]);
    _ui->ni_y->setValue(ni[1]);
    _ui->ni_z->setValue(ni[2]);
}
