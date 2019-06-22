//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/UnitCellPropertyWidget.h
//! @brief     Defines class UnitCellPropertyWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <memory>

#include <QWidget>

#include "core/peak/Peak3D.h"

namespace Ui {

class UnitCellPropertyWidget;
}

class QStandardItem;
class QString;

class UnitCellItem;

class UnitCellPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit UnitCellPropertyWidget(UnitCellItem* caller, QWidget* parent = 0);
    ~UnitCellPropertyWidget();

private slots:

    void setUnitCellName();

    void getLatticeParams();

    void setLatticeParams();

    void setChemicalFormula();

    void setSpaceGroup(QString sg);

    void setZValue(int z);

    void setMassDensity() const;

    void activateSpaceGroupCompletion(QString sg);

    void setIndexingTolerance(double);

    void update(QStandardItem* item = nullptr);

private:
    void updateCellParameters();

private:
    UnitCellItem* _unitCellItem;

    Ui::UnitCellPropertyWidget* ui;
};
