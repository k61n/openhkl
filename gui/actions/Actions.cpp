//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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
#include "core/shape/PeakCollection.h"
#include "gui/MainWin.h" // for gGui
#include "gui/connect/Sentinel.h" // for sentinel
#include "gui/dialogs/ClonePeakDialog.h"
#include "gui/dialogs/ComboDialog.h"
#include "gui/dialogs/Messages.h"
#include "gui/dialogs/NewCellDialog.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h" //for gSession
#include "gui/subframe_home/SubframeHome.h"
#include "gui/subwindows/DetectorWindow.h"
#include "gui/subwindows/LogWindow.h"
#include "gui/subwindows/PeakWindow.h"
#include "gui/utility/SideBar.h"
#include "gui/widgets/LogWidget.h"
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
    save_experiment_as = new QAction("Save as"); 
    save_experiment_as = new QAction("Save as");
    save_all_experiment = new QAction("Save all");
    remove_experiment = new QAction("Remove experiment");
    quit = new QAction("Quit");

    connect(new_experiment, &QAction::triggered, []() { gGui->home->createNew(); });
    connect(new_experiment, &QAction::triggered, []() { gGui->sideBar()->refreshCurrent(); });
    connect(load_experiment, &QAction::triggered, []() { gGui->home->loadFromFile(); });
    connect(load_experiment, &QAction::triggered, []() { gGui->sideBar()->refreshCurrent(); });
    connect(save_experiment, &QAction::triggered, []() { gGui->home->saveCurrent(); });
    connect(save_experiment_as, &QAction::triggered, []() { gGui->home->saveCurrent(true); });
    connect(save_all_experiment, &QAction::triggered, []() { gGui->home->saveAll(); });
    connect(remove_experiment, &QAction::triggered, this, &Actions::removeExperiment);

    connect(quit, &QAction::triggered, []() { gGui->close(); }); 
}

void Actions::setupView()
{
    detector_window = new QAction("Open detector window");
    instrumentstate_window = new QAction("Open instrument states window");
    log_window = new QAction("Open log window");
    close_peak_windows = new QAction("Close all peak windows");

    connect(detector_window, &QAction::triggered, []() {
        gGui->detector_window->show();
        gGui->detector_window->refreshAll();
    });
    connect(instrumentstate_window, &QAction::triggered, []() {
        //gGui->instrumentstate_window->show();
        //gGui->instrumentstate_window->refreshAll();
    });
    connect(log_window, &QAction::triggered, []() {
        gGui->log_window->show();
        gGui->log_window->logWidget()->verticalScrollBar()->setValue(
            gGui->log_window->logWidget()->verticalScrollBar()->maximum());
    });
    connect(close_peak_windows, &QAction::triggered, this, &Actions::closePeakWindows);
}

void Actions::setupData()
{
    add_data = new QAction("Add data set");
    show_input_files = new QAction("Show input files");
    remove_data = new QAction("Remove data set");
    add_raw = new QAction("Add raw/tiff data");
    add_hdf5 = new QAction("Add NSX(HDF5) data");
    add_nexus = new QAction("Add Nexus data");

    connect(add_raw, &QAction::triggered, []() { // can cause a crash without checking
        if (gSession->loadRawData())
            gGui->sideBar()->refreshCurrent();
    });
    connect(add_hdf5, &QAction::triggered, []() { gSession->loadData(nsx::DataFormat::NSX); });
    connect(add_hdf5, &QAction::triggered, []() { gSession->loadData(nsx::DataFormat::NEXUS); });
    connect(add_hdf5, &QAction::triggered, []() { gGui->sideBar()->refreshCurrent(); });
    connect(add_nexus, &QAction::triggered, []() { gGui->sideBar()->refreshCurrent(); });

    connect(remove_data, &QAction::triggered, this, &Actions::removeData);    

    connect(show_input_files, &QAction::triggered,
        [ ]() {
            gGui->input_files_window->show();
            gGui->input_files_window->refreshAll();

        }
    ); 
}

void Actions::removeExperiment()
{
    QString description{"Experiment to remove"};

    QStringList expt_list;
    std::vector<unsigned int> id_list;

    for (int expt_idx = 0; expt_idx < gSession->numExperiments(); ++expt_idx) {
        std::string name = gSession->experimentAt(expt_idx)->experiment()->name();
        expt_list.push_back(QString::fromStdString(name));
        id_list.push_back(gSession->experimentAt(expt_idx)->id());
    }
    std::unique_ptr<ComboDialog> dlg(new ComboDialog(expt_list, description));
    dlg->exec();

    if (dlg->itemName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    gGui->sideBar()->manualSelect(0); // switch to SubframeHome
    gSession->removeExperiment(id_list[dlg->itemIndex()]);
    gSession->onExperimentChanged();
}

void Actions::removeData()
{
    QString description{"Data set to remove"};
    QStringList data_list = gSession->currentProject()->getDataNames();
    std::unique_ptr<ComboDialog> dlg(new ComboDialog(data_list, description));
    dlg->exec();

    if (dlg->itemName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    gGui->sideBar()->manualSelect(0); // switch to SubframeHome
    std::string data_name = dlg->itemName().toStdString();
    gSession->currentProject()->experiment()->removeData(data_name);
    gSession->onDataChanged();
    data_list = gSession->currentProject()->getDataNames();
}

void Actions::setupInstrument() { }

void Actions::setupOptions() { }

void Actions::setupPeaks()
{
    show_peaks = new QAction("Show Peaks");
    remove_peaks = new QAction("Remove peak collection");
    clone_peaks = new QAction("Clone peak collection");

    connect(remove_peaks, &QAction::triggered, this, &Actions::removePeaks);
    connect(clone_peaks, &QAction::triggered, this, &Actions::clonePeaks);

    connect(
         show_peaks, &QAction::triggered,
        [ ]() {
            gGui->peak_list_window->show();
            gGui->peak_list_window->refreshAll(  );
        }        
    );  
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

    if (dlg->unitCellName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    nsx::Experiment* expt = gSession->currentProject()->experiment();
    expt->addUnitCell(
        dlg->unitCellName().toStdString(), dlg->a(), dlg->b(), dlg->c(), dlg->alpha(),
        dlg->beta(), dlg->gamma(), dlg->spaceGroup().toStdString());
    gSession->onUnitCellChanged();
    auto cell_list = gSession->currentProject()->getUnitCellNames();
    gGui->sideBar()->refreshCurrent();
}

void Actions::removeCell()
{
    QString description{"Unit cell to remove"};
    QStringList cell_list = gSession->currentProject()->getUnitCellNames();
    std::unique_ptr<ComboDialog> dlg(new ComboDialog(cell_list, description));
    dlg->exec();

    if (dlg->itemName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    std::string data_name = dlg->itemName().toStdString();
    gSession->currentProject()->experiment()->removeUnitCell(data_name);
    gSession->onUnitCellChanged();
    cell_list = gSession->currentProject()->getUnitCellNames();
    gGui->sideBar()->refreshCurrent();
}

void Actions::removePeaks()
{
    gGui->setReady(false);
    QString description{"Peak collection to remove"};
    QStringList peaks_list = gSession->currentProject()->getPeakListNames();
    if (peaks_list.empty())
        return;

    std::unique_ptr<ComboDialog> dlg(new ComboDialog(peaks_list, description));
    dlg->exec();
    if (dlg->itemName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    QString peaks_name = dlg->itemName();
    nsx::Experiment* experiment = gSession->currentProject()->experiment();

    experiment->removePeakCollection(peaks_name.toStdString());

    gSession->currentProject()->removePeakModel(peaks_name);
    gSession->onPeaksChanged();
    peaks_list = gSession->currentProject()->getPeakListNames();

    gGui->sideBar()->refreshCurrent();
    gGui->setReady(true);
}

void Actions::clonePeaks()
{
    gGui->setReady(false);
    QString description{"Peak collection to clone"};
    QString suggested_name = QString::fromStdString(
        gSession->currentProject()->experiment()->generatePeakCollectionName());
    QStringList peaks_list = gSession->currentProject()->getPeakListNames();

    if (peaks_list.empty())
        return;

    std::unique_ptr<ClonePeakDialog> dlg(new ClonePeakDialog(peaks_list, suggested_name));
    dlg->exec();

    if (dlg->clonedCollectionName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    QString original = dlg->originalCollectionName();
    QString cloned = dlg->clonedCollectionName();
    gSession->currentProject()->clonePeakCollection(original, cloned);
    peaks_list = gSession->currentProject()->getPeakListNames();
    gSession->onPeaksChanged();
    gGui->setReady(true);
}

void Actions::aboutBox()
{
    std::unique_ptr<AboutBox> dlg(new AboutBox());
    dlg->exec();
}

void Actions::closePeakWindows()
{
    for (PeakWindow* window : gGui->peak_windows)
        window->close();
    gGui->peak_windows.clear();
}
