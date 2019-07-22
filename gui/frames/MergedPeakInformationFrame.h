//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/MergedPeakInformationFrame.h
//! @brief     Defines class MergedPeakInformationFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_MERGEDPEAKINFORMATIONFRAME_H
#define GUI_FRAMES_MERGEDPEAKINFORMATIONFRAME_H

#include "core/analyse/MergedData.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/SpaceGroup.h"
#include "gui/graphics/SXPlot.h"
#include <QCR/widgets/actions.h>
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QTableView>

class MergedPeakInformationFrame : public QcrFrame {
 public:
    MergedPeakInformationFrame(nsx::SpaceGroup group, nsx::PeakList list);

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

    nsx::PeakList peakList;
    nsx::SpaceGroup spaceGroup;
    nsx::MergedData mergedData;

    QTableView* statisticsView;
    QcrDoubleSpinBox* dmin;
    QcrDoubleSpinBox* dmax;
    QcrSpinBox* dshells;
    QcrCheckBox* friedel;
    QcrComboBox* plottableStatistics;
    SXPlot* statisticsPlot;
    QTableView* mergedView;
    QcrComboBox* filetypesMerged;
    QTableView* unmergedView;
    QcrComboBox* filetypesUnmerged;
};

#endif // GUI_FRAMES_MERGEDPEAKINFORMATIONFRAME_H
