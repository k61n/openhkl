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

#include "gui/views/PeakTableView.h"
#include "gui/models/PeakModel.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

#include <QComboBox>

//! Tab of the SubframeSetup which contains the different peak lists
class TabPeaks : public QcrWidget {
   Q_OBJECT
public:
   TabPeaks();
   void selectedPeaksChanged();
   void selectedExperimentChanged();

private:
   QComboBox* peak_list_combo;
   PeaksTableView* peak_table;

   QLabel* type;
   QLabel* file_name;
   QLabel* kernel_name;
   QLabel* parent_name;
   QLabel* peak_num;
   QLabel* valid;
   QLabel* non_valid;

public slots:
   void changedPeakSelection(const QString& name);


};

#endif // GUI_PANELS_TABPEAKS_H
