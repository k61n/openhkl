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

#include "core/experiment/Experiment.h"
#include "gui/dialogs/ComboDialog.h"
#include "gui/dialogs/Messages.h"
#include "gui/dialogs/NewCellDialog.h"
#include "gui/MainWin.h" // for gGui
#include "gui/detector_window/DetectorWindow.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h" //for gSession
#include "gui/subframe_home/SubframeHome.h"
#include "gui/utility/SideBar.h"
#include "tables/crystal/SpaceGroup.h"

Actions::Actions()
{
    setupExperiment();
    setupData();
    setupPeaks();
    setupView();
    setupInstrument();
    setupOptions();
    setupRest();
    setupCell();
}

void Actions::setupExperiment()
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

    connect(remove_data, &QAction::triggered, this, &Actions::removeData);
}

void Actions::removeData()
{
    QString description{"Data set to remove"};
    QStringList data_list = gSession->currentProject()->getDataNames();
    std::unique_ptr<ComboDialog> dlg(new ComboDialog(data_list, description));
    dlg->exec();
    if (!dlg->itemName().isEmpty()) {
        std::string data_name = dlg->itemName().toStdString();
        gSession->currentProject()->experiment()->removeData(data_name);
        gSession->onDataChanged();
        gGui->sideBar()->refreshAll();
    }
}

void Actions::setupInstrument()
{
}

void Actions::setupOptions()
{
}

void Actions::setupPeaks()
{
    remove_peaks = new QAction("Remove peak collection");

    connect(remove_peaks, &QAction::triggered, this, &Actions::removePeaks);
}

void Actions::setupRest()
{
    about = new QAction("About");

    connect(about, &QAction::triggered, this, &Actions::aboutBox);
}

void Actions::setupCell()
{
    add_cell = new QAction("Add unit cell");
    remove_cell = new QAction("Remove unit cell");

    connect(add_cell, &QAction::triggered, this, &Actions::addCell);
    connect(remove_cell, &QAction::triggered, this, &Actions::removeCell);
}

void Actions::addCell()
{
    QStringList space_groups;
    for (const auto& symbol : nsx::SpaceGroup::symbols())
        space_groups.push_back(QString::fromStdString(symbol));
    std::unique_ptr<NewCellDialog> dlg(new NewCellDialog(space_groups));
    dlg->exec();
    if (!dlg->unitCellName().isEmpty()) {
        nsx::Experiment* expt = gSession->currentProject()->experiment();
        expt->addUnitCell(
            dlg->unitCellName().toStdString(), dlg->a(), dlg->b(), dlg->c(),
            dlg->alpha(), dlg->beta(), dlg->gamma(), dlg->spaceGroup().toStdString());
        gGui->onUnitCellChanged();
        gGui->sideBar()->refreshAll();
    }
}

void Actions::removeCell()
{
    QString description{"Unit cell to remove"};
    QStringList cell_list = gSession->currentProject()->getUnitCellNames();
    std::unique_ptr<ComboDialog> dlg(new ComboDialog(cell_list, description));
    dlg->exec();
    if (!dlg->itemName().isEmpty()) {
        std::string data_name = dlg->itemName().toStdString();
        gSession->currentProject()->experiment()->removeUnitCell(data_name);
        gGui->onUnitCellChanged();
        gGui->sideBar()->refreshAll();
    }
}

void Actions::removePeaks()
{
    QString description{"Peak collection to remove"};
    QStringList peaks_list = gSession->currentProject()->getPeakListNames();
    std::unique_ptr<ComboDialog> dlg(new ComboDialog(peaks_list, description));
    dlg->exec();
    if (!dlg->itemName().isEmpty()) {
        QString peaks_name = dlg->itemName();
        gSession->currentProject()->removePeakModel(peaks_name);
        gGui->onPeaksChanged();
        gGui->sideBar()->refreshAll();
    }
}

void Actions::aboutBox()
{
    std::unique_ptr<AboutBox> dlg(new AboutBox());
    dlg->exec();
}
