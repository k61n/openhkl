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

//! Tab of the SubframeSetup which contains the different peak lists
class TabPeaks : public QcrWidget {
    Q_OBJECT
 public:
    TabPeaks();

    void selectedListChanged(int i);
    void selectedExperimentChanged();

 private:
    QcrComboBox* foundPeaksLists;
    PeaksTableView* peaksTable;
};

#endif // GUI_PANELS_TABPEAKS_H
