//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameDetectorGlobalOffsets.h
//! @brief     Defines class FrameDetectorGlobalOffsets
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "apps/frames/NSXQFrame.h"

namespace Ui {
class FrameDetectorGlobalOffsets;
}

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
