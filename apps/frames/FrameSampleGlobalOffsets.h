//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameSampleGlobalOffsets.h
//! @brief     Defines ###CLASSES###
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
