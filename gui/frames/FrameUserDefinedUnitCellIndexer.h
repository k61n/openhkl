#pragma once

#include <utility>
#include <vector>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/UserDefinedUnitCellIndexer.h>

#include "NSXQFrame.h"

namespace Ui {
class FrameUserDefinedUnitCellIndexer;
}

class ExperimentItem;
class QAbstractButton;

class FrameUserDefinedUnitCellIndexer : public NSXQFrame
{
    Q_OBJECT

public:

    static FrameUserDefinedUnitCellIndexer* create(ExperimentItem* experiment_item, const nsx::PeakList &peaks);

    static FrameUserDefinedUnitCellIndexer* Instance();

    ~FrameUserDefinedUnitCellIndexer();

private slots:

    void slotActionClicked(QAbstractButton *button);

    void slotTabEdited(int index);

    void slotTabRemoved(int index);

    void slotSelectSolution(int);

private:

    explicit FrameUserDefinedUnitCellIndexer(ExperimentItem *experiment_item, const nsx::PeakList &peaks);

    void accept();

    void buildUnitCellsTable();

    void index();

    void resetPeaks();

private:

    static FrameUserDefinedUnitCellIndexer *_instance;

    Ui::FrameUserDefinedUnitCellIndexer *_ui;

    ExperimentItem *_experiment_item;

    std::vector<std::pair<nsx::sptrPeak3D,nsx::sptrUnitCell>> _defaults;

    nsx::UserDefinedUnitCellIndexer _indexer;

    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;
};
