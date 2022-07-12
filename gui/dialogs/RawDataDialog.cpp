//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/RawDataDialog.cpp
//! @brief     Implements class RawDataDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/RawDataDialog.h"

#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "gui/dialogs/ConfirmOverwriteDialog.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/SafeSpinBox.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QLabel>

RawDataDialog::RawDataDialog(
    const nsx::RawDataReaderParameters& parameters0, const QStringList& datanames_cur)
    : _parameters0{parameters0}, _dataset_names{datanames_cur}
{
    setModal(true);
    resize(400, 300);

    QGridLayout* main_grid = new QGridLayout();
    GridFiller gridfiller(main_grid);
    setLayout(main_grid);

    _datasetName = gridfiller.addLineEdit(
        "Name", QString::fromStdString(parameters0.dataset_name));
    _dataArrangement = gridfiller.addCombo(
        "Data arrangement", "Toggle data arrangement between row and column major");
    _dataFormat = gridfiller.addCombo("Data format", "Number of bytes per pixel in images");
    _swapEndianness = gridfiller.addCheckBox(
        "Swap endian", "Swap the endianness of the input data", 1);
    _chi = gridfiller.addDoubleSpinBox(
        QString((QChar)0x0394) + " " + QString((QChar)0x03C7),
        "Angle increment about the chi instrument axis");
    _omega = gridfiller.addDoubleSpinBox(
        QString((QChar)0x0394) + " " + QString((QChar)0x03C9),
        "Angle increment about the omega instrument axis");
    _phi = gridfiller.addDoubleSpinBox(
        QString((QChar)0x0394) + " " + QString((QChar)0x03C6),
        "Angle incremet about the phi instrument axis");
    _wavelength = gridfiller.addDoubleSpinBox("Wavelength", "Wavelength of the incident beam");

    _set_baseline_and_gain = new QGroupBox("Use baseline/gain");
    _set_baseline_and_gain->setCheckable(true);
    _set_baseline_and_gain->setChecked(false);
    _set_baseline_and_gain->setToolTip("Use baseline and gain from yml2c file");

    QGridLayout* small_grid = new QGridLayout();
    _baseline = new SafeDoubleSpinBox();
    _gain = new SafeDoubleSpinBox();
    QLabel* label1 = new QLabel("Baseline");
    label1->setToolTip("To be subtracted from each pixel");
    label1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLabel* label2 = new QLabel("Gain");
    label2->setToolTip("Each pixel count to be divided by this value");
    label2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    small_grid->addWidget(label1, 0, 0, 1, 1);
    small_grid->addWidget(_baseline, 0, 1, 1, 1);
    small_grid->addWidget(label2, 1, 0, 1, 1);
    small_grid->addWidget(_gain, 1, 1, 1, 1);
    _set_baseline_and_gain->setLayout(small_grid);

    _buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    gridfiller.addWidget(_set_baseline_and_gain);
    gridfiller.addWidget(_buttons);


    _dataArrangement->addItems(QStringList{"Column major", "Row major"});
    _dataFormat->addItems(QStringList{"8 bit integer", "16 bit integer", "32 bit integer"});

    _chi->setDecimals(3);
    _phi->setDecimals(3);
    _omega->setDecimals(3);
    _wavelength->setDecimals(3);
    _baseline->setDecimals(3);
    _gain->setDecimals(3);

    _baseline->setMaximum(10000);

    // default to Row major/16 bit
    _dataArrangement->setCurrentIndex(1);
    _dataFormat->setCurrentIndex(1);
    _swapEndianness->setCheckState(Qt::Checked);

    _chi->setValue(parameters0.delta_chi);
    _omega->setValue(parameters0.delta_omega);
    _phi->setValue(parameters0.delta_phi);
    _wavelength->setValue(parameters0.wavelength);
    _datasetName->setText(QString::fromStdString(parameters0.dataset_name));
    nsx::Detector* detector =
        gSession->currentProject()->experiment()->getDiffractometer()->detector();
    _baseline->setValue(detector->baseline());
    _gain->setValue(detector->gain());

    connect(_buttons, &QDialogButtonBox::accepted, this, &RawDataDialog::verify);
    connect(_buttons, &QDialogButtonBox::rejected, this, &RawDataDialog::reject);
}


bool RawDataDialog::rowMajor()
{
    QString selection = _dataArrangement->currentText();
    if (selection == "Row major")
        return true;
    if (selection == "Column major")
        return false;
    return false;
}

int RawDataDialog::bpp()
{
    switch (_dataFormat->currentIndex()) {
        case 0: // 8 bit
            return 1;
        case 1: // 16 bit
            return 2;
        case 2: // 32 bit
            return 4;
        default: return -1;
    }
}

void RawDataDialog::verify()
{
    // confirm overwrite if the name already exists
    const QString& dname = dataset_name();
    const bool name_exists = _dataset_names.contains(dname);
    bool dialog_accepted = true;

    if (name_exists) {
        const QString msg("Name '" + dname + "' already exists");
        QMessageBox::critical(nullptr, "Error", msg);
        dialog_accepted = false;
        return;
    }

    // check wavelength
    const double eps = 1e-8;
    const double waveln = wavelength();
    if (waveln < eps) {
        const QString msg{
            QString::fromStdString("Wavelength, " + std::to_string(waveln) + ", must be > 0")};
        QMessageBox::critical(nullptr, "Error", msg);
        dialog_accepted = false;
        return;
    }

    if (dialog_accepted)
        this->accept();
}

nsx::RawDataReaderParameters RawDataDialog::parameters()
{
    nsx::RawDataReaderParameters parameters;
    parameters.dataset_name = _datasetName->text().toStdString();
    parameters.wavelength = _wavelength->value();
    parameters.delta_omega = _omega->value();
    parameters.delta_chi = _chi->value();
    parameters.delta_phi = _phi->value();
    parameters.row_major = rowMajor();
    parameters.swap_endian = _swapEndianness->isChecked();
    parameters.bpp = bpp();
    if (_set_baseline_and_gain->isChecked()) {
        parameters.baseline = _baseline->value();
        parameters.gain = _gain->value();
    } else {
        parameters.baseline = 0.0;
        parameters.gain = 1.0;
    }
    return parameters;
}
