//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogHDF5Converter.h
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

#include "core/data/DataTypes.h"

class QAbstractButton;

namespace Ui {
class DialogHDF5Converter;
}

class DialogHDF5Converter : public QDialog {
    Q_OBJECT

public:
    explicit DialogHDF5Converter(const nsx::DataList& numors, QWidget* parent = 0);
    ~DialogHDF5Converter();

private slots:

    void slotActionClicked(QAbstractButton* button);

    void browseOutputDirectory();

private:
    void convert();

private:
    Ui::DialogHDF5Converter* _ui;

    nsx::DataList _numors;
};
