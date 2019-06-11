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

#include <QCR/widgets/controls.h>
#include <QDialogButtonBox>
#include <QDialog>

class RawDataDialog : public QDialog {
public:
    RawDataDialog();
    double wavelength() { return wave->value(); }
    double deltaChi() { return chi->value(); }
    double deltaOmega() { return omega->value(); }
    double deltaPhi() { return phi->value(); }
    bool swapEndian() { return swapEndianness->getValue(); }
    bool rowMajor();
    int bpp();
    void setWavelength(double newWavelength) { wave->setCellValue(newWavelength); }

private:
    QcrComboBox* dataArrangement;
    QcrComboBox* dataFormat;
    QcrCheckBox* swapEndianness;
    QcrDoubleSpinBox* chi;
    QcrDoubleSpinBox* omega;
    QcrDoubleSpinBox* phi;
    QcrDoubleSpinBox* wave;
    QDialogButtonBox* buttons;
};

#endif //GUI_DIALOGS_RAWDATADIALOG_H
