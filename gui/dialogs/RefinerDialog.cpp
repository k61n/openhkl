//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/RefinerDialog.cpp
//! @brief     Implements classes RefinerDialog, RefinerFitWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/RefinerDialog.h"

#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/peak/Peak3D.h"
#include "gui/models/Meta.h"
#include "gui/models/Session.h"

#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSpacerItem>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>


RefinerDialog::RefinerDialog(nsx::UnitCell* unit_cell) : QDialog(), _current_frame(0)
{
    _unit_cell = unit_cell;

    setModal(true);
    _layout();
    _setUnitCellDrop();
    _setDataList();
    _setPeakList();

    _fetchAllInitialValues();
}

void RefinerDialog::_layout()
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QSplitter* splitter_item = new QSplitter();
    QWidget* top_content = new QWidget();
    QWidget* bot_content = new QWidget();

    _setSizePolicies();
    _setInputUp();
    _setInformationUp();
    _setGraphUp();

    QHBoxLayout* info_layout = new QHBoxLayout();

    info_layout->addLayout(_input_layout);
    info_layout->addLayout(_info_layout);

    top_content->setLayout(info_layout);
    bot_content->setLayout(_graph_layout);

    splitter_item->addWidget(top_content);
    splitter_item->addWidget(bot_content);
    splitter_item->setStretchFactor(0, 0);
    splitter_item->setStretchFactor(1, 1);
    splitter_item->setCollapsible(1, false);

    main_layout->addWidget(splitter_item);
}

void RefinerDialog::_setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void RefinerDialog::_setInputUp()
{
    _input_layout = new QVBoxLayout();

    QGroupBox* input_group = new QGroupBox("Refinable parameters");
    input_group->setSizePolicy(*_size_policy_box);

    QVBoxLayout* _group_layout = new QVBoxLayout(input_group);

    QScrollArea* input_scroll = new QScrollArea();
    QWidget* input_content = new QWidget();
    QVBoxLayout* input_layout = new QVBoxLayout(input_content);

    _select_uc = new QComboBox();
    _select_peaks_list = new QListWidget();
    _select_data_list = new QListWidget();
    _refine_lattice = new QCheckBox("Refine lattice");
    _refine_sample_position = new QCheckBox("Refine sample position");
    _refine_detector_position = new QCheckBox("Refine detector position");
    _refine_sample_orientation = new QCheckBox("Refine sample orientation");
    _refine_ki = new QCheckBox("Refine ki");
    _refine = new QPushButton("Refine");

    _refine_lattice->setChecked(true);
    _refine_sample_position->setChecked(true);
    _refine_sample_orientation->setChecked(true);
    _select_peaks_list->setSelectionMode(QAbstractItemView::MultiSelection);
    _select_peaks_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _select_data_list->setSelectionMode(QAbstractItemView::MultiSelection);
    _select_data_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* batch_layout = new QHBoxLayout;
    _number_of_batches = new QSpinBox();
    _number_of_batches->setValue(20);

    batch_layout->addWidget(new QLabel("Number of batches"));
    batch_layout->addWidget(_number_of_batches);

    input_layout->addWidget(new QLabel("Select unit cell:"));
    input_layout->addWidget(_select_uc);
    input_layout->addWidget(new QLabel("Select peaks:"));
    input_layout->addWidget(_select_peaks_list);
    input_layout->addWidget(new QLabel("Select data:"));
    input_layout->addWidget(_select_data_list);
    input_layout->addWidget(_refine_lattice);
    input_layout->addWidget(_refine_sample_position);
    input_layout->addWidget(_refine_detector_position);
    input_layout->addWidget(_refine_sample_orientation);
    input_layout->addWidget(_refine_ki);
    input_layout->addLayout(batch_layout);

    input_scroll->setWidget(input_content);

    input_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    input_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    input_scroll->setMinimumWidth(
        input_content->width() + qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    _group_layout->addWidget(input_scroll);
    _group_layout->addWidget(_refine);

    _input_layout->addWidget(input_group);

    connect(_refine, &QPushButton::clicked, this, &RefinerDialog::refine);

    connect(_select_data_list, &QListWidget::clicked, this, &RefinerDialog::_setDataDrop);
}

void RefinerDialog::_setInformationUp()
{
    _info_layout = new QVBoxLayout();

    QTabWidget* info_tab = new QTabWidget();
    info_tab->setSizePolicy(*_size_policy_right);

    QScrollArea* sample_tab = new QScrollArea();
    QScrollArea* uc_tab = new QScrollArea();
    QScrollArea* instrument_tab = new QScrollArea();
    QScrollArea* detector_tab = new QScrollArea();

    QWidget* sample_content = new QWidget();
    QWidget* uc_content = new QWidget();
    QWidget* instrument_content = new QWidget();
    QWidget* detector_content = new QWidget();

    sample_tab->setSizePolicy(*_size_policy_box);
    sample_content->setSizePolicy(*_size_policy_fixed);
    uc_tab->setSizePolicy(*_size_policy_box);
    uc_content->setSizePolicy(*_size_policy_fixed);
    instrument_tab->setSizePolicy(*_size_policy_box);
    instrument_content->setSizePolicy(*_size_policy_fixed);
    detector_tab->setSizePolicy(*_size_policy_box);
    detector_content->setSizePolicy(*_size_policy_fixed);

    sample_tab->setWidgetResizable(true);
    sample_tab->setWidget(sample_content);
    uc_tab->setWidgetResizable(true);
    uc_tab->setWidget(uc_content);
    instrument_tab->setWidgetResizable(true);
    instrument_tab->setWidget(instrument_content);
    detector_tab->setWidgetResizable(true);
    detector_tab->setWidget(detector_content);

    _sample_layout = new QGridLayout();
    _uc_layout = new QGridLayout();
    _instrument_layout = new QGridLayout();
    _detector_layout = new QGridLayout();

    sample_content->setLayout(_sample_layout);
    uc_content->setLayout(_uc_layout);
    instrument_content->setLayout(_instrument_layout);
    detector_content->setLayout(_detector_layout);

    _setSampleUp();
    _setDetectorUp();
    _setUnitCellUp();
    _setInstrumentUp();

    info_tab->addTab(sample_tab, "Sample");
    info_tab->addTab(uc_tab, "Unit cell");
    info_tab->addTab(instrument_tab, "Instrument");
    info_tab->addTab(detector_tab, "Detector");

    _info_layout->addWidget(info_tab);

    _navigator = new QWidget();

    QHBoxLayout* navigator_layout = new QHBoxLayout();

    _select_data = new QComboBox();
    _left = new QPushButton("-");
    _left->setSizePolicy(*_size_policy_fixed);
    _right = new QPushButton("+");
    _right->setSizePolicy(*_size_policy_fixed);
    _current_index_spin = new QSpinBox();
    _current_index_spin->setSizePolicy(*_size_policy_fixed);

    navigator_layout->addWidget(_select_data);
    navigator_layout->addWidget(new QLabel("Current frame:"));
    navigator_layout->addWidget(_left);
    navigator_layout->addWidget(_current_index_spin);
    navigator_layout->addWidget(_right);
    navigator_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    _navigator->setLayout(navigator_layout);
    _info_layout->addWidget(_navigator);
    _info_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum));

    connect(_left, &QPushButton::clicked, [=]() { _current_index_spin->stepDown(); });

    connect(_right, &QPushButton::clicked, [=]() { _current_index_spin->stepUp(); });

    connect(
        _current_index_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        [this](int val) {
            _current_frame = val;
            _setInitialValues(val);
            _setRefinedValues(val);
        });

    connect(
        _select_data, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &RefinerDialog::_selectedDataChanged);
}

void RefinerDialog::_setSampleUp()
{
    QLabel* label_ptr;

    label_ptr = new QLabel("Initial:");
    label_ptr->setAlignment(Qt::AlignRight);
    _sample_layout->addWidget(label_ptr, 0, 1, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Refined:");
    label_ptr->setAlignment(Qt::AlignRight);
    _sample_layout->addWidget(label_ptr, 0, 2, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    QCheckBox* checkbox_ptr;

    checkbox_ptr = new QCheckBox("Position X:");
    _sample_layout->addWidget(checkbox_ptr, 1, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Position Y:");
    _sample_layout->addWidget(checkbox_ptr, 2, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Position Z:");
    _sample_layout->addWidget(checkbox_ptr, 3, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation X-X:");
    _sample_layout->addWidget(checkbox_ptr, 4, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation X-Y:");
    _sample_layout->addWidget(checkbox_ptr, 5, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation X-Z:");
    _sample_layout->addWidget(checkbox_ptr, 6, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation Y-X:");
    _sample_layout->addWidget(checkbox_ptr, 7, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation Y-Y:");
    _sample_layout->addWidget(checkbox_ptr, 8, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation Y-Z:");
    _sample_layout->addWidget(checkbox_ptr, 9, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation Z-X:");
    _sample_layout->addWidget(checkbox_ptr, 10, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation Z-Y:");
    _sample_layout->addWidget(checkbox_ptr, 11, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    checkbox_ptr = new QCheckBox("Orientation Z-Z:");
    _sample_layout->addWidget(checkbox_ptr, 12, 0, 1, 1);
    checkbox_ptr->setSizePolicy(*_size_policy_widgets);

    _sample_position_X = new QScienceSpinBox(this);
    _sample_position_Y = new QScienceSpinBox(this);
    _sample_position_Z = new QScienceSpinBox(this);

    _sample_position_X_ref = new QScienceSpinBox(this);
    _sample_position_Y_ref = new QScienceSpinBox(this);
    _sample_position_Z_ref = new QScienceSpinBox(this);

    _sample_layout->addWidget(_sample_position_X, 1, 1, 1, 1);
    _sample_layout->addWidget(_sample_position_Y, 2, 1, 1, 1);
    _sample_layout->addWidget(_sample_position_Z, 3, 1, 1, 1);
    _sample_layout->addWidget(_sample_position_X_ref, 1, 2, 1, 1);
    _sample_layout->addWidget(_sample_position_Y_ref, 2, 2, 1, 1);
    _sample_layout->addWidget(_sample_position_Z_ref, 3, 2, 1, 1);

    _sample_orientation_00 = new QScienceSpinBox(this);
    _sample_orientation_01 = new QScienceSpinBox(this);
    _sample_orientation_02 = new QScienceSpinBox(this);
    _sample_orientation_10 = new QScienceSpinBox(this);
    _sample_orientation_11 = new QScienceSpinBox(this);
    _sample_orientation_12 = new QScienceSpinBox(this);
    _sample_orientation_20 = new QScienceSpinBox(this);
    _sample_orientation_21 = new QScienceSpinBox(this);
    _sample_orientation_22 = new QScienceSpinBox(this);

    _sample_orientation_00_ref = new QScienceSpinBox(this);
    _sample_orientation_01_ref = new QScienceSpinBox(this);
    _sample_orientation_02_ref = new QScienceSpinBox(this);
    _sample_orientation_10_ref = new QScienceSpinBox(this);
    _sample_orientation_11_ref = new QScienceSpinBox(this);
    _sample_orientation_12_ref = new QScienceSpinBox(this);
    _sample_orientation_20_ref = new QScienceSpinBox(this);
    _sample_orientation_21_ref = new QScienceSpinBox(this);
    _sample_orientation_22_ref = new QScienceSpinBox(this);

    _sample_layout->addWidget(_sample_orientation_00, 4, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_01, 5, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_02, 6, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_10, 7, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_11, 8, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_12, 9, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_20, 10, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_21, 11, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_22, 12, 1, 1, 1);
    _sample_layout->addWidget(_sample_orientation_00_ref, 4, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_01_ref, 5, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_02_ref, 6, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_10_ref, 7, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_11_ref, 8, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_12_ref, 9, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_20_ref, 10, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_21_ref, 11, 2, 1, 1);
    _sample_layout->addWidget(_sample_orientation_22_ref, 12, 2, 1, 1);

    QList<QScienceSpinBox*> spin_boxes =
        _sample_layout->parentWidget()->findChildren<QScienceSpinBox*>();

    foreach (QScienceSpinBox* spin_box, spin_boxes) {
        spin_box->setSizePolicy(*_size_policy_fixed);
        spin_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spin_box->setReadOnly(true);
        // spin_box->setDecimals(6);
        // spin_box->setMinimum(-1e6);
        // spin_box->setMaximum(1e6);
    }

    QList<QCheckBox*> checkboxes = _sample_layout->parentWidget()->findChildren<QCheckBox*>();

    foreach (QCheckBox* checkbox, checkboxes) {
        connect(checkbox, &QCheckBox::stateChanged, this, &RefinerDialog::_plot);
    }
}

void RefinerDialog::_setDetectorUp()
{
    QLabel* label_ptr;

    label_ptr = new QLabel("Initial:");
    label_ptr->setAlignment(Qt::AlignCenter);
    _detector_layout->addWidget(label_ptr, 0, 1, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Refined:");
    label_ptr->setAlignment(Qt::AlignCenter);
    _detector_layout->addWidget(label_ptr, 0, 2, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Position X:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Position Y:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Position Z:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation X-X:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation X-Y:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 5, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation X-Z:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 6, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation Y-X:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 7, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation Y-Y:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 8, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation Y-Z:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 9, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation Z-X:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 10, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation Z-Y:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 11, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Orientation Z-Z:");
    label_ptr->setAlignment(Qt::AlignRight);
    _detector_layout->addWidget(label_ptr, 12, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _detector_position_X = new QScienceSpinBox(this);
    _detector_position_Y = new QScienceSpinBox(this);
    _detector_position_Z = new QScienceSpinBox(this);

    _detector_position_X_ref = new QScienceSpinBox(this);
    _detector_position_Y_ref = new QScienceSpinBox(this);
    _detector_position_Z_ref = new QScienceSpinBox(this);

    _detector_layout->addWidget(_detector_position_X, 1, 1, 1, 1);
    _detector_layout->addWidget(_detector_position_Y, 2, 1, 1, 1);
    _detector_layout->addWidget(_detector_position_Z, 3, 1, 1, 1);
    _detector_layout->addWidget(_detector_position_X_ref, 1, 2, 1, 1);
    _detector_layout->addWidget(_detector_position_Y_ref, 2, 2, 1, 1);
    _detector_layout->addWidget(_detector_position_Z_ref, 3, 2, 1, 1);

    _detector_orientation_00 = new QScienceSpinBox(this);
    _detector_orientation_01 = new QScienceSpinBox(this);
    _detector_orientation_02 = new QScienceSpinBox(this);
    _detector_orientation_10 = new QScienceSpinBox(this);
    _detector_orientation_11 = new QScienceSpinBox(this);
    _detector_orientation_12 = new QScienceSpinBox(this);
    _detector_orientation_20 = new QScienceSpinBox(this);
    _detector_orientation_21 = new QScienceSpinBox(this);
    _detector_orientation_22 = new QScienceSpinBox(this);

    _detector_orientation_00_ref = new QScienceSpinBox(this);
    _detector_orientation_01_ref = new QScienceSpinBox(this);
    _detector_orientation_02_ref = new QScienceSpinBox(this);
    _detector_orientation_10_ref = new QScienceSpinBox(this);
    _detector_orientation_11_ref = new QScienceSpinBox(this);
    _detector_orientation_12_ref = new QScienceSpinBox(this);
    _detector_orientation_20_ref = new QScienceSpinBox(this);
    _detector_orientation_21_ref = new QScienceSpinBox(this);
    _detector_orientation_22_ref = new QScienceSpinBox(this);

    _detector_layout->addWidget(_detector_orientation_00, 4, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_01, 5, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_02, 6, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_10, 7, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_11, 8, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_12, 9, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_20, 10, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_21, 11, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_22, 12, 1, 1, 1);
    _detector_layout->addWidget(_detector_orientation_00_ref, 4, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_01_ref, 5, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_02_ref, 6, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_10_ref, 7, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_11_ref, 8, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_12_ref, 9, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_20_ref, 10, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_21_ref, 11, 2, 1, 1);
    _detector_layout->addWidget(_detector_orientation_22_ref, 12, 2, 1, 1);

    QList<QScienceSpinBox*> spin_boxes =
        _detector_layout->parentWidget()->findChildren<QScienceSpinBox*>();

    foreach (QScienceSpinBox* spin_box, spin_boxes) {
        spin_box->setSizePolicy(*_size_policy_fixed);
        spin_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spin_box->setReadOnly(true);
        // spin_box->setDecimals(6);
        // spin_box->setMinimum(-1e6);
        // spin_box->setMaximum(1e6);
    }
}

void RefinerDialog::_setUnitCellUp()
{
    QLabel* label_ptr;

    label_ptr = new QLabel("Initial:");
    label_ptr->setAlignment(Qt::AlignCenter);
    _uc_layout->addWidget(label_ptr, 0, 1, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Refined:");
    label_ptr->setAlignment(Qt::AlignCenter);
    _uc_layout->addWidget(label_ptr, 0, 2, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("a:");
    label_ptr->setAlignment(Qt::AlignRight);
    _uc_layout->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("b:");
    label_ptr->setAlignment(Qt::AlignRight);
    _uc_layout->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("c:");
    label_ptr->setAlignment(Qt::AlignRight);
    _uc_layout->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel(QString((QChar)0x03B1) + ":");
    label_ptr->setAlignment(Qt::AlignRight);
    _uc_layout->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel(QString((QChar)0x03B2) + ":");
    label_ptr->setAlignment(Qt::AlignRight);
    _uc_layout->addWidget(label_ptr, 5, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel(QString((QChar)0x03B3) + ":");
    label_ptr->setAlignment(Qt::AlignRight);
    _uc_layout->addWidget(label_ptr, 6, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _uc_a = new QScienceSpinBox(this);
    _uc_b = new QScienceSpinBox(this);
    _uc_c = new QScienceSpinBox(this);
    _uc_a_ref = new QScienceSpinBox(this);
    _uc_b_ref = new QScienceSpinBox(this);
    _uc_c_ref = new QScienceSpinBox(this);

    _uc_alpha = new QScienceSpinBox(this);
    _uc_beta = new QScienceSpinBox(this);
    _uc_gamma = new QScienceSpinBox(this);
    _uc_alpha_ref = new QScienceSpinBox(this);
    _uc_beta_ref = new QScienceSpinBox(this);
    _uc_gamma_ref = new QScienceSpinBox(this);

    _uc_layout->addWidget(_uc_a, 1, 1, 1, 1);
    _uc_layout->addWidget(_uc_b, 2, 1, 1, 1);
    _uc_layout->addWidget(_uc_c, 3, 1, 1, 1);
    _uc_layout->addWidget(_uc_a_ref, 1, 2, 1, 1);
    _uc_layout->addWidget(_uc_b_ref, 2, 2, 1, 1);
    _uc_layout->addWidget(_uc_c_ref, 3, 2, 1, 1);
    _uc_layout->addWidget(_uc_alpha, 4, 1, 1, 1);
    _uc_layout->addWidget(_uc_beta, 5, 1, 1, 1);
    _uc_layout->addWidget(_uc_gamma, 6, 1, 1, 1);
    _uc_layout->addWidget(_uc_alpha_ref, 4, 2, 1, 1);
    _uc_layout->addWidget(_uc_beta_ref, 5, 2, 1, 1);
    _uc_layout->addWidget(_uc_gamma_ref, 6, 2, 1, 1);

    QList<QScienceSpinBox*> spin_boxes =
        _uc_layout->parentWidget()->findChildren<QScienceSpinBox*>();

    foreach (QScienceSpinBox* spin_box, spin_boxes) {
        spin_box->setSizePolicy(*_size_policy_fixed);
        spin_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spin_box->setReadOnly(true);
        // spin_box->setDecimals(6);
    }
}

void RefinerDialog::_setInstrumentUp()
{
    QLabel* label_ptr;

    label_ptr = new QLabel("Initial:");
    label_ptr->setAlignment(Qt::AlignRight);
    _instrument_layout->addWidget(label_ptr, 0, 1, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Refined:");
    label_ptr->setAlignment(Qt::AlignRight);
    _instrument_layout->addWidget(label_ptr, 0, 2, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("ni X:");
    label_ptr->setAlignment(Qt::AlignRight);
    _instrument_layout->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("ni Y:");
    label_ptr->setAlignment(Qt::AlignRight);
    _instrument_layout->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("ni Z:");
    label_ptr->setAlignment(Qt::AlignRight);
    _instrument_layout->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Wavelength:");
    label_ptr->setAlignment(Qt::AlignRight);
    _instrument_layout->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Wavelegnth offset:");
    label_ptr->setAlignment(Qt::AlignRight);
    _instrument_layout->addWidget(label_ptr, 5, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _ni_X = new QScienceSpinBox(this);
    _ni_Y = new QScienceSpinBox(this);
    _ni_Z = new QScienceSpinBox(this);
    _wavelength = new QScienceSpinBox(this);
    _wavelength_offset = new QScienceSpinBox(this);

    _ni_X_ref = new QScienceSpinBox(this);
    _ni_Y_ref = new QScienceSpinBox(this);
    _ni_Z_ref = new QScienceSpinBox(this);
    _wavelength_ref = new QScienceSpinBox(this);
    _wavelength_offset_ref = new QScienceSpinBox(this);

    _instrument_layout->addWidget(_ni_X, 1, 1, 1, 1);
    _instrument_layout->addWidget(_ni_Y, 2, 1, 1, 1);
    _instrument_layout->addWidget(_ni_Z, 3, 1, 1, 1);
    _instrument_layout->addWidget(_wavelength, 4, 1, 1, 1);
    _instrument_layout->addWidget(_wavelength_offset, 5, 1, 1, 1);
    _instrument_layout->addWidget(_ni_X_ref, 1, 2, 1, 1);
    _instrument_layout->addWidget(_ni_Y_ref, 2, 2, 1, 1);
    _instrument_layout->addWidget(_ni_Z_ref, 3, 2, 1, 1);
    _instrument_layout->addWidget(_wavelength_ref, 4, 2, 1, 1);
    _instrument_layout->addWidget(_wavelength_offset_ref, 5, 2, 1, 1);

    QList<QScienceSpinBox*> spin_boxes =
        _instrument_layout->parentWidget()->findChildren<QScienceSpinBox*>();

    foreach (QScienceSpinBox* spin_box, spin_boxes) {
        spin_box->setSizePolicy(*_size_policy_fixed);
        spin_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spin_box->setReadOnly(true);
        // spin_box->setDecimals(6);
    }
}

void RefinerDialog::_setGraphUp()
{
    _graph_layout = new QVBoxLayout();

    _visualise_plot = new SXPlot();
    _visualise_plot->xAxis->setLabel("Frame");
    _visualise_plot->yAxis->setLabel("Value");
    _visualise_plot->setSizePolicy(*_size_policy_right);

    _graph_layout->addWidget(_visualise_plot);
}

void RefinerDialog::_setUnitCellDrop()
{
    _select_uc->blockSignals(true);
    QStringList uc_list = gSession->selectedExperiment()->getUnitCellNames();
    if (!uc_list.isEmpty()) {
        _select_uc->addItems(uc_list);
        _select_uc->setCurrentIndex(0);
    }
    _select_uc->blockSignals(false);
}

void RefinerDialog::_setPeakList()
{
    _select_peaks_list->blockSignals(true);
    QStringList peak_list = gSession->selectedExperiment()->getPeakListNames();
    if (!peak_list.isEmpty())
        _select_peaks_list->addItems(peak_list);
    _select_peaks_list->blockSignals(false);
}

void RefinerDialog::_setDataList()
{
    _select_data_list->blockSignals(true);
    _data_list = gSession->selectedExperiment()->allData();
    if (!_data_list.isEmpty()) {
        for (nsx::sptrDataSet data : _data_list) {
            QFileInfo fileinfo(QString::fromStdString(data->filename()));
            _select_data_list->addItem(fileinfo.baseName());
        }
    }
    _select_data_list->blockSignals(false);
}

void RefinerDialog::_setDataDrop()
{
    _select_data->blockSignals(true);
    QString last_text = _select_data->currentText();
    _select_data->clear();
    QList<QListWidgetItem*> selected_data = _select_data_list->selectedItems();
    if (!selected_data.isEmpty()) {
        for (QListWidgetItem* data_item : selected_data)
            _select_data->addItem(data_item->text());
    } else {
        return;
    }

    int index = _select_data->findText(last_text);
    if (index == -1)
        _select_data->setCurrentIndex(0);
    else
        _select_data->setCurrentIndex(index);
    _select_data->blockSignals(false);

    _selectedDataChanged();
}

void RefinerDialog::_selectedDataChanged()
{
    nsx::sptrDataSet data_set = gSession->selectedExperiment()->experiment()->getData(
        _select_data->currentText().toStdString());
    _current_index_spin->setMaximum(data_set->nFrames() - 1);
    if (_current_index_spin->value() > data_set->nFrames() - 1)
        _current_index_spin->setValue(data_set->nFrames() - 1);
    else
        _current_index_spin->setValue(_current_index_spin->value());

    _setInitialValues(_current_index_spin->value());
    _setRefinedValues(_current_index_spin->value());
    _plot();
}

void RefinerDialog::_fetchAllInitialValues()
{
    _sample_orientations.clear();
    _detector_orientations.clear();
    _sample_positions.clear();
    _detector_positions.clear();
    _nis.clear();
    _wavelengths.clear();

    nsx::UnitCell* unit_cell = gSession->selectedExperiment()->experiment()->getUnitCell(
        _select_uc->currentText().toStdString());

    if (!unit_cell)
        return;

    QList<nsx::sptrDataSet> data_set_list = gSession->selectedExperiment()->allData();

    for (nsx::sptrDataSet data_set : data_set_list) {
        const nsx::InstrumentStateList& instrument_states = data_set->instrumentStates();

        std::vector<Eigen::Matrix3d> sample_orientations_temp;
        std::vector<Eigen::Matrix3d> detector_orientations_temp;
        std::vector<Eigen::Vector3d> sample_positions_temp;
        std::vector<Eigen::Vector3d> detector_positions_temp;
        std::vector<Eigen::Vector3d> nis_temp;
        std::vector<double> wavelengths_temp;
        std::vector<double> wavelength_offsets_temp;
        Eigen::Vector3d uc_dims_temp;
        Eigen::Vector3d uc_angles_temp;

        for (nsx::InstrumentState state : instrument_states) {
            sample_orientations_temp.push_back(state.sampleOrientationMatrix());
            detector_orientations_temp.push_back(state.detectorOrientation);
            sample_positions_temp.push_back(state.samplePosition);
            detector_positions_temp.push_back(state.detectorPositionOffset);
            nis_temp.push_back(state.ni);
            wavelengths_temp.push_back(state.wavelength);
            wavelength_offsets_temp.push_back(state.wavelength * (state.ni.norm() - 1.0));
        }

        nsx::UnitCellCharacter unitcharacter = unit_cell->character();
        uc_dims_temp = Eigen::Vector3d {unitcharacter.a, unitcharacter.b, unitcharacter.c};
        uc_angles_temp =
            Eigen::Vector3d {unitcharacter.alpha, unitcharacter.beta, unitcharacter.gamma};

        _sample_orientations.emplace(data_set, sample_orientations_temp);
        _detector_orientations.emplace(data_set, detector_orientations_temp);
        _sample_positions.emplace(data_set, sample_positions_temp);
        _detector_positions.emplace(data_set, detector_positions_temp);
        _nis.emplace(data_set, nis_temp);
        _wavelengths.emplace(data_set, wavelengths_temp);
        _wavelength_offsets.emplace(data_set, wavelength_offsets_temp);
        _uc_dims.emplace(data_set, uc_dims_temp);
        _uc_angles.emplace(data_set, uc_angles_temp);
    }

    _setInitialValues(_current_frame);
}

void RefinerDialog::_fetchAllRefinedValues()
{
    _sample_orientations_ref.clear();
    _detector_orientations_ref.clear();
    _sample_positions_ref.clear();
    _detector_positions_ref.clear();
    _nis_ref.clear();
    _wavelengths_ref.clear();

    nsx::UnitCell* unit_cell = gSession->selectedExperiment()->experiment()->getUnitCell(
        _select_uc->currentText().toStdString());

    nsx::sptrDataSet data_set = gSession->selectedExperiment()->experiment()->getData(
        _select_data->currentText().toStdString());
    QList<nsx::sptrDataSet> data_set_list = gSession->selectedExperiment()->allData();

    if (!unit_cell)
        return;

    for (nsx::sptrDataSet data_set : data_set_list) {
        const nsx::InstrumentStateList& instrument_states = data_set->instrumentStates();

        std::vector<Eigen::Matrix3d> sample_orientations_temp;
        std::vector<Eigen::Matrix3d> detector_orientations_temp;
        std::vector<Eigen::Vector3d> sample_positions_temp;
        std::vector<Eigen::Vector3d> detector_positions_temp;
        std::vector<Eigen::Vector3d> nis_temp;
        std::vector<double> wavelengths_temp;
        std::vector<double> wavelength_offsets_temp;
        Eigen::Vector3d uc_dims_temp;
        Eigen::Vector3d uc_angles_temp;

        for (nsx::InstrumentState state : instrument_states) {
            sample_orientations_temp.push_back(state.sampleOrientationMatrix());
            detector_orientations_temp.push_back(state.detectorOrientation);
            sample_positions_temp.push_back(state.samplePosition);
            detector_positions_temp.push_back(state.detectorPositionOffset);
            nis_temp.push_back(state.ni);
            wavelengths_temp.push_back(state.wavelength);
            wavelength_offsets_temp.push_back(state.wavelength * (state.ni.norm() - 1.0));
        }

        nsx::UnitCellCharacter unitcharacter = unit_cell->character();
        uc_dims_temp = Eigen::Vector3d {unitcharacter.a, unitcharacter.b, unitcharacter.c};
        uc_angles_temp =
            Eigen::Vector3d {unitcharacter.alpha, unitcharacter.beta, unitcharacter.gamma};

        _sample_orientations_ref.emplace(data_set, sample_orientations_temp);
        _detector_orientations_ref.emplace(data_set, detector_orientations_temp);
        _sample_positions_ref.emplace(data_set, sample_positions_temp);
        _detector_positions_ref.emplace(data_set, detector_positions_temp);
        _nis_ref.emplace(data_set, nis_temp);
        _wavelengths_ref.emplace(data_set, wavelengths_temp);
        _wavelength_offsets_ref.emplace(data_set, wavelength_offsets_temp);
        _uc_dims_ref.emplace(data_set, uc_dims_temp);
        _uc_angles_ref.emplace(data_set, uc_angles_temp);
    }

    _setRefinedValues(_current_frame);
}

void RefinerDialog::_setInitialValues(int frame)
{

    QString temp_text = _select_data->currentText();

    if (temp_text.isEmpty())
        return;

    nsx::sptrDataSet data_set =
        gSession->selectedExperiment()->experiment()->getData(temp_text.toStdString());

    _sample_orientation_00->setValue(_sample_orientations[data_set][frame](0, 0));
    _sample_orientation_01->setValue(_sample_orientations[data_set][frame](0, 1));
    _sample_orientation_02->setValue(_sample_orientations[data_set][frame](0, 2));
    _sample_orientation_10->setValue(_sample_orientations[data_set][frame](1, 0));
    _sample_orientation_11->setValue(_sample_orientations[data_set][frame](1, 1));
    _sample_orientation_12->setValue(_sample_orientations[data_set][frame](1, 2));
    _sample_orientation_20->setValue(_sample_orientations[data_set][frame](2, 0));
    _sample_orientation_21->setValue(_sample_orientations[data_set][frame](2, 1));
    _sample_orientation_22->setValue(_sample_orientations[data_set][frame](2, 2));

    _detector_orientation_00->setValue(_detector_orientations[data_set][frame](0, 0));
    _detector_orientation_01->setValue(_detector_orientations[data_set][frame](0, 1));
    _detector_orientation_02->setValue(_detector_orientations[data_set][frame](0, 2));
    _detector_orientation_10->setValue(_detector_orientations[data_set][frame](1, 0));
    _detector_orientation_11->setValue(_detector_orientations[data_set][frame](1, 1));
    _detector_orientation_12->setValue(_detector_orientations[data_set][frame](1, 2));
    _detector_orientation_20->setValue(_detector_orientations[data_set][frame](2, 0));
    _detector_orientation_21->setValue(_detector_orientations[data_set][frame](2, 1));
    _detector_orientation_22->setValue(_detector_orientations[data_set][frame](2, 2));

    _sample_position_X->setValue(_sample_positions[data_set][frame][0]);
    _sample_position_Y->setValue(_sample_positions[data_set][frame][1]);
    _sample_position_Z->setValue(_sample_positions[data_set][frame][2]);

    _detector_position_X->setValue(_detector_positions[data_set][frame][0]);
    _detector_position_Y->setValue(_detector_positions[data_set][frame][1]);
    _detector_position_Z->setValue(_detector_positions[data_set][frame][2]);

    _ni_X->setValue(_nis[data_set][frame][0]);
    _ni_Y->setValue(_nis[data_set][frame][1]);
    _ni_Z->setValue(_nis[data_set][frame][2]);
    _wavelength->setValue(_wavelengths[data_set][frame]);
    _wavelength_offset->setValue(_wavelength_offsets[data_set][frame]);

    _uc_a->setValue(_uc_dims[data_set][0]);
    _uc_b->setValue(_uc_dims[data_set][1]);
    _uc_c->setValue(_uc_dims[data_set][2]);
    _uc_alpha->setValue(_uc_angles[data_set][0]);
    _uc_beta->setValue(_uc_angles[data_set][1]);
    _uc_gamma->setValue(_uc_angles[data_set][2]);
}


void RefinerDialog::_setRefinedValues(int frame)
{
    QString temp_text = _select_data->currentText();

    if (temp_text.isEmpty())
        return;

    nsx::sptrDataSet data_set =
        gSession->selectedExperiment()->experiment()->getData(temp_text.toStdString());

    if (refiners.find(data_set) == refiners.end())
        return;

    _sample_orientation_00_ref->setValue(_sample_orientations_ref[data_set][frame](0, 0));
    _sample_orientation_01_ref->setValue(_sample_orientations_ref[data_set][frame](0, 1));
    _sample_orientation_02_ref->setValue(_sample_orientations_ref[data_set][frame](0, 2));
    _sample_orientation_10_ref->setValue(_sample_orientations_ref[data_set][frame](1, 0));
    _sample_orientation_11_ref->setValue(_sample_orientations_ref[data_set][frame](1, 1));
    _sample_orientation_12_ref->setValue(_sample_orientations_ref[data_set][frame](1, 2));
    _sample_orientation_20_ref->setValue(_sample_orientations_ref[data_set][frame](2, 0));
    _sample_orientation_21_ref->setValue(_sample_orientations_ref[data_set][frame](2, 1));
    _sample_orientation_22_ref->setValue(_sample_orientations_ref[data_set][frame](2, 2));

    _detector_orientation_00_ref->setValue(_detector_orientations_ref[data_set][frame](0, 0));
    _detector_orientation_01_ref->setValue(_detector_orientations_ref[data_set][frame](0, 1));
    _detector_orientation_02_ref->setValue(_detector_orientations_ref[data_set][frame](0, 2));
    _detector_orientation_10_ref->setValue(_detector_orientations_ref[data_set][frame](1, 0));
    _detector_orientation_11_ref->setValue(_detector_orientations_ref[data_set][frame](1, 1));
    _detector_orientation_12_ref->setValue(_detector_orientations_ref[data_set][frame](1, 2));
    _detector_orientation_20_ref->setValue(_detector_orientations_ref[data_set][frame](2, 0));
    _detector_orientation_21_ref->setValue(_detector_orientations_ref[data_set][frame](2, 1));
    _detector_orientation_22_ref->setValue(_detector_orientations_ref[data_set][frame](2, 2));

    _sample_position_X_ref->setValue(_sample_positions_ref[data_set][frame][0]);
    _sample_position_Y_ref->setValue(_sample_positions_ref[data_set][frame][1]);
    _sample_position_Z_ref->setValue(_sample_positions_ref[data_set][frame][2]);

    _detector_position_X_ref->setValue(_detector_positions_ref[data_set][frame][0]);
    _detector_position_Y_ref->setValue(_detector_positions_ref[data_set][frame][1]);
    _detector_position_Z_ref->setValue(_detector_positions_ref[data_set][frame][2]);

    _ni_X_ref->setValue(_nis_ref[data_set][frame][0]);
    _ni_Y_ref->setValue(_nis_ref[data_set][frame][1]);
    _ni_Z_ref->setValue(_nis_ref[data_set][frame][2]);
    _wavelength_ref->setValue(_wavelengths_ref[data_set][frame]);
    _wavelength_offset_ref->setValue(_wavelength_offsets_ref[data_set][frame]);

    _uc_a_ref->setValue(_uc_dims_ref[data_set][0]);
    _uc_b_ref->setValue(_uc_dims_ref[data_set][1]);
    _uc_c_ref->setValue(_uc_dims_ref[data_set][2]);
    _uc_alpha_ref->setValue(_uc_angles_ref[data_set][0]);
    _uc_beta_ref->setValue(_uc_angles_ref[data_set][1]);
    _uc_gamma_ref->setValue(_uc_angles_ref[data_set][2]);
}

void RefinerDialog::refine()
{
    int n_batches = _number_of_batches->value();
    QList<QListWidgetItem*> selected_peaks = _select_peaks_list->selectedItems();
    QList<QListWidgetItem*> selected_data = _select_data_list->selectedItems();
    nsx::UnitCell* unit_cell = gSession->selectedExperiment()->experiment()->getUnitCell(
        _select_uc->currentText().toStdString());

    if (n_batches == 0) {
        qDebug() << "[ERROR] 0 batch number";
        return;
    }

    if (selected_peaks.isEmpty()) {
        qDebug() << "[ERROR] No peaks selected";
        return;
    }

    if (selected_data.isEmpty()) {
        qDebug() << "[ERROR] No data selected";
        return;
    }

    if (!unit_cell) {
        qDebug() << "[ERROR] No unit cell set for the selected peaks";
        return;
    }

    refiners.clear();

    for (QListWidgetItem* data_item : selected_data) {

        std::string data_name = data_item->text().toStdString();
        nsx::sptrDataSet data = gSession->selectedExperiment()->experiment()->getData(data_name);
        std::vector<nsx::Peak3D*> reference_peaks;
        std::vector<nsx::Peak3D*> predicted_peaks;

        // Keep the peak that belong to this data and split them
        // between the found and predicted ones
        for (QListWidgetItem* peak_collection_item : selected_peaks) {

            std::string peak_name = peak_collection_item->text().toStdString();
            nsx::PeakCollection* peak_collection =
                gSession->selectedExperiment()->experiment()->getPeakCollection(peak_name);
            std::vector<nsx::Peak3D*> temp_peaks = peak_collection->getPeakList();

            for (nsx::Peak3D* peak : temp_peaks) {
                if (peak->dataSet() != data)
                    continue;
                if (peak->predicted())
                    predicted_peaks.push_back(peak);
                else
                    reference_peaks.push_back(peak);
            }
        }

        qDebug() << "[INFO] " + QString::number(reference_peaks.size()) + " splitted into "
                + QString::number(n_batches) + "refining batches.";

        nsx::InstrumentStateList& states = data->instrumentStates();

        nsx::Refiner refiner(states, unit_cell, reference_peaks, n_batches);

        if (_refine_lattice->isChecked()) {
            refiner.refineUB();
            qDebug() << "[INFO] Refining UB matrix";
        }

        if (_refine_sample_position->isChecked()) {
            refiner.refineSamplePosition();
            qDebug() << "[INFO] Refinining sample position";
        }

        if (_refine_detector_position->isChecked()) {
            refiner.refineDetectorOffset();
            qDebug() << "[INFO] Refinining detector position";
        }

        if (_refine_sample_orientation->isChecked()) {
            refiner.refineSampleOrientation();
            qDebug() << "[INFO] Refinining sample orientation";
        }

        if (_refine_ki->isChecked()) {
            refiner.refineKi();
            qDebug() << "[INFO] Refining Ki";
        }

        bool success = refiner.refine();

        if (success) {
            qDebug() << "[INFO] Successfully refined parameters for numor "
                    + QString::fromStdString(data->filename());
            int updated = refiner.updatePredictions(predicted_peaks);
            refiners.emplace(data, std::move(refiner));
            qDebug() << "[INFO] done; updated " + QString::number(updated) + " peaks";
        } else {
            qDebug() << "[INFO] Failed to refine parameters for numor "
                    + QString::fromStdString(data->filename());
        }
    }

    gSession->onExperimentChanged();
    _fetchAllRefinedValues();
    _plot();
}

void RefinerDialog::accept()
{
    close();
}

void RefinerDialog::_plot()
{

    QString temp_text = _select_data->currentText();

    if (temp_text.isEmpty())
        return;

    nsx::sptrDataSet data_set =
        gSession->selectedExperiment()->experiment()->getData(temp_text.toStdString());

    std::vector<std::string> selected_text;

    QList<QCheckBox*> checkboxes = _sample_layout->parentWidget()->findChildren<QCheckBox*>();

    foreach (QCheckBox* checkbox, checkboxes) {
        if (checkbox->isChecked())
            selected_text.push_back(checkbox->text().toStdString());
    }

    _visualise_plot->clearGraphs();

    int graph_num = 0;

    std::vector<QColor> colors {Qt::black,     Qt::blue,        Qt::red,
                                Qt::darkGreen, Qt::darkMagenta, Qt::darkYellow};

    std::vector<std::string> position_names {"Position X:", "Position Y:", "Position Z:"};

    // Positions
    for (int index = 0; index < position_names.size(); ++index) {
        if (std::find(selected_text.begin(), selected_text.end(), position_names[index])
            != selected_text.end()) {
            QPen pen;
            pen.setColor(colors[index % 6]);
            pen.setWidth(3);

            QVector<double> x_ini(_sample_positions[data_set].size());
            QVector<double> y_ini(_sample_positions[data_set].size());

            for (int i = 0; i < _sample_positions[data_set].size(); ++i) {
                x_ini[i] = i;
                y_ini[i] = _sample_positions[data_set][i][index];
            }
            _visualise_plot->addGraph();
            _visualise_plot->graph(graph_num)->setData(x_ini, y_ini);
            _visualise_plot->graph(graph_num)->setPen(pen);
            _visualise_plot->graph(graph_num)->setName(QString::fromStdString(
                position_names[index].substr(0, position_names[index].size() - 1)));
            ++graph_num;

            if (!(refiners.find(data_set) == refiners.end())) {
                QVector<double> x_ref(_sample_positions_ref[data_set].size());
                QVector<double> y_ref(_sample_positions_ref[data_set].size());

                for (int i = 0; i < _sample_positions_ref[data_set].size(); ++i) {
                    x_ref[i] = i;
                    y_ref[i] = _sample_positions_ref[data_set][i][index];
                }

                pen.setDashPattern(QVector<qreal>() << 5 << 2);

                _visualise_plot->addGraph();
                _visualise_plot->graph(graph_num)->setData(x_ref, y_ref);
                _visualise_plot->graph(graph_num)->setPen(pen);
                _visualise_plot->graph(graph_num)->setName(QString::fromStdString(
                    position_names[index].substr(0, position_names[index].size() - 1)
                    + " refined"));
                ++graph_num;
            }
        }
    }

    std::vector<std::string> orientation_names {
        "Orientation X-X:", "Orientation X-Y:", "Orientation X-Z:",
        "Orientation Y-X:", "Orientation Y-Y:", "Orientation Y-Z:",
        "Orientation Z-X:", "Orientation Z-Y:", "Orientation Z-Z:"};

    // Orientations
    for (int index = 0; index < orientation_names.size(); ++index) {
        if (std::find(selected_text.begin(), selected_text.end(), orientation_names[index])
            != selected_text.end()) {
            QPen pen;
            pen.setColor(colors[index % 6]);
            pen.setWidth(3);

            QVector<double> x_ini(_sample_orientations[data_set].size());
            QVector<double> y_ini(_sample_orientations[data_set].size());

            for (int i = 0; i < _sample_orientations[data_set].size(); ++i) {
                x_ini[i] = i;
                y_ini[i] = _sample_orientations[data_set][i](index);
            }
            _visualise_plot->addGraph();
            _visualise_plot->graph(graph_num)->setData(x_ini, y_ini);
            _visualise_plot->graph(graph_num)->setPen(pen);
            _visualise_plot->graph(graph_num)->setName(QString::fromStdString(
                orientation_names[index].substr(0, orientation_names[index].size() - 1)));
            ++graph_num;

            if (!(refiners.find(data_set) == refiners.end())) {
                QVector<double> x_ref(_sample_orientations_ref[data_set].size());
                QVector<double> y_ref(_sample_orientations_ref[data_set].size());

                for (int i = 0; i < _sample_orientations_ref[data_set].size(); ++i) {
                    x_ref[i] = i;
                    y_ref[i] = _sample_orientations_ref[data_set][i](index);
                }

                pen.setDashPattern(QVector<qreal>() << 5 << 2);

                _visualise_plot->addGraph();
                _visualise_plot->graph(graph_num)->setData(x_ref, y_ref);
                _visualise_plot->graph(graph_num)->setPen(pen);
                _visualise_plot->graph(graph_num)->setName(QString::fromStdString(
                    orientation_names[index].substr(0, orientation_names[index].size() - 1)
                    + " refined"));
                ++graph_num;
            }
        }
    }
    _visualise_plot->replot();
}
