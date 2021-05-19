//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/Actions.cpp
//! @brief     Implements class Actions
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/actions/Actions.h"

#include "gui/MainWin.h" // for gGui
#include "gui/detector_window/DetectorWindow.h"
#include "gui/models/Session.h" //for gSession
#include "gui/subframe_home/SubframeHome.h"
#include "gui/utility/SideBar.h"

Actions::Actions()
{
    setupData();
    setupExperiment();
    setupPeaks();
    setupFiles();
    setupView();
    setupInstrument();
    setupOptions();
    setupRest();
    setupCell();
}

void Actions::setupFiles()
{
    new_experiment = new QAction("New experiment");
    load_experiment = new QAction("Load experiment");
    save_experiment = new QAction("Save");
    save_all_experiment = new QAction("Save all");
    remove_experiment = new QAction("Remove experiment");
    quit = new QAction("Quit");

    connect(new_experiment, &QAction::triggered, []() { gGui->home->createNew(); });
    connect(new_experiment, &QAction::triggered, []() { gGui->sideBar()->refreshAll(); });
    connect(load_experiment, &QAction::triggered, []() { gGui->home->loadFromFile(); });
    connect(load_experiment, &QAction::triggered, []() { gGui->sideBar()->refreshAll(); });
    connect(save_experiment, &QAction::triggered, []() { gGui->home->saveCurrent(); });
    connect(save_all_experiment, &QAction::triggered, []() { gGui->home->saveAll(); });
    // connect(remove_experiment, &QAction::triggered, [](){gGui->home->remo();} );
    connect(quit, &QAction::triggered, []() { gGui->close(); });
}

void Actions::setupView()
{
    detector_window = new QAction("Open detector window");

    connect(detector_window, &QAction::triggered, []() {
        gGui->detector_window->show();
        gGui->detector_window->refreshAll();
    });
}

void Actions::setupData()
{
    add_data = new QAction("Add data set");
    remove_data = new QAction("Remove data set");
    add_raw = new QAction("Add raw/tiff data");
    add_hdf5 = new QAction("Add HDF5 data");
    add_nexus = new QAction("Add Nexus data");

    connect(add_raw, &QAction::triggered, []() { gSession->loadRawData(); });
    connect(add_hdf5, &QAction::triggered, []() { gSession->loadData(nsx::DataFormat::HDF5); });
    connect(add_hdf5, &QAction::triggered, []() { gSession->loadData(nsx::DataFormat::NEXUS); });
    connect(add_raw, &QAction::triggered, []() { gGui->sideBar()->refreshAll(); });
    connect(add_hdf5, &QAction::triggered, []() { gGui->sideBar()->refreshAll(); });
    connect(add_nexus, &QAction::triggered, []() { gGui->sideBar()->refreshAll(); });
}

void Actions::setupExperiment()
{
}

void Actions::setupInstrument()
{
}

void Actions::setupOptions()
{
}

void Actions::setupPeaks()
{
    add_peaks = new QAction("Add peak collection");
    remove_peaks = new QAction("Remove peak collection");
}

void Actions::setupRest()
{
    about = new QAction("About");
}

void Actions::setupCell()
{
    add_cell = new QAction("Add unit cell");
    remove_cell = new QAction("Remove unit cell");
}
