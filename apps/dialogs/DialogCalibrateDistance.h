//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogCalibrateDistance.h
//! @brief     Defines class DialogCalibrateDistance
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

class DialogCalibrateDistance;
}

class DialogCalibrateDistance : public QDialog {
    Q_OBJECT

 public:
    explicit DialogCalibrateDistance(QWidget* parent = 0);
    ~DialogCalibrateDistance();

 signals:
    void on_calibrateDistanceSpinBox_valueChanged(double arg1);
    void on_calibrateDistanceButtonOK_accepted();

 private slots:

 private:
    Ui::DialogCalibrateDistance* ui;
};
