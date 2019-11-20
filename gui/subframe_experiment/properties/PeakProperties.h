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

#include <QComboBox>
#include <QLabel>
#include <QPushButton>

//! Tab of the SubframeSetup which contains the different peak collections
class PeakProperties : public QWidget {

public:

   //! Constructor
   PeakProperties();
   //! Inform that the selected peak has changed
   void selectedPeaksChanged();
   //! Inform that the selected experiment has changed
   void selectedExperimentChanged();
   //! Refresh the input of the input fields
   void refreshInput();
   //! Set up the GUI size policies
   void setSizePolicies();

   //! Set up the GUI size policies
   void addMenuRequested();
   //! Set up the GUI size policies
   void jumpToFinder();
   //! Set up the GUI size policies
   void jumpToPredictor();
   //! Set up the GUI size policies
   void jumpToFilter();
   //! Set up the GUI size policies
   void deleteCollection();

public:

   //! Notify that the usr has selected another peak
   void changedPeakSelection(const QString& name);

private:

   QComboBox* _peak_list_combo;
   PeaksTableView* _peak_table;

   QLabel* _type;
   QLabel* _file_name;
   QLabel* _kernel_name;
   QLabel* _parent_name;
   QLabel* _peak_num;
   QLabel* _valid;
   QLabel* _non_valid;

   QPushButton* _add;
   QPushButton* _remove;
   QPushButton* _filter;

   QSizePolicy* _size_policy_widgets;
   QSizePolicy* _size_policy_box;
   QSizePolicy* _size_policy_right;
   QSizePolicy* _size_policy_fixed;

};

#endif // GUI_PANELS_TABPEAKS_H
