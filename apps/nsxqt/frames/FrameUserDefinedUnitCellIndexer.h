#pragma once

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

    void accept();

    void index();

private:

    static FrameUserDefinedUnitCellIndexer *_instance;

    Ui::FrameUserDefinedUnitCellIndexer *_ui;

    ExperimentItem *_experiment_item;
};
