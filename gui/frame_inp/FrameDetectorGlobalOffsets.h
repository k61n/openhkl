#pragma once

#include "NSXQFrame.h"
#include "ui_FrameDetectorGlobalOffsets.h"

class ExperimentItem;
class QAbstractButton;

class FrameDetectorGlobalOffsets : public NSXQFrame {
    Q_OBJECT

public:
    static FrameDetectorGlobalOffsets* create(ExperimentItem* experiment_item);

    static FrameDetectorGlobalOffsets* Instance();

    FrameDetectorGlobalOffsets(const FrameDetectorGlobalOffsets& other) = delete;

    FrameDetectorGlobalOffsets& operator=(const FrameDetectorGlobalOffsets& other) = delete;

    ~FrameDetectorGlobalOffsets();

private slots:
    void slotActionClicked(QAbstractButton* button);

private:
    explicit FrameDetectorGlobalOffsets(ExperimentItem* experiment_item);

    void fit();

private:
    static FrameDetectorGlobalOffsets* _instance;

    Ui::FrameDetectorGlobalOffsets* _ui;

    ExperimentItem* _experiment_item;
};
