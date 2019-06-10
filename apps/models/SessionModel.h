//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SessionModel.h
//! @brief     Defines class SessionModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QStandardItemModel>

#include "core/crystal/UnitCell.h"
#include "core/experiment/DataTypes.h"

class ExperimentItem;

class SessionModel : public QStandardItemModel {

    Q_OBJECT

public:
    explicit SessionModel();

    ~SessionModel();

    nsx::PeakList peaks(nsx::sptrDataSet data) const;

    void selectData(nsx::sptrDataSet data);

    ExperimentItem* selectExperiment(nsx::sptrDataSet data);

signals:

    void plotData(nsx::sptrDataSet);

    void inspectWidget(QWidget*);

    void updatePeaks();

    void signalSelectedDataChanged(nsx::sptrDataSet, int frame);

    void signalSelectedPeakChanged(nsx::sptrPeak3D peak);

    void signalEnabledPeakChanged(nsx::sptrPeak3D peak);

    void signalMaskedPeaksChanged(const nsx::PeakList& peaks);

    void signalUnitCellRemoved(nsx::sptrUnitCell unit_cell);

public slots:

    void createNewExperiment();

    void onItemChanged(QStandardItem* item);
};
