//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/StatisticsFrame.h
//! @brief     Defines class StatisticsFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_STATISTICSFRAME_H
#define GUI_FRAMES_STATISTICSFRAME_H

#include "core/analyse/MergedData.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/SpaceGroup.h"
#include "gui/graphics/SXPlot.h"
#include <QCR/widgets/actions.h>
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QTableView>

class StatisticsFrame : public QcrFrame {
 public:
    StatisticsFrame(nsx::SpaceGroup group, nsx::PeakList list);

 private:
    void update();
    void saveStatistics();
    void saveMergedPeaks();
    void saveUnmergedPeaks();
    void plotStatistics(int column);
    void saveToFullProf(QTableView* table);
    void saveToShelX(QTableView* table);
    void updateMergedPeaksTab();
    void updateUnmergedPeaksTab();
    void updateStatisticsTab();

    nsx::PeakList peaks;
    nsx::SpaceGroup space;
    nsx::MergedData mergedData;

    QcrTabWidget* tabs;
    QcrWidget* stats;
    QTableView* viewStats;
    QcrDoubleSpinBox* dmin;
    QcrDoubleSpinBox* dmax;
    QcrSpinBox* dshells;
    QcrCheckBox* friedel;
    QcrComboBox* selectedStats;
    SXPlot* plot;
    QcrTextTriggerButton* saveStats;
    QcrWidget* merged;
    QTableView* mergedView;
    QcrComboBox* typesMerged;
    QcrTextTriggerButton* saveMerged;
    QcrWidget* unmerged;
    QTableView* unmergedView;
    QcrComboBox* typesUnmerged;
    QcrTextTriggerButton* saveUnmerged;
};

#endif // GUI_FRAMES_STATISTICSFRAME_H
