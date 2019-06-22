//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameAutoIndexer.h
//! @brief     Defines class FrameAutoIndexer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <utility>
#include <vector>

#include "core/peak/Peak3D.h"

#include "apps/frames/NSXQFrame.h"

namespace Ui {

class FrameAutoIndexer;
}

class CollectedPeaksModel;
class ExperimentItem;
class QAbstractButton;

class FrameAutoIndexer : public NSXQFrame {
    Q_OBJECT

 public:
    static FrameAutoIndexer* create(ExperimentItem* experiment_item, const nsx::PeakList& peaks);

    static FrameAutoIndexer* Instance();

    ~FrameAutoIndexer();

 private slots:

    void slotActionClicked(QAbstractButton* button);

    void slotTabEdited(int index);

    void slotTabRemoved(int index);

    void selectSolution(int);

 private:
    FrameAutoIndexer(ExperimentItem* experiment_item, const nsx::PeakList& peaks);

    void accept();

    void buildSolutionsTable();

    void resetUnitCell();

    void run();

 private:
    static FrameAutoIndexer* _instance;

    Ui::FrameAutoIndexer* _ui;

    ExperimentItem* _experiment_item;

    CollectedPeaksModel* _peaks_model;

    std::vector<std::pair<nsx::sptrPeak3D, std::shared_ptr<nsx::UnitCell>>> _defaults;

    std::vector<std::pair<nsx::sptrUnitCell, double>> _solutions;
};
