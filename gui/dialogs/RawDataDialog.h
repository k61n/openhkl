//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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
#include <QLineEdit>
#include <QString>

#include "core/loader/RawDataReader.h" // RawDataReaderParameters

#include <string>

//! Dialog to get the parameters to the selected raw data
class RawDataDialog : public QDialog {
 public:
    RawDataDialog(
        const nsx::RawDataReaderParameters& parameters0,
        const QStringList& datanames_cur = QStringList());

    QString dataset_name() { return datasetName->text(); }
    double wavelength() { return wave->value(); }
    double deltaChi() { return chi->value(); }
    double deltaOmega() { return omega->value(); }
    double deltaPhi() { return phi->value(); }
    bool swapEndian() { return swapEndianness->isChecked(); }
    bool rowMajor();
    int bpp();
    void setWavelength(double newWavelength) { wave->setValue(newWavelength); }
    //! Return a copy of the parameters acquired from the dialog
    nsx::RawDataReaderParameters parameters();

 private:
    //! Verify user-provided parameters
    void verify();

 private:
    QComboBox* dataArrangement;
    QComboBox* dataFormat;
    QCheckBox* swapEndianness;
    QDoubleSpinBox* chi;
    QDoubleSpinBox* omega;
    QDoubleSpinBox* phi;
    QDoubleSpinBox* wave;
    QDialogButtonBox* buttons;
    QLineEdit* datasetName;
    const nsx::RawDataReaderParameters& parameters0; // initial parameters
    const QStringList& dataset_names; // list of current dataset names
};

#endif // NSX_GUI_DIALOGS_RAWDATADIALOG_H
