//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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
#include "gui/dialogs/ConfirmOverwriteDialog.h"

#include <QFormLayout>
#include <QMessageBox>

RawDataDialog::RawDataDialog(const nsx::RawDataReaderParameters& parameters0,
                             const QStringList& datanames_cur):
    parameters0 {parameters0},
    dataset_names{datanames_cur}
{
    QFormLayout* layout = new QFormLayout(this);

    dataArrangement = new QComboBox();
    dataFormat = new QComboBox();
    swapEndianness = new QCheckBox("Swap endian");
    chi = new QDoubleSpinBox();
    omega = new QDoubleSpinBox();
    phi = new QDoubleSpinBox();
    wave = new QDoubleSpinBox();
    datasetName = new QLineEdit();

    buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    dataArrangement->addItems(QStringList{"Column major", "Row major"});
    dataFormat->addItems(QStringList{"8 bit integer", "16 bit integer", "32 bit integer"});
    chi->setDecimals(3);
    phi->setDecimals(3);
    omega->setDecimals(3);
    wave->setDecimals(3);

    // default to Row major/16 bit
    dataArrangement->setCurrentIndex(1);
    dataFormat->setCurrentIndex(1);
    swapEndianness->setCheckState(Qt::Checked);

    chi->setValue(parameters0.delta_chi);
    omega->setValue(parameters0.delta_omega);
    phi->setValue(parameters0.delta_phi);
    wave->setValue(parameters0.wavelength);
    datasetName->setText(QString::fromStdString(parameters0.dataset_name));

    dataArrangement->setToolTip("Toggle data arrangement between row and column major");
    chi->setToolTip("Angle increment about the chi instrument axis");
    omega->setToolTip("Angle increment about the omega instrument axis");
    phi->setToolTip("Angle increment about the phi instrument axis");
    swapEndianness->setToolTip("Swap the endianness of the input data");
    wave->setToolTip("Wavelength of the incident beam");
    dataFormat->setToolTip("Number of bytes per pixel in images");

    layout->addRow("Name", datasetName);
    layout->addRow("Data arrangement", dataArrangement);
    layout->addRow("Data format", dataFormat);
    layout->addRow("", swapEndianness);
    layout->addRow(QString((QChar)0x0394) + " " +  QString((QChar)0x03C7), chi);
    layout->addRow(QString((QChar)0x0394) + " " + QString((QChar)0x03C9), omega);
    layout->addRow(QString((QChar)0x0394) + " " + QString((QChar)0x03C6), phi);
    layout->addRow("Wavelength", wave);
    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &RawDataDialog::verify);

    connect(buttons, &QDialogButtonBox::rejected, this, &RawDataDialog::reject);
}


bool RawDataDialog::rowMajor()
{
    QString selection = dataArrangement->currentText();
    if (selection == "Row major")
        return true;
    if (selection == "Column major")
        return false;
    return false;
}

int RawDataDialog::bpp()
{
    switch (dataFormat->currentIndex()) {
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
    const bool name_exists = dataset_names.contains(dname);
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
        const QString msg{QString::fromStdString(
                "Wavelength, " + std::to_string(waveln) + ", must be > 0")};
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
    parameters.dataset_name = datasetName->text().toStdString();
    parameters.wavelength = wave->value();
    parameters.delta_omega = omega->value();
    parameters.delta_chi = chi->value();
    parameters.delta_phi = phi->value();
    parameters.row_major = rowMajor();
    parameters.swap_endian = swapEndianness->isChecked();
    parameters.bpp = bpp();
    return parameters;
}
