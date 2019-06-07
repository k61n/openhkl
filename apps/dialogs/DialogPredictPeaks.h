//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogPredictPeaks.h
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

#include <core/CrystalTypes.h>

class CollectedPeaksModel;
class ExperimentItem;
class QAbstractButton;

namespace Ui {
class DialogPredictPeaks;
}

class DialogPredictPeaks : public QDialog {
    Q_OBJECT

public:
    static DialogPredictPeaks* create(
        ExperimentItem* experiment_tree, const nsx::UnitCellList& peaks, QWidget* parent = nullptr);

    static DialogPredictPeaks* Instance();

    ~DialogPredictPeaks();

    double dMin() const;
    double dMax() const;
    double radius() const;
    double nFrames() const;

    int minNeighbors() const;

    int interpolation() const;

    nsx::sptrUnitCell cell();

public slots:

    virtual void accept() override;

private slots:

    void slotActionClicked(QAbstractButton* button);

private:
    DialogPredictPeaks(
        ExperimentItem* experiment_tree, const nsx::UnitCellList& unit_cells,
        QWidget* parent = nullptr);

    void predictPeaks();

private:
    static DialogPredictPeaks* _instance;

    Ui::DialogPredictPeaks* _ui;

    ExperimentItem* _experiment_item;

    CollectedPeaksModel* _peaks_model;
};
