//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/PeakProperties.h
//! @brief     Defines classes ListTab, TabPeaks
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_PEAKPROPERTIES_H
#define OHKL_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_PEAKPROPERTIES_H

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
    void currentProjectChanged();
    //! Refresh the input of the input fields
    void refreshInput();
    //! Set up the GUI size policies
    void setSizePolicies();

    //! Set up the GUI size policies
    void addMenuRequested();
    //! Jump to the peak finder subframe
    void jumpToFinder();
    //! Jump to the peak predictor subframe
    void jumpToPredictor();
    //! Jump to the peak filter subframe
    void jumpToFilter();
    //! Clone a peak collection (for comparison in detector window)
    void clonePeakCollection();
    //! Set up the GUI size policies
    void deleteCollection();
    //! Set PeakCollection
    void setPeakCollection(QString pc_name);

 public:
    //! Notify that the usr has selected another peak
    void changedPeakSelection(const QString& name);

 private:
    QComboBox* _peak_list_combo;
    PeakTableView* _peak_table;

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

    QStringList _peak_list;
};

#endif // OHKL_GUI_SUBFRAME_EXPERIMENT_PROPERTIES_PEAKPROPERTIES_H
