#pragma once

#include "NSXQFrame.h"

namespace Ui {
class FrameSampleGlobalOffsets;
}

class ExperimentItem;
class QAbstractButton;

class FrameSampleGlobalOffsets : public NSXQFrame {
    Q_OBJECT

public:
    static FrameSampleGlobalOffsets* create(ExperimentItem* experiment_item);

    static FrameSampleGlobalOffsets* Instance();

    FrameSampleGlobalOffsets(const FrameSampleGlobalOffsets& other) = delete;

    FrameSampleGlobalOffsets& operator=(const FrameSampleGlobalOffsets& other) = delete;

    ~FrameSampleGlobalOffsets();

private slots:

    void slotActionClicked(QAbstractButton* button);

private:
    FrameSampleGlobalOffsets(ExperimentItem* experiment_item);

    void fit();

private:
    static FrameSampleGlobalOffsets* _instance;

    Ui::FrameSampleGlobalOffsets* _ui;

    ExperimentItem* _experiment_item;
};
