//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameRefiner.h
//! @brief     Defines class FrameRefiner
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <map>

#include "core/peak/Peak3D.h"
#include "core/experiment/DataTypes.h"
#include "core/algo/Refiner.h"

#include "apps/frames/NSXQFrame.h"

namespace Ui {

class FrameRefiner;
}

class CollectedPeaksModel;
class ExperimentItem;
class QAbstractButton;
class QWidget;

class FrameRefiner : public NSXQFrame {
    Q_OBJECT

public:
    static FrameRefiner* create(ExperimentItem* experiment_item, const nsx::PeakList& peaks);

    static FrameRefiner* Instance();

    ~FrameRefiner();

private slots:

    void slotActionClicked(QAbstractButton* button);

    void slotTabRemoved(int index);

private:
    explicit FrameRefiner(ExperimentItem* experiment_item, const nsx::PeakList& peaks);

    void accept();

    void refine();

private:
    static FrameRefiner* _instance;

    Ui::FrameRefiner* _ui;

    ExperimentItem* _experiment_item;
};
