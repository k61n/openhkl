#pragma once

#include <map>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/Refiner.h>

#include "NSXQFrame.h"

namespace Ui {
class FrameRefiner;
}

class CollectedPeaksModel;
class ExperimentItem;
class QAbstractButton;
class QWidget;

class FrameRefiner : public NSXQFrame
{
    Q_OBJECT

public:

    static FrameRefiner* create(ExperimentItem* experiment_item, const nsx::PeakList &peaks);

    static FrameRefiner* Instance();

    ~FrameRefiner();

private slots:

    void slotActionClicked(QAbstractButton *button);

    void slotTabRemoved(int index);

private:

    explicit FrameRefiner(ExperimentItem *experiment_item, const nsx::PeakList &peaks);

    void accept();

    void refine();

private:

    static FrameRefiner *_instance;

    Ui::FrameRefiner *_ui;

    ExperimentItem *_experiment_item;
};
