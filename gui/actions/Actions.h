//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/Actions.h
//! @brief     Defines class Actions
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_ACTIONS_ACTIONS_H
#define NSX_GUI_ACTIONS_ACTIONS_H

#include <QAction>
#include <QObject>

//! Collection of trigger and toggle actions, for use as member of MainWin.
class Actions : public QObject {
    Q_OBJECT

 public:
    Actions();

    void removeData();

    // Files:
    QAction* new_experiment;
    QAction* load_experiment;
    QAction* save_experiment;
    QAction* save_all_experiment;
    QAction* remove_experiment;
    QAction* quit;

    // View:
    QAction* detector_window;

    // Data
    QAction* add_data;
    QAction* remove_data;
    QAction* add_raw;
    QAction* add_hdf5;
    QAction* add_nexus;

    // Peaks
    QAction* add_peaks;
    QAction* remove_peaks;

    // Unit Cells
    QAction* add_cell;
    QAction* remove_cell;

    // Help
    QAction* about; 

 private:
    void setupFiles();
    void setupView();
    void setupData();
    void setupInstrument();
    void setupPeaks();
    void setupExperiment();
    void setupOptions();
    void setupRest();
    void setupCell();
    void addCell();
    void removeCell();
    void removePeaks();
    void aboutBox();
};

#endif // NSX_GUI_ACTIONS_ACTIONS_H
