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

#ifndef GUI_DIALOGS_RAWDATADIALOG_H
#define GUI_DIALOGS_RAWDATADIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>

//! Dialog to get the parameters to the selected raw data
class RawDataDialog : public QDialog {
 public:
    RawDataDialog();
    double wavelength() { return wave->value(); }
    double deltaChi() { return chi->value(); }
    double deltaOmega() { return omega->value(); }
    double deltaPhi() { return phi->value(); }
    bool swapEndian() { return swapEndianness->isChecked(); }
    bool rowMajor();
    int bpp();
    void setWavelength(double newWavelength) { wave->setValue(newWavelength); }

 private:
    QComboBox* dataArrangement;
    QComboBox* dataFormat;
    QCheckBox* swapEndianness;
    QDoubleSpinBox* chi;
    QDoubleSpinBox* omega;
    QDoubleSpinBox* phi;
    QDoubleSpinBox* wave;
    QDialogButtonBox* buttons;
};

#endif // GUI_DIALOGS_RAWDATADIALOG_H
