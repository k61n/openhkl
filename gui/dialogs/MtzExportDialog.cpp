//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/MtzExportDialog.cpp
//! @brief     Implements class MtzExportDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "MtzExportDialog.h"
#include "core/statistics/PeakMerger.h"

namespace ohkl {

MtzExportDialog::MtzExportDialog(bool merged_data)
    : _selected_data(""), _selected_pc(""), _comment(""), _merged_data(merged_data)
{
    setModal(true);

    setWindowTitle(QString("MTZ Export"));

    QLabel* lab_data = new QLabel(QString("Dateset:"));
    QLabel* lab_peaks = new QLabel(QString("PeakCollection:"));
    QLabel* lab_rb_unmerged = new QLabel(QString("Unmerged data:"));
    QLabel* lab_rb_merged = new QLabel(QString("Merged data:"));

    QGridLayout* gridLayout = new QGridLayout(this);

    _datacombo = new DataComboBox();
    _peakcombo = new QComboBox();

    _rb_group = new QGroupBox();
    _rb_merged = new QRadioButton();
    _rb_unmerged = new QRadioButton();
    //_rb_group->setTitle("Export");

    QHBoxLayout* hlay_cmb = new QHBoxLayout();
    QHBoxLayout* hlay_rb = new QHBoxLayout(_rb_group);
    QVBoxLayout* vlay = new QVBoxLayout();


    vlay->setSpacing(10);

    hlay_cmb->addWidget(lab_data);
    hlay_cmb->addWidget(_datacombo);
    hlay_cmb->addWidget(lab_peaks);
    hlay_cmb->addWidget(_peakcombo);

    hlay_rb->addWidget(lab_rb_unmerged);
    hlay_rb->addWidget(_rb_unmerged);
    hlay_rb->addWidget(lab_rb_merged);
    hlay_rb->addWidget(_rb_merged);

    vlay->addLayout(hlay_cmb);
    vlay->addWidget(_rb_group);

    gridLayout->addLayout(vlay, 0, 0);

    _merge_param_box = new QGroupBox();
    _drange_min = new QDoubleSpinBox();
    _drange_max = new QDoubleSpinBox();
    _frame_min = new QDoubleSpinBox();
    _frame_max = new QDoubleSpinBox();
    _shell_res = new QDoubleSpinBox();
    _friedel = new QCheckBox();

    QLabel* lab_drange = new QLabel("Resolution (d) range:");
    QLabel* lab_img_range = new QLabel("Image range:");
    QLabel* lab_nshells = new QLabel("Number res. shells:");
    QLabel* lab_friedel = new QLabel("Friedel:");
    QLabel* lab_merge_param = new QLabel("Merge Parameter:");
    QLabel* lab_min = new QLabel("Min");
    QLabel* lab_max = new QLabel("Max");

    //_merge_param_box->setTitle("Merge Parameters");

    QVBoxLayout* vlayer_merge_params = new QVBoxLayout(_merge_param_box);
    QHBoxLayout* hlayer_minmax = new QHBoxLayout();
    QHBoxLayout* hlayer_drange = new QHBoxLayout();
    QHBoxLayout* hlayer_frame = new QHBoxLayout();
    QHBoxLayout* hlayer_shell = new QHBoxLayout();
    QHBoxLayout* hlayer_fridel = new QHBoxLayout();

    vlayer_merge_params->addLayout(hlayer_minmax);
    vlayer_merge_params->addLayout(hlayer_drange);
    vlayer_merge_params->addLayout(hlayer_frame);
    vlayer_merge_params->addLayout(hlayer_shell);
    vlayer_merge_params->addLayout(hlayer_fridel);

    hlayer_minmax->addWidget(lab_merge_param);
    hlayer_minmax->addWidget(lab_min);
    hlayer_minmax->addWidget(lab_max);
    hlayer_drange->addWidget(lab_drange);
    hlayer_drange->addWidget(_drange_min);
    hlayer_drange->addWidget(_drange_max);
    hlayer_frame->addWidget(lab_img_range);
    hlayer_frame->addWidget(_frame_min);
    hlayer_frame->addWidget(_frame_max);
    hlayer_shell->addWidget(lab_nshells);
    hlayer_shell->addWidget(_shell_res);
    hlayer_fridel->addWidget(lab_friedel);
    hlayer_fridel->addWidget(_friedel);


    // set layout for 2 element rows
    QSizePolicy left_widget(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy right_widget(QSizePolicy::Preferred, QSizePolicy::Preferred);
    left_widget.setHorizontalStretch(1);
    right_widget.setHorizontalStretch(2);
    lab_nshells->setSizePolicy(left_widget);
    _shell_res->setSizePolicy(right_widget);
    lab_friedel->setSizePolicy(left_widget);
    _friedel->setSizePolicy(right_widget);

    gridLayout->addWidget(_merge_param_box);

    _comment_grp_box = new QGroupBox();
    //_comment_grp_box->setTitle("Comment");
    QVBoxLayout* vlay_comment = new QVBoxLayout();

    _textbox = new QTextEdit();
    QLabel* lab_cmt = new QLabel("Comment");
    vlay_comment->addWidget(lab_cmt);
    vlay_comment->addWidget(_textbox);
    _comment_grp_box->setLayout(vlay_comment);
    gridLayout->addWidget(_comment_grp_box);

    _button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    gridLayout->addWidget(_button_box);

    _rb_merged->setChecked(_merged_data);
    _rb_unmerged->setChecked(!_merged_data);

    loadMergeParams();

    connect(_button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(_button_box, &QDialogButtonBox::accepted, this, [=]() {
        _selected_data = _datacombo->currentData()->name();
        _selected_pc = _peakcombo->currentText().toStdString();
        _comment = _textbox->toPlainText().toStdString();
        _merged_data = _rb_merged->isChecked();
        accept();
    });

    connect(
        _drange_min, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        &MtzExportDialog::setMergeParams);
    connect(
        _drange_max, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        &MtzExportDialog::setMergeParams);
    connect(
        _frame_min, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        &MtzExportDialog::setMergeParams);
    connect(
        _frame_max, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        &MtzExportDialog::setMergeParams);
    connect(
        _shell_res, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        &MtzExportDialog::setMergeParams);
    connect(_friedel, &QCheckBox::stateChanged, this, &MtzExportDialog::setMergeParams);

    refresh();
}

MtzExportDialog::~MtzExportDialog() { }

void MtzExportDialog::loadMergeParams()
{
    auto params = gSession->currentProject()->experiment()->peakMerger()->parameters();

    _drange_min->setValue(params->d_min);
    _drange_max->setValue(params->d_max);
    _frame_min->setValue(params->frame_min);
    _frame_max->setValue(params->frame_max);
    _shell_res->setValue(params->n_shells);
    _friedel->setChecked(params->friedel);
}

void MtzExportDialog::setMergeParams()
{
    auto params = gSession->currentProject()->experiment()->peakMerger()->parameters();

    params->d_min = _drange_min->value();
    params->d_max = _drange_max->value();
    params->frame_min = _frame_min->value();
    params->frame_max = _frame_max->value();
    params->n_shells = _shell_res->value();
    params->friedel = _friedel->isChecked();
}

void MtzExportDialog::refresh()
{
    _datacombo->refresh();
    loadMergeParams();

    auto pcollections = gSession->currentProject()->experiment()->getPeakCollections();

    for (auto& e : pcollections) {
        if (e->type() == ohkl::PeakCollectionType::FOUND)
            continue;
        _peakcombo->addItem(QString::fromStdString(e->name()));
    }
}
}
