//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/RawDataDialog.h
//! @brief     Defines class RawDataDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_DIALOGS_RAWDATADIALOG_H
#define NSX_GUI_DIALOGS_RAWDATADIALOG_H

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

    QString dataset_name() { return _datasetName->text(); }
    double wavelength() { return _wavelength->value(); }
    double deltaChi() { return _chi->value(); }
    double deltaOmega() { return _omega->value(); }
    double deltaPhi() { return _phi->value(); }
    bool swapEndian() { return _swapEndianness->isChecked(); }
    double baseline() { return _baseline->value(); }
    double gain() { return _gain->value(); }
    bool rowMajor();
    int bpp();
    void setWavelength(double newWavelength) { _wavelength->setValue(newWavelength); }
    //! Return a copy of the parameters acquired from the dialog
    ohkl::RawDataReaderParameters parameters();

 private:
    //! Verify user-provided parameters
    void verify();

 private:
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
    const ohkl::RawDataReaderParameters& _parameters0; // initial parameters
    const QStringList& _dataset_names; // list of current dataset names
};

#endif // NSX_GUI_DIALOGS_RAWDATADIALOG_H
