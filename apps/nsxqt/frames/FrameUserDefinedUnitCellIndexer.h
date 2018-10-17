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

private:

    explicit FrameUserDefinedUnitCellIndexer(ExperimentItem *experiment_item, const nsx::PeakList &peaks);

    void buildUnitCellsTable();

    void accept();

    void index();

private:

    static FrameUserDefinedUnitCellIndexer *_instance;

    Ui::FrameUserDefinedUnitCellIndexer *_ui;

    ExperimentItem *_experiment_item;

    nsx::UserDefinedUnitCellIndexer _indexer;

    std::vector<std::pair<nsx::sptrUnitCell,double>> _solutions;
};
