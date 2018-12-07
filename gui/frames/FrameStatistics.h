#pragma once

#include <map>

#include <core/CrystalTypes.h>
#include <core/MergedData.h>
#include <core/SpaceGroup.h>

#include "NSXQFrame.h"

namespace Ui {
class FrameStatistics;
}

class CollectedPeaksModel;
class ExperimentItem;
class QAbstractButton;
class QTableView;
class QWidget;

class FrameStatistics : public NSXQFrame
{
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

    void slotActionClicked(QAbstractButton *button);

private:

    explicit FrameStatistics(const nsx::PeakList& peaks, const nsx::SpaceGroup& space_group);

    void plotStatistics(int column);

    void saveToFullProf(QTableView* table);

    void saveToShelX(QTableView* table);

    void updateMergedPeaksTab();

    void updateUnmergedPeaksTab();

    void updateStatisticsTab();

private:

    static FrameStatistics *_instance;

    Ui::FrameStatistics *_ui;

    ExperimentItem *_experiment_item;

    nsx::PeakList _peaks;

    nsx::SpaceGroup _space_group;

    nsx::MergedData _merged_data;
};
