//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogRawData.h
//! @brief     Defines class DialogRawData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QDialog>

namespace Ui {
class DialogRawData;
}

class DialogRawData : public QDialog {
    Q_OBJECT

public:
    explicit DialogRawData(QWidget* parent = 0);
    ~DialogRawData();

    double wavelength();
    double deltaChi();
    double deltaOmega();
    double deltaPhi();

    bool swapEndian();
    bool rowMajor();
    int bpp();

    void setWavelength(double new_wavelength);

private:
    Ui::DialogRawData* ui;
};
