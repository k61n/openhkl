//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/ImageReaderDialog.h
//! @brief     Defines class ImageReaderDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_DATADIALOG_H
#define OHKL_GUI_DIALOGS_DATADIALOG_H

#include "core/loader/IDataReader.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QString>

#include <string>

class SafeDoubleSpinBox;
namespace ohkl {
class DataReaderParameters;
}

//! Dialog to either get *.raw or *.tif files
class ImageReaderDialog : public QDialog {
 public:
    //! Constructor in case or processing Raw files
    ImageReaderDialog(
        const QStringList& filenames, ohkl::DataReaderParameters* parameters0,
        ohkl::DataFormat data_format = ohkl::DataFormat::TIFF);
    //! Disable widgets that are relevant for multiple images only
    void setSingleImageMode();
    //! Return a copy of the DataReaderParameters object
    ohkl::DataReaderParameters dataReaderParameters();
    //! Allows to select Detector resolution
    void selectDetectorResolution();

 private:
    //! Verify user-provided parameters
    void verify();
    bool rowMajor();
    int bytesPerPixel();
    //! Check tiff files for resolution and bit depth
    bool checkTiffFiles(const QStringList& filenames);

    QComboBox* _dataArrangement;
    QComboBox* _dataFormat;
    QCheckBox* _swapEndianness;
    SafeDoubleSpinBox* _chi;
    SafeDoubleSpinBox* _omega;
    SafeDoubleSpinBox* _phi;
    SafeDoubleSpinBox* _2theta_gamma;
    SafeDoubleSpinBox* _2theta_nu;
    SafeDoubleSpinBox* _wavelength;
    QGroupBox* _set_baseline_and_gain;
    SafeDoubleSpinBox* _baseline;
    SafeDoubleSpinBox* _gain;
    QDialogButtonBox* _buttons;
    QLineEdit* _datasetName;

    ohkl::DataReaderParameters* _parameters0;

    int _bytes_per_pixel;
    std::pair<int, int> _img_res;

    QComboBox* _image_resolution;
    QComboBox* _rebin_size;

    ohkl::DataFormat _data_format;
};

#endif // OHKL_GUI_DIALOGS_RAWDATADIALOG_H
