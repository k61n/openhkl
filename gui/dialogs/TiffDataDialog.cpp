//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/TiffDataDialog.cpp
//! @brief     Implements class TiffDataDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/TiffDataDialog.h"

#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "gui/dialogs/ConfirmOverwriteDialog.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/SafeSpinBox.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

TiffDataDialog::TiffDataDialog(
    const ohkl::TiffDataReaderParameters& parameters0, const QStringList& datanames_cur,
    QString image_resolution)
    : _parameters0{parameters0}, _dataset_names{datanames_cur}, _img_res(image_resolution)
{
    setModal(true);

    auto detector = gSession->currentProject()->experiment()->getDiffractometer()->detector();

    // extract file_res from string
    auto pos_x = image_resolution.toStdString().find_first_of("x");
    auto pos_end = image_resolution.toStdString().find_first_of(" ");
    _img_res_cols = std::stoi(image_resolution.toStdString().substr(0, pos_x));
    _img_res_rows = std::stoi(image_resolution.toStdString().substr(pos_x + 1, pos_end));


    QGridLayout* main_grid = new QGridLayout();
    GridFiller gridfiller(main_grid);
    setLayout(main_grid);

    QGridLayout* check_grid = new QGridLayout();
    QGroupBox* _check = new QGroupBox("Data from files do not fit selected detector");

    _check->setLayout(check_grid);
    gridfiller.addWidget(_check);
    _check->setVisible(false);

    _datasetName = gridfiller.addLineEdit("Name", QString::fromStdString(parameters0.dataset_name));
    image_resolution += QString(" Pixels");
    _image_resolution = gridfiller.addLineEdit(
        "Image resolution: ", image_resolution,
        "Shows the resolution which was found in the selected files");
    _image_resolution->setReadOnly(true);

    _detector_resolutions = gridfiller.addCombo(
        "Target resolution:",
        "Select the target resolution to which data from files will be mapped to. Necessary data "
        "rebinning is shown in bracket and will be automatically performed.");

    // fill the detector resolution combo box with the available resolution
    auto cols = detector->getColRes();
    auto rows = detector->getRowRes();

    QString det_res, data_bin;

    for (int i = 0; i < cols.size(); ++i) { // building combo box entries
        int ratio = _img_res_cols / int(cols[i]);
        det_res = QString::fromStdString(std::to_string(int(cols[i]))) + " x "
            + QString::fromStdString(std::to_string(int(rows[i]))) + " Pixels";

        if (ratio > 0)
            data_bin = "  ( Binning: " + QString::number(ratio) + " x " + QString::number(ratio)
                + " -> 1 )";
        else
            data_bin = "  [ NOT SUPPORTED ]";

        _detector_resolutions->addItem(det_res + data_bin);
    }

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

    _chi->setDecimals(3);
    _phi->setDecimals(3);
    _omega->setDecimals(3);
    _wavelength->setDecimals(3);
    _baseline->setDecimals(3);
    _gain->setDecimals(3);

    _baseline->setMaximum(10000);

    _chi->setValue(parameters0.delta_chi);
    _omega->setValue(parameters0.delta_omega);
    _phi->setValue(parameters0.delta_phi);
    _wavelength->setValue(parameters0.wavelength);
    _datasetName->setText(QString::fromStdString(parameters0.dataset_name));

    _baseline->setValue(detector->baseline());
    _gain->setValue(detector->gain());

    selectDetectorResolution();

    connect(_buttons, &QDialogButtonBox::accepted, this, &TiffDataDialog::verify);
    connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(
        _detector_resolutions, &QComboBox::currentTextChanged, this,
        &TiffDataDialog::selectDetectorResolution);
}

void TiffDataDialog::selectDetectorResolution()
{
    // make sure only supported target resolution are accepted
    auto detector = gSession->currentProject()->experiment()->getDiffractometer()->detector();
    auto idx = _detector_resolutions->currentIndex();

    detector->selectDetectorResolution(idx);

    std::vector<int> _det_res_cols = detector->getColRes();

    _buttons->buttons()[0]->setEnabled(_img_res_cols >= _det_res_cols[idx]);

    if (_img_res_cols < _det_res_cols[idx])
        QMessageBox::warning(
            this, tr("Importing Tiff Data"),
            "Target resolution cannot be greater than recorded resolution from data files. Make "
            "sure that your selected target resolution is supported by your selected data files.",
            QMessageBox::Ok);
    else {
        detector->selectDetectorResolution(idx);
    }
}

void TiffDataDialog::verify()
{
    // confirm overwrite if the name already exists
    const QString& dname = _datasetName->text();
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
    const double waveln = _wavelength->value();
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

ohkl::TiffDataReaderParameters TiffDataDialog::parameters()
{
    ohkl::TiffDataReaderParameters parameters;
    parameters.dataset_name = _datasetName->text().toStdString();
    parameters.wavelength = _wavelength->value();
    parameters.delta_omega = _omega->value();
    parameters.delta_chi = _chi->value();
    parameters.delta_phi = _phi->value();

    if (_set_baseline_and_gain->isChecked()) {
        parameters.baseline = _baseline->value();
        parameters.gain = _gain->value();
    } else {
        parameters.baseline = 0.0;
        parameters.gain = 1.0;
    }

    static int binning[3] = {1, 4, 16};

    return parameters;
}
