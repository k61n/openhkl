//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/DataDialog.h
//! @brief     Defines class DataDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************
#include "core/loader/TiffDataReader.h"
#include "gui/utility/SafeSpinBox.h"

#ifndef OHKL_GUI_DIALOGS_DATADIALOG_H
#define OHKL_GUI_DIALOGS_DATADIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QString>

#include "core/loader/RawDataReader.h" // RawDataReaderParameters
#include "gui/utility/SafeSpinBox.h"

#include <string>

//! Dialog to either get *.raw or *.tif files
// Name change from RawDataDialog to DataDialog
class DataDialog : public QDialog {
 public:
    //! Constructor in case or processing Raw files
    DataDialog(
        ohkl::DataReaderParameters* parameters0, const QStringList& datanames_cur = QStringList(),
        bool tif_data = false, QString img_res = "");
    //! Disable widgets that are relevant for multiple images only
    void setSingleImageMode();
    //! Return a copy of raw parameters
    ohkl::RawDataReaderParameters rawParameters();
    //! Return a copy of tif parameters
    ohkl::TiffDataReaderParameters tiffParameters();
    //! Checks Settings
    void checkSettings(uint16_t bpp, uint32_t cols_in_file, uint32_t rows_in_file);
    //! Allows to select Detector resolution
    void selectDetectorResolution();

 private:
    //! Verify user-provided parameters
    void verify();
    bool rowMajor();
    int bpp();

    const QStringList& _dataset_names; // list of current dataset names

    QComboBox* _dataArrangement;
    QComboBox* _dataFormat;
    QCheckBox* _swapEndianness;
    SafeDoubleSpinBox* _chi;
    SafeDoubleSpinBox* _omega;
    SafeDoubleSpinBox* _phi;
    SafeDoubleSpinBox* _wavelength;
    QGroupBox* _set_baseline_and_gain;
    SafeDoubleSpinBox* _baseline;
    SafeDoubleSpinBox* _gain;
    QDialogButtonBox* _buttons;
    QLineEdit* _datasetName;

    ohkl::DataReaderParameters* _parameters0;

    bool _allow_rebinning_data;
    int _img_res_cols;
    int _img_res_rows;

    QSpinBox* _databits_in_img;
    QSpinBox* _cols_in_img;
    QSpinBox* _rows_in_img;
    QLineEdit* _img_res_image_resolution;
    QComboBox* _detector_resolutions;

    // Do we process Tiff (true) or Raw Files(false)
    bool _processing_tif_files;
    QString _img_res;
};

#endif // OHKL_GUI_DIALOGS_RAWDATADIALOG_H
