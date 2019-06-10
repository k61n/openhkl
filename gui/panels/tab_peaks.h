//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/tab_peaks.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_TAB_PEAKS_H
#define GUI_PANELS_TAB_PEAKS_H

#include "gui/models/peaklists.h"
#include "gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class ListTab : public QcrWidget {
public:
    ListTab(FilteredPeaksModel* filteredModel);

private:
    PeaksTableView* view;
};

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

#endif // GUI_PANELS_TAB_PEAKS_H
