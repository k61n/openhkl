//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/TiffDataDialog.h
//! @brief     Defines class TiffDataDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_TIFFDATADIALOG_H
#define OHKL_GUI_DIALOGS_TIFFDATADIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QTextEdit>

#include "core/loader/TiffDataReader.h"
#include "gui/utility/SafeSpinBox.h"

#include <string>

//! Dialog to get the parameters to the selected raw data
class TiffDataDialog : public QDialog {
 public:
    TiffDataDialog(
        const ohkl::TiffDataReaderParameters& parameters0,
        const QStringList& datanames_cur = QStringList(),
        QString img_res = ""
        );

    //! Return a copy of the parameters acquired from the dialog
    ohkl::TiffDataReaderParameters parameters();
    void checkSettings(uint16_t bpp, uint32_t cols_in_file, uint32_t rows_in_file);
    void selectDetectorResolution();

 private:
    //! Verify user-provided parameters
    void verify();

    bool _allow_rebinning_data;

    int _file_res_cols;
    int _file_res_rows;
    QString _img_res;

    QSpinBox* _databits_in_file;
    QSpinBox* _cols_in_file;
    QSpinBox* _rows_in_file;

    SafeDoubleSpinBox* _chi;
    SafeDoubleSpinBox* _omega;
    SafeDoubleSpinBox* _phi;
    SafeDoubleSpinBox* _wavelength;
    QGroupBox* _set_baseline_and_gain;
    SafeDoubleSpinBox* _baseline;
    SafeDoubleSpinBox* _gain;
    QDialogButtonBox* _buttons;
    QLineEdit* _datasetName;
    QLineEdit* _image_resolution;
    QComboBox* _detector_resolutions;
    const ohkl::TiffDataReaderParameters& _parameters0; // initial parameters
    const QStringList& _dataset_names; // list of current dataset names
};

#endif // OHKL_GUI_DIALOGS_TIFFDATADIALOG_H
