//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ImageReaderDialog.cpp
//! @brief     Implements class ImageReaderDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/ImageReaderDialog.h"

#include "core/data/DataTypes.h"
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/TiffDataReader.h"
#include "core/raw/DataKeys.h"
#include "gui/dialogs/ConfirmOverwriteDialog.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/SafeSpinBox.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <stdexcept>

ImageReaderDialog::ImageReaderDialog(
    const QStringList& filenames, ohkl::DataReaderParameters* parameters0,
    ohkl::DataFormat data_format)
    : _parameters0{parameters0}, _data_format(data_format)
{
    setModal(true);

    if (_data_format == ohkl::DataFormat::TIFF) {
        if (!checkTiffFiles(filenames)) {
            QMessageBox::critical(
                nullptr, "Error", "Mismatch between .tiff files (resolution or depth)");
            this->reject();
        }
    }

    QGridLayout* main_grid = new QGridLayout();
    GridFiller gridfiller(main_grid);
    setLayout(main_grid);

    _datasetName =
        gridfiller.addLineEdit("Name", QString::fromStdString(parameters0->dataset_name));

    _dataArrangement = gridfiller.addCombo(
        "Data arrangement", "Toggle data arrangement between row and column major");
    _dataFormat = gridfiller.addCombo("Data format", "Image bit depth and numerical format");
    _swapEndianness =
        gridfiller.addCheckBox("Swap endian", "Swap the endianness of the input data", 1);
    _image_resolution =
        gridfiller.addCombo("Image resolution: ", "Resolution of image (columns x rows)");
    _rebin_size = gridfiller.addCombo(
        "Rebinning:", "Reduce resolution by averaging over a square pixel grid");

    // Poppulate the resolution combo boxes
    auto* diffractometer = gSession->currentProject()->experiment()->getDiffractometer();
    auto detector = diffractometer->detector();
    std::vector<std::pair<int, int>> resolutions = detector->getResolutions();

    QString det_res, data_bin;

    for (int i = 0; i < resolutions.size(); ++i) {
        det_res = QString::fromStdString(std::to_string(resolutions[i].first)) + " x "
            + QString::fromStdString(std::to_string(resolutions[i].second));
        _image_resolution->addItem(det_res);
    }

    _rebin_size->addItem(QString("1 x 1 ") + (QChar)0x2192 + QString(" 1"));
    _rebin_size->addItem(QString("2 x 2 ") + (QChar)0x2192 + QString(" 1"));
    _rebin_size->addItem(QString("4 x 4 ") + (QChar)0x2192 + QString(" 1"));

    QLabel* label;

    QGroupBox* sample_gonio_group = new QGroupBox("Sample goniometer");
    QGridLayout* sample_gonio_grid = new QGridLayout;

    label = new QLabel(QString((QChar)0x0394) + " " + QString((QChar)0x03C7));
    label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _chi = new SafeDoubleSpinBox;
    _chi->setToolTip("Angle increment about the chi sample axis (degrees)");
    sample_gonio_grid->addWidget(label, 0, 0, 1, 1);
    sample_gonio_grid->addWidget(_chi, 0, 1, 1, 1);
    label = new QLabel(QString((QChar)0x0394) + " " + QString((QChar)0x03C9));
    label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _omega = new SafeDoubleSpinBox;
    _omega->setToolTip("Angle increment about the omega sample axis (degrees)");
    sample_gonio_grid->addWidget(label, 1, 0, 1, 1);
    sample_gonio_grid->addWidget(_omega, 1, 1, 1, 1);
    label = new QLabel(QString((QChar)0x0394) + " " + QString((QChar)0x03C6));
    label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _phi = new SafeDoubleSpinBox;
    _phi->setToolTip("Angle increment about the phi sample axis (degrees)");
    sample_gonio_grid->addWidget(label, 2, 0, 1, 1);
    sample_gonio_grid->addWidget(_phi, 2, 1, 1, 1);

    sample_gonio_group->setLayout(sample_gonio_grid);
    gridfiller.addWidget(sample_gonio_group);

    QGroupBox* detector_gonio_group = new QGroupBox("Detector goniometer");
    QGridLayout* detector_gonio_grid = new QGridLayout;
    label = new QLabel("2" + QString((QChar)0x03B8) + " (" + QString((QChar)0x03B3) + ")");
    label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _2theta_gamma = new SafeDoubleSpinBox;
    _2theta_gamma->setToolTip("Detector goniometer gamma angle (degrees)");
    detector_gonio_grid->addWidget(label, 0, 0, 1, 1);
    detector_gonio_grid->addWidget(_2theta_gamma, 0, 1, 1, 1);
    label = new QLabel("2" + QString((QChar)0x03B8) + " (" + QString((QChar)0x03BD) + ")");
    label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _2theta_nu = new SafeDoubleSpinBox;
    _2theta_nu->setToolTip("Detector goniometer nu angle (degrees)");
    detector_gonio_grid->addWidget(label, 1, 0, 1, 1);
    detector_gonio_grid->addWidget(_2theta_nu, 1, 1, 1, 1);

    detector_gonio_group->setLayout(detector_gonio_grid);
    gridfiller.addWidget(detector_gonio_group);


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
    _dataFormat->addItems(QStringList{"16 bit integer", "32 bit integer"});

    _chi->setDecimals(3);
    _phi->setDecimals(3);
    _omega->setDecimals(3);
    _wavelength->setDecimals(3);
    _baseline->setDecimals(3);
    _gain->setDecimals(3);

    _baseline->setMaximum(10000);

    _chi->setValue(_parameters0->delta_chi);
    _omega->setValue(_parameters0->delta_omega);
    _phi->setValue(_parameters0->delta_phi);
    _wavelength->setValue(_parameters0->wavelength);
    _baseline->setValue(detector->baseline());
    _gain->setValue(detector->gain());

    _dataArrangement->setEnabled(false);
    _dataFormat->setEnabled(false);
    _image_resolution->setEnabled(false);
    switch (_data_format) {
        case (ohkl::DataFormat::TIFF): {
            switch (_bytes_per_pixel) {
                case 2: _dataFormat->setCurrentIndex(0); break;
                case 4: _dataFormat->setCurrentIndex(1); break;
                default:
                    throw std::runtime_error(
                        "ImageReaderDialog::ImageReaderDialog: invalid tiff bytes_per_pixel");
            }
            bool valid_resolution = false;
            for (int idx = 0; idx < resolutions.size(); ++idx) {
                if (_img_res.first == resolutions[idx].first
                    && _img_res.second == resolutions[idx].second) {
                    _image_resolution->setCurrentIndex(idx);
                    valid_resolution = true;
                    break;
                }
            }
            if (!valid_resolution)
                throw std::runtime_error("ImageReaderDialog::ImageReaderDialog: tiff files are not "
                                         "an expected resolution");
            break;
        }
        case (ohkl::DataFormat::RAW): {
            _dataArrangement->setEnabled(true);
            _dataFormat->setEnabled(true);
            _image_resolution->setEnabled(true);
            _rebin_size->setEnabled(false);
            _dataArrangement->setCurrentIndex(1);
            _dataFormat->setCurrentIndex(0);
            _swapEndianness->setCheckState(Qt::Checked);
            break;
        }
        case (ohkl::DataFormat::PLAINTEXT): {
            _dataArrangement->setEnabled(false);
            _dataFormat->setEnabled(true);
            _image_resolution->setEnabled(true);
            _rebin_size->setEnabled(false);
            _dataArrangement->setCurrentIndex(1);
            _dataFormat->setCurrentIndex(0);
            _swapEndianness->setCheckState(Qt::Checked);
            _swapEndianness->setEnabled(false);
            break;
        }
        default:
            throw std::runtime_error(
                "ImageReaderDialog::ImageReaderDialog: Unexpected DataFormat Encountered");
    }

    auto sample_axes = diffractometer->sample().gonio().axisNames();
    auto detector_axes = diffractometer->detector()->gonio().axisNames();

    if (std::find(sample_axes.begin(), sample_axes.end(), ohkl::ax_chi) == sample_axes.end())
        _chi->setDisabled(true);
    if (std::find(sample_axes.begin(), sample_axes.end(), ohkl::ax_omega) == sample_axes.end())
        _omega->setDisabled(true);
    if (std::find(sample_axes.begin(), sample_axes.end(), ohkl::ax_chi) == sample_axes.end())
        _phi->setDisabled(true);
    if (std::find(detector_axes.begin(), detector_axes.end(), ohkl::ax_2thetaGamma) ==
        detector_axes.end())
        _2theta_gamma->setDisabled(true);
    if (std::find(detector_axes.begin(), detector_axes.end(), ohkl::ax_2thetaNu)
        == detector_axes.end())
        _2theta_nu->setDisabled(true);

    connect(_buttons, &QDialogButtonBox::accepted, this, &ImageReaderDialog::verify);
    connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool ImageReaderDialog::rowMajor()
{
    QString selection = _dataArrangement->currentText();
    if (selection == "Row major")
        return true;
    if (selection == "Column major")
        return false;
    return false;
}

int ImageReaderDialog::bytesPerPixel()
{
    if (_data_format == ohkl::DataFormat::TIFF)
        // this values is saved in the tif files and has already been loaded
        return ((ohkl::DataReaderParameters*)_parameters0)->bytes_per_pixel;

    // For raw files
    switch (_dataFormat->currentIndex()) {
        case 0: // 16 bit
            return 2;
        case 1: // 32 bit
            return 4;
        default: return -1;
    }
}

void ImageReaderDialog::verify()
{
    // check wavelength
    const double eps = 1e-8;

    if (_wavelength->value() < eps) {
        QMessageBox::critical(nullptr, "Error", "Positive, non-zero wavelength is required");
        return;
    }

    if (_omega->value() < eps && _chi->value() < eps && _phi->value() < eps) {
        QMessageBox::critical(nullptr, "Error",
                              "Positive, non-zero value required for a sample angle increment");
        return;
    }

    // check name was given for DataSet
    if (_datasetName->text().isEmpty()) {
        QMessageBox::critical(nullptr, "Error", "A data set name is required");
        return;
    }

    this->accept();
}

void ImageReaderDialog::setSingleImageMode()
{
    _chi->setEnabled(false);
    _phi->setEnabled(false);
    _omega->setEnabled(false);
}

ohkl::DataReaderParameters ImageReaderDialog::dataReaderParameters()
{
    ohkl::DataReaderParameters parameters;
    parameters.dataset_name = _datasetName->text().toStdString();
    parameters.wavelength = _wavelength->value();
    parameters.delta_omega = _omega->value();
    parameters.delta_chi = _chi->value();
    parameters.delta_phi = _phi->value();
    parameters.swap_endian = _swapEndianness->isChecked();
    parameters.bytes_per_pixel = bytesPerPixel();

    if (_set_baseline_and_gain->isChecked()) {
        parameters.baseline = _baseline->value();
        parameters.gain = _gain->value();
    } else {
        parameters.baseline = 0.0;
        parameters.gain = 1.0;
    }

    switch (_data_format) {
        case (ohkl::DataFormat::TIFF): {
            parameters.data_format = ohkl::DataFormat::TIFF;
            parameters.cols = _img_res.first;
            parameters.rows = _img_res.second;
            switch (_rebin_size->currentIndex()) {
                case 0: parameters.rebin_size = 1; break;
                case 1: parameters.rebin_size = 2; break;
                case 2: parameters.rebin_size = 4; break;
            }

            auto detector =
                gSession->currentProject()->experiment()->getDiffractometer()->detector();
            auto idx = _image_resolution->currentIndex();
            detector->selectDetectorResolution(idx);

            if (parameters.rebin_size != 1) {
                detector->setNCols(detector->nCols() / parameters.rebin_size);
                detector->setNRows(detector->nRows() / parameters.rebin_size);
            }
            break;
        }
        case (ohkl::DataFormat::RAW): {
            parameters.data_format = ohkl::DataFormat::RAW;
            parameters.row_major = rowMajor();
            break;
        }
        case (ohkl::DataFormat::PLAINTEXT): {
            parameters.data_format = ohkl::DataFormat::PLAINTEXT;
            break;
        }
        default:
            throw std::runtime_error(
                "ImageReaderDialog::dataReaderParameters: Unexpected DataFormat Encountered");
    }

    return parameters;
}

void ImageReaderDialog::selectDetectorResolution()
{
    // make sure only supported target resolution are accepted
}

bool ImageReaderDialog::checkTiffFiles(const QStringList& filenames)
{
    std::vector<std::string> files;
    for (const auto& name : filenames)
        files.push_back(name.toStdString());

    ohkl::TiffDataReader reader;
    auto bpps = reader.readFileBitDepths(files);
    for (const auto& bpp : bpps) {
        if (bpp != bpps[0])
            return false;
    }

    auto resolutions = reader.readFileResolutions(files);
    for (const auto& res : resolutions)
        if (resolutions[0].first != res.first && resolutions[0].second != res.second)
            return false;

    _bytes_per_pixel = bpps[0] / 8;
    _img_res = resolutions[0];
    return true;
}
