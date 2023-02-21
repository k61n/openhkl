//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/actions/Actions.h
//! @brief     Defines class Actions
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_ACTIONS_ACTIONS_H
#define OHKL_GUI_ACTIONS_ACTIONS_H

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
    QAction* save_experiment_as;
    QAction* save_all_experiment;
    QAction* export_peaks;
    QAction* remove_experiment;
    QAction* quit;

    // View:
    QAction* detector_window;
    QAction* instrumentstate_window;
    QAction* log_window;
    QAction* close_peak_windows;

    // Data
    QAction* show_input_files;
    QAction* add_data;
    QAction* add_single_image;
    QAction* remove_data;
    QAction* add_raw;
    QAction* add_hdf5;
    QAction* add_nexus;

    // Peaks
    QAction* show_peaks;
    QAction* remove_peaks;
    QAction* clone_peaks;

    // Unit Cells
    QAction* add_cell;
    QAction* remove_cell;

    // Help
    QAction* documentation;
    QAction* about;

 private:
    void setupExperiment();
    void setupView();
    void setupData();
    void setupInstrument();
    void setupPeaks();
    void setupOptions();
    void setupRest();
    void setupCell();
    void addCell();
    void removeExperiment();
    void removeCell();
    void removePeaks();
    void clonePeaks();
    void aboutBox();
    void closePeakWindows();
    void openWebsite();
    void exportPeaks();
};

#endif // OHKL_GUI_ACTIONS_ACTIONS_H
