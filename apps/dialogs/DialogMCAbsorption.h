//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogMCAbsorption.h
//! @brief     Defines class DialogMCAbsorption
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QDialog>

#include "core/instrument/InstrumentTypes.h"
#include "models/SessionModel.h"

class QWidget;

namespace Ui {

class DialogMCAbsorption;
}

class ExperimentItem;

class DialogMCAbsorption : public QDialog {
    Q_OBJECT

 public:
    explicit DialogMCAbsorption(ExperimentItem* experiment_item, QWidget* parent = 0);
    ~DialogMCAbsorption();

 private slots:
    void on_pushButton_run_pressed();

 private:
    Ui::DialogMCAbsorption* ui;
    ExperimentItem* _experiment_item;
};
