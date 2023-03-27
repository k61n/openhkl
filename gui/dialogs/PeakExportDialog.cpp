//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/PeakExportDialog.cpp
//! @brief     Implements class PeakExportDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "PeakExportDialog.h"

#include "core/statistics/PeakExporter.h"
#include "core/statistics/PeakMerger.h"
#include "gui/MainWin.h" // gGui
#include "gui/utility/DataComboBox.h"
#include "gui/utility/IntegratedPeakComboBox.h"
#include "tables/crystal/SpaceGroup.h"

#include <QFileDialog>
#include <QSettings>

#include <string>

PeakExportDialog::PeakExportDialog() : QDialog(), _exporter()
{
    setModal(true);
    setWindowTitle("Export to .mtz file");

    _data_combo = new DataComboBox(this);
    _peak_combo_1 = new IntegratedPeakComboBox(this);
    _peak_combo_2 = new IntegratedPeakComboBox(this);
    _format_combo = new QComboBox;
    _peak_combo_2->setEmptyFirst();

    _data_combo->refresh();
    _peak_combo_1->refresh();
    _peak_combo_2->refresh();

    for (int idx = 0; idx < static_cast<int>(ohkl::ExportFormat::Count); ++idx) {
        std::string text =
            _exporter.exportFormatStrings()->at(static_cast<ohkl::ExportFormat>(idx));
        _format_combo->addItem(QString::fromStdString(text));
    }

    QLabel* label;
    QGridLayout* combo_grid = new QGridLayout;

    label = new QLabel("Data set:");
    combo_grid->addWidget(label, 0, 0, 1, 1);
    combo_grid->addWidget(_data_combo, 0, 1, 1, 1);
    label = new QLabel("Peak collection 1:");
    combo_grid->addWidget(label, 1, 0, 1, 1);
    combo_grid->addWidget(_peak_combo_1, 1, 1, 1, 1);
    label = new QLabel("Peak collection 2:");
    combo_grid->addWidget(label, 2, 0, 1, 1);
    combo_grid->addWidget(_peak_combo_2, 2, 1, 1, 1);
    label = new QLabel("Export format:");
    combo_grid->addWidget(label, 3, 0, 1, 1);
    combo_grid->addWidget(_format_combo, 3, 1, 1, 1);

    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QGroupBox* merge_radio_group = new QGroupBox;
    _rb_merged = new QRadioButton("Merged peaks", merge_radio_group);
    _rb_unmerged = new QRadioButton("Unmerged peaks", merge_radio_group);

    QHBoxLayout* merge_radio_layout = new QHBoxLayout(merge_radio_group);
    merge_radio_layout->addWidget(_rb_merged);
    merge_radio_layout->addWidget(_rb_unmerged);

    QGroupBox* sum_radio_group = new QGroupBox;
    _rb_sum = new QRadioButton("Sum intensities", sum_radio_group);
    _rb_profile = new QRadioButton("Profile intensities", sum_radio_group);

    QHBoxLayout* sum_radio_layout = new QHBoxLayout(sum_radio_group);
    sum_radio_layout->addWidget(_rb_sum);
    sum_radio_layout->addWidget(_rb_profile);

    main_layout->addLayout(combo_grid);
    main_layout->addWidget(merge_radio_group);
    main_layout->addWidget(sum_radio_group);

    QGroupBox* merge_param_group = new QGroupBox();
    _drange_min = new QDoubleSpinBox();
    _drange_max = new QDoubleSpinBox();
    _frame_min = new QSpinBox();
    _frame_max = new QSpinBox();
    _scale_factor = new QDoubleSpinBox();
    _friedel = new QCheckBox("Friedel");

    QGridLayout* params_grid = new QGridLayout;
    int row = 0;
    label = new QLabel("Resolution (d) range:");
    params_grid->addWidget(label, row, 0, 1, 1);
    params_grid->addWidget(_drange_min, row, 1, 1, 1);
    params_grid->addWidget(_drange_max, row++, 2, 1, 1);
    label = new QLabel("Image range:");
    params_grid->addWidget(label, row, 0, 1, 1);
    params_grid->addWidget(_frame_min, row, 1, 1, 1);
    params_grid->addWidget(_frame_max, row++, 2, 1, 1);
    label = new QLabel("Scale factor:");
    params_grid->addWidget(label, row, 0, 1, 1);
    params_grid->addWidget(_scale_factor, row++, 1, 1, 1);
    params_grid->addWidget(_friedel, row++, 1, 1, 1);
    merge_param_group->setLayout(params_grid);

    // set layout for 2 element rows
    QSizePolicy left_widget(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy right_widget(QSizePolicy::Preferred, QSizePolicy::Preferred);
    left_widget.setHorizontalStretch(1);
    right_widget.setHorizontalStretch(2);

    main_layout->addWidget(merge_param_group);

    QGroupBox* comment_group = new QGroupBox();
    QVBoxLayout* vlay_comment = new QVBoxLayout();

    _textbox = new QTextEdit();
    QLabel* lab_cmt = new QLabel("Comment");
    vlay_comment->addWidget(lab_cmt);
    vlay_comment->addWidget(_textbox);
    comment_group->setLayout(vlay_comment);
    main_layout->addWidget(comment_group);

    _button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    main_layout->addWidget(_button_box);

    _rb_merged->setChecked(true);
    _rb_sum->setChecked(true);

    loadMergeParams();

    connect(_button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(_button_box, &QDialogButtonBox::accepted, this, &PeakExportDialog::processMerge);
    connect(
        _data_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &PeakExportDialog::onDataChanged);
    connect(_rb_sum, &QRadioButton::toggled, this, &PeakExportDialog::setSumIntensities);

    refresh();
    onDataChanged();
}

void PeakExportDialog::onDataChanged()
{
    auto data = _data_combo->currentData();
    _frame_max->setMaximum(data->nFrames());
    _frame_max->setValue(data->nFrames());
    _data_combo->refresh();
}

void PeakExportDialog::loadMergeParams()
{
    auto params = gSession->currentProject()->experiment()->peakMerger()->parameters();

    _drange_min->setValue(params->d_min);
    _drange_max->setValue(params->d_max);
    _frame_min->setValue(0);
    _frame_max->setValue(_data_combo->currentData()->nFrames());
    _friedel->setChecked(params->friedel);
    _scale_factor->setValue(params->scale);
}

void PeakExportDialog::setMergeParams()
{
    auto params = gSession->currentProject()->experiment()->peakMerger()->parameters();

    params->d_min = _drange_min->value();
    params->d_max = _drange_max->value();
    params->frame_min = _frame_min->value();
    params->frame_max = _frame_max->value();
    params->friedel = _friedel->isChecked();
    params->scale = _scale_factor->value();
}

void PeakExportDialog::refresh()
{
    _data_combo->refresh();
    _peak_combo_1->refresh();
    _peak_combo_2->refresh();
    loadMergeParams();
}

void PeakExportDialog::processMerge()
{
    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();
    merger->reset();
    setMergeParams();

    std::vector<ohkl::PeakCollection*> peak_collections;
    peak_collections.push_back(_peak_combo_1->currentPeakCollection());
    if (_peak_combo_2->currentPeakCollection())
        peak_collections.push_back(_peak_combo_2->currentPeakCollection());

    auto cell = singleBatchRefine();
    merger->setSpaceGroup(cell->spaceGroup());

    merger->addPeakCollection(_peak_combo_1->currentPeakCollection());
    if (_peak_combo_2->currentPeakCollection())
        merger->addPeakCollection(_peak_combo_2->currentPeakCollection());
    ohkl::ExportFormat fmt = static_cast<ohkl::ExportFormat>(_format_combo->currentIndex());

    merger->mergePeaks();

    // Get the file name and save the file
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("experiment", QDir::homePath()).toString();
    std::string filter = _exporter.exportFormatStrings()->at(
        static_cast<ohkl::ExportFormat>(_format_combo->currentIndex()));
    QString filename = QFileDialog::getSaveFileName(
        this, "Export Experiment as MTZ file", loadDirectory, QString::fromStdString(filter));
    std::string comment = _textbox->toPlainText().toStdString();
    if (filename.isEmpty())
        return;

    _exporter.exportPeaks(
        fmt, filename.toStdString(), merger->getMergedData(), _data_combo->currentData(), cell,
        _rb_merged->isChecked(), _scale_factor->value(), comment);
    accept();
}

ohkl::sptrUnitCell PeakExportDialog::singleBatchRefine()
{
    auto expt = gSession->currentProject()->experiment();
    auto* peaks = _peak_combo_1->currentPeakCollection();
    const auto data = _data_combo->currentData();
    auto states = data->instrumentStates();
    auto* refiner = expt->refiner();
    auto* params = refiner->parameters();

    params->nbatches = 1;
    params->max_iter = 1000;
    params->refine_ub = true;
    params->refine_sample_position = false;
    params->refine_sample_orientation = false;
    params->refine_detector_offset = false;
    params->refine_ki = false;
    params->use_batch_cells = true;
    params->set_unit_cell = false;

    try {
        expt->refine(peaks, data.get());
        gSession->onUnitCellChanged();
    } catch (const std::exception& ex) {
        gGui->statusBar()->showMessage("Error: " + QString(ex.what()));
    }

    return refiner->batches()[0].sptrCell();
}

void PeakExportDialog::setSumIntensities(bool flag)
{
    _exporter.setSumIntensities(flag);
}
