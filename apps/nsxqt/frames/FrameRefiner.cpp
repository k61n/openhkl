#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

#include <nsxlib/Peak3D.h>

#include "CollectedPeaksModel.h"
#include "ExperimentItem.h"
#include "FrameRefiner.h"
#include "PeakTableView.h"

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

FrameRefiner::FrameRefiner(ExperimentItem *experiment_item, const nsx::PeakList &peaks)
: NSXQFrame(),
  _experiment_item(experiment_item)
{
    setWindowTitle(tr("UB and instrument offsets refiner"));

    setupLayout();

    CollectedPeaksModel *peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),peaks);
    _peaks->setModel(peaks_model);

    setupConnections();
}

FrameRefiner::~FrameRefiner()
{
    if (_instance) {
        _instance = nullptr;
    }
}

void FrameRefiner::setupLayout()
{
    // Set the tabs
    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(new QWidget(),"Settings");
    tabs->addTab(new QWidget(),"Refined offsets");

    _peaks = new PeakTableView();

    // The checkbox for refining the UB matrix
    _refine_ub = new QCheckBox();
    _refine_ub->setChecked(true);
    _refine_ub->setToolTip("Refine or not the UB matrix");

    // The checkbox for refining the sample position
    _refine_sample_position = new QCheckBox();
    _refine_sample_position->setChecked(true);
    _refine_sample_position->setToolTip("Refine or not the sample position");

    // The checkbox for refining the sample orientation
    _refine_sample_orientation = new QCheckBox();
    _refine_sample_orientation->setChecked(true);
    _refine_sample_orientation->setToolTip("Refine or not the sample orientation");

    // The checkbox for refining the incident beam
    _refine_ki = new QCheckBox();
    _refine_ki->setChecked(true);
    _refine_ki->setToolTip("Refine or not the incident beam (ki)");

    // The checkbox for refining the detector offsets
    _refine_detector_offsets = new QCheckBox();
    _refine_detector_offsets->setChecked(false);
    _refine_detector_offsets->setToolTip("Refine or not the detector offsets");

    // The spinbox for setting the number of frames per batch
    _nframes_per_batch = new QSpinBox();
    _nframes_per_batch->setWrapping(true);
    _nframes_per_batch->setMinimum(1e1);
    _nframes_per_batch->setMaximum(1e4);
    _nframes_per_batch->setValue(1e3);
    _nframes_per_batch->setToolTip(tr("Set the number of frames used to subdivide each dataset into batches where the refined parameters are constant"));

    // The list of data for inspecting the offsets
    _datas = new QListWidget();

    // The frame number spinbox
    _selected_frame = new QSpinBox();
    _selected_frame->setMinimum(0);
    _selected_frame->setMaximum(1e4);
    _selected_frame->setValue(0);
    _selected_frame->setWrapping(true);
    _selected_frame->setSingleStep(1);
    _selected_frame->setToolTip("The value of the selected frame for viewing the offsets");

    // The frame number slider
    _selected_frame_slider = new QSlider();
    _selected_frame_slider->setOrientation(Qt::Horizontal);
    _selected_frame_slider->setMinimum(0);
    _selected_frame_slider->setMaximum(1e4);
    _selected_frame_slider->setValue(0);
    _selected_frame_slider->setSingleStep(1);
    _selected_frame_slider->setToolTip("The selected frame for viewing the offsets");

    // The spinboxes for viewing the sample position offsets
    for (size_t i=0; i<3; ++i) {
        _sample_position[i] = new QDoubleSpinBox();
        _sample_position[i]->setValue(0.0);
        _sample_position[i]->setMinimum(-10.0);
        _sample_position[i]->setMaximum(10.0);
        _sample_position[i]->setDecimals(4);
        _sample_position[i]->setWrapping(true);
        _sample_position[i]->setSingleStep(1.0e-4);
        _sample_position[i]->setToolTip("The sample position offset");
    }

    // The spinboxes for viewing the sample orientation
    for (size_t i=0; i<3; ++i) {
        for (size_t j=0; j<3; ++j) {
            _sample_orientation[i][j] = new QDoubleSpinBox();
            _sample_orientation[i][j]->setValue(0.0);
            _sample_orientation[i][j]->setMinimum(-1.0);
            _sample_orientation[i][j]->setMaximum(1.0);
            _sample_orientation[i][j]->setDecimals(4);
            _sample_orientation[i][j]->setWrapping(true);
            _sample_orientation[i][j]->setSingleStep(1.0e-4);
            _sample_orientation[i][j]->setToolTip("The sample orientation matrix offset");
        }
    }

    // The spinboxes for viewing the sample orientation offsets
    for (size_t i=0; i<3; ++i) {
        for (size_t j=0; j<3; ++j) {
            _sample_orientation_offsets[i][j] = new QDoubleSpinBox();
            _sample_orientation_offsets[i][j]->setValue(0.0);
            _sample_orientation_offsets[i][j]->setMinimum(-1.0);
            _sample_orientation_offsets[i][j]->setMaximum(1.0);
            _sample_orientation_offsets[i][j]->setDecimals(4);
            _sample_orientation_offsets[i][j]->setWrapping(true);
            _sample_orientation_offsets[i][j]->setSingleStep(1.0e-4);
            _sample_orientation_offsets[i][j]->setToolTip("The sample orientation matrix offset");
        }
    }

    // The spinboxes for viewing the detector orientation
    for (size_t i=0; i<3; ++i) {
        for (size_t j=0; j<3; ++j) {
            _detector_orientation[i][j] = new QDoubleSpinBox();
            _detector_orientation[i][j]->setValue(0.0);
            _detector_orientation[i][j]->setMinimum(-1.0);
            _detector_orientation[i][j]->setMaximum(1.0);
            _detector_orientation[i][j]->setDecimals(4);
            _detector_orientation[i][j]->setWrapping(true);
            _detector_orientation[i][j]->setSingleStep(1.0e-4);
            _detector_orientation[i][j]->setToolTip("The detector orientation");
        }
    }

    // The spinboxes for viewing the incident beam offsets
    for (size_t i=0; i<3; ++i) {
        _ni[i] = new QDoubleSpinBox();
        _ni[i]->setValue(0.0);
        _ni[i]->setMinimum(-1.0);
        _ni[i]->setMaximum(1.0);
        _ni[i]->setDecimals(4);
        _ni[i]->setWrapping(true);
        _ni[i]->setSingleStep(1.0e-4);
        _ni[i]->setToolTip("The incident beam (ni) offset");
    }

    _actions = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Apply,this);

    // Set the layout
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    QHBoxLayout *settings_tabs_layout = new QHBoxLayout();

    QVBoxLayout *parameters_layout = new QVBoxLayout();

    QGroupBox *refinable_parameters_box = new QGroupBox(tr("Refinable parameters"));
    QFormLayout *refinable_parameters_box_layout = new QFormLayout();
    refinable_parameters_box_layout->addRow(new QLabel("UB matrix"),_refine_ub);
    refinable_parameters_box_layout->addRow(new QLabel("sample position"),_refine_sample_position);
    refinable_parameters_box_layout->addRow(new QLabel("sample orientation"),_refine_sample_orientation);
    refinable_parameters_box_layout->addRow(new QLabel("incident beam (ki)"),_refine_ki);
    refinable_parameters_box_layout->addRow(new QLabel("detector offsets"),_refine_detector_offsets);
    refinable_parameters_box->setLayout(refinable_parameters_box_layout);

    parameters_layout->addWidget(refinable_parameters_box);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(new QLabel(tr("Number of frames per batch")));
    layout->addWidget(_nframes_per_batch);
    parameters_layout->addLayout(layout);
    parameters_layout->addStretch(1);

    settings_tabs_layout->addLayout(parameters_layout);

    settings_tabs_layout->addWidget(_peaks);

    auto settings_tab = tabs->widget(0);
    settings_tab->setLayout(settings_tabs_layout);

    QHBoxLayout *offsets_tabs_layout = new QHBoxLayout();

    QVBoxLayout *data_layout = new QVBoxLayout();
    data_layout->addWidget(_datas);
    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(new QLabel(tr("Frame")));
    layout1->addWidget(_selected_frame);
    layout1->addWidget(_selected_frame_slider);
    data_layout->addLayout(layout1);

    offsets_tabs_layout->addLayout(data_layout);

    QVBoxLayout *offsets_layout = new QVBoxLayout();

    QGroupBox *sample_position_box = new QGroupBox(tr("Sample position"));
    QFormLayout *sample_position_box_layout = new QFormLayout();
    for (size_t i=0; i< 3; ++i) {
        sample_position_box_layout->addRow(nullptr,_sample_position[0]);
    }
    sample_position_box->setLayout(sample_position_box_layout);

    QGroupBox *sample_orientation_box = new QGroupBox(tr("Sample orientation"));
    QFormLayout *sample_orientation_box_layout = new QFormLayout();
    for (size_t i=0; i < 3; ++i) {
        for (size_t j=0; j < 3; ++j) {
            sample_orientation_box_layout->addRow(nullptr,_sample_orientation[0][0]);
        }
    }
    sample_orientation_box->setLayout(sample_orientation_box_layout);

    QGroupBox *sample_orientation__offsets_box = new QGroupBox(tr("Sample orientation offsets"));
    QFormLayout *sample_orientation_offsets_box_layout = new QFormLayout();
    for (size_t i=0; i < 3; ++i) {
        for (size_t j=0; j < 3; ++j) {
            sample_orientation_box_layout->addRow(nullptr,_sample_orientation[0][0]);
        }
    }
    sample_orientation_box->setLayout(sample_orientation_box_layout);

    offsets_layout->addWidget(sample_position_box);

    offsets_tabs_layout->addLayout(offsets_layout);

    auto offsets_tab = tabs->widget(1);
    offsets_tab->setLayout(offsets_tabs_layout);

    main_layout->addWidget(tabs);
    main_layout->addWidget(_actions);

    setLayout(main_layout);

    setMinimumSize(800,500);

    tabs->setCurrentIndex(0);
}

void FrameRefiner::setupConnections()
{
}

void FrameRefiner::accept()
{
    close();
}

void FrameRefiner::run()
{
}

void FrameRefiner::doActions(QAbstractButton *button)
{
    auto button_role = _actions->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        run();
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
