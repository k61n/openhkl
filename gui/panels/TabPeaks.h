//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/TabPeaks.h
//! @brief     Defines classes ListTab, TabPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_TABPEAKS_H
#define GUI_PANELS_TABPEAKS_H

#include "gui/models/PeaksTable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

//! Tab with one PeaksTable of the TabPeaks
class ListTab : public QcrWidget {
// public:
//    ListTab(FilteredPeaksModel* filteredModel);

// private:
//    PeaksTableView* view;
};

//! Tab of the SubframeSetup which contains the different peak lists
class TabPeaks : public QcrWidget {
    Q_OBJECT
 public:
    TabPeaks();
 private slots:
    void slotSelectedListChanged(int i);

 private:
    QcrComboBox* foundPeaksLists;
    QcrTabWidget* filtered;
};

#endif // GUI_PANELS_TABPEAKS_H
