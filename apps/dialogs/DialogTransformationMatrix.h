//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogTransformationMatrix.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <Eigen/Dense>

#include <QDialog>

#include "core/experiment/CrystalTypes.h"

namespace Ui {
class DialogTransformationMatrix;
}

class Widget;

class DialogTransformationMatrix : public QDialog {
    Q_OBJECT

signals:
    void getMatrix(const Eigen::Matrix3d& m);

public:
    explicit DialogTransformationMatrix(nsx::sptrUnitCell unitCell, QWidget* parent = 0);
    ~DialogTransformationMatrix();

public slots:

    virtual void accept() override;

private:
    Ui::DialogTransformationMatrix* ui;
    nsx::sptrUnitCell _unitCell;
};
