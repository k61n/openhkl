//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogExperiment.h
//! @brief     Defines class DialogExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QDialog>

class Widget;

namespace Ui {

class DialogExperiment;
}

class DialogExperiment : public QDialog {
    Q_OBJECT

public:
    explicit DialogExperiment(QWidget* parent = 0);
    ~DialogExperiment();

    QString getExperimentName() const;
    QString getInstrumentName() const;

private:
    Ui::DialogExperiment* ui;
};
