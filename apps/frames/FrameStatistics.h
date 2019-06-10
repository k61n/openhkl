//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameStatistics.h
//! @brief     Defines class FrameStatistics
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <map>

#include "core/crystal/SpaceGroup.h"
#include "core/peak/Peak3D.h"
#include "core/peak/MergedData.h"

#include "apps/frames/NSXQFrame.h"

namespace Ui {
class FrameStatistics;
}

class CollectedPeaksModel;
class ExperimentItem;
class QAbstractButton;
class QTableView;
class QWidget;

class FrameStatistics : public NSXQFrame {
    Q_OBJECT

public:
    static FrameStatistics* create(const nsx::PeakList& peaks, const nsx::SpaceGroup& space_group);

    static FrameStatistics* Instance();

    ~FrameStatistics();

private slots:

    void update();

    void saveStatistics();

    void saveMergedPeaks();

    void saveUnmergedPeaks();

    void slotActionClicked(QAbstractButton* button);

private:
    explicit FrameStatistics(const nsx::PeakList& peaks, const nsx::SpaceGroup& space_group);

    void plotStatistics(int column);

    void saveToFullProf(QTableView* table);

    void saveToShelX(QTableView* table);

    void updateMergedPeaksTab();

    void updateUnmergedPeaksTab();

    void updateStatisticsTab();

private:
    static FrameStatistics* _instance;

    Ui::FrameStatistics* _ui;

    ExperimentItem* _experiment_item;

    nsx::PeakList _peaks;

    nsx::SpaceGroup _space_group;

    nsx::MergedData _merged_data;
};
