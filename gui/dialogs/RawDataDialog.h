//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/RawDataDialog.h
//! @brief     Defines class RawDataDialog
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOGS_RAWDATADIALOG_H
#define OHKL_GUI_DIALOGS_RAWDATADIALOG_H

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

//! Dialog to get the parameters to the selected raw data
class RawDataDialog : public QDialog {
 public:
    RawDataDialog(
        const ohkl::RawDataReaderParameters& parameters0,
        const QStringList& datanames_cur = QStringList());

    //! Return a copy of the parameters acquired from the dialog
    ohkl::RawDataReaderParameters parameters();

 private:
    //! Verify user-provided parameters
    void verify();
    bool rowMajor();
    int bpp();

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
    const QStringList& _dataset_names; // list of current dataset names
};

#endif // OHKL_GUI_DIALOGS_RAWDATADIALOG_H
