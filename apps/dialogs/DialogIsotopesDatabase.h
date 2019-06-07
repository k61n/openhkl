//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogIsotopesDatabase.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QDialog>

class QWidget;

namespace Ui {
class DialogIsotopesDatabase;
}

class DialogIsotopesDatabase : public QDialog {
    Q_OBJECT

public:
    explicit DialogIsotopesDatabase(QWidget* parent = 0);

    ~DialogIsotopesDatabase();

private:
    Ui::DialogIsotopesDatabase* ui;
};
