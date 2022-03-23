//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/Menus.cpp
//! @brief     Implements class Menus
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/actions/Menus.h"

#include "gui/MainWin.h"
#include "gui/actions/Actions.h"
#include "gui/models/Session.h"
#include "gui/models/Project.h"
#include "core/experiment/Experiment.h"

#include <QAction>
#include <QMenu>
#include <iostream>

class Project;

//! Initialize the menu bar.
Menus::Menus(QMenuBar* menu_bar) : _menu_bar{menu_bar}
{
    Actions* actions = gGui->triggers;

    _menu_bar->setNativeMenuBar(true);

    _expt_menu = _menu_bar->addMenu("Experiment");
    _view_menu = _menu_bar->addMenu("View");
    _data_menu = _menu_bar->addMenu("Data");
    _peaks_menu = _menu_bar->addMenu("Peaks");
    _cells_menu = _menu_bar->addMenu("Cells");
    _help_menu = _menu_bar->addMenu("Help");

    _expt_menu->addAction(actions->new_experiment);
    _expt_menu->addAction(actions->load_experiment);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->save_experiment);
    _expt_menu->addAction(actions->save_experiment_as);
    _expt_menu->addAction(actions->save_all_experiment);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->remove_experiment);
    _expt_menu->addAction(actions->quit);

    // View menu
    _view_menu->addAction(actions->detector_window);
    _view_menu->addAction(actions->instrumentstate_window);
    _view_menu->addAction(actions->log_window);
    _view_menu->addSeparator();
    _view_menu->addAction(actions->close_peak_windows);

    QMenu* _data_sub = _data_menu->addMenu("Add data set");
    _data_sub->addAction(actions->add_raw);
    _data_sub->addAction(actions->add_hdf5);
    _data_sub->addAction(actions->add_nexus);
    _data_menu->addAction(actions->remove_data);

    _peaks_menu->addAction(actions->clone_peaks);
    _peaks_menu->addAction(actions->remove_peaks);

    _cells_menu->addAction(actions->add_cell);
    _cells_menu->addAction(actions->remove_cell);

    _help_menu->addAction(actions->about);

    toggle_entries();
}

QAction* Menus::separator() const
{
    QAction* ret = new QAction{_menu_bar};
    ret->setSeparator(true);
    return ret;
}

QMenu* Menus::actionsToMenu(const char* menuName, QList<QAction*> actions)
{
    QMenu* menu = new QMenu{menuName};
    _menu_bar->addMenu(menu);
    menu->addActions(actions);
    QString prefix = QString("%1: ").arg(menu->title().remove('&'));
    for (auto action : actions)
        action->setToolTip(prefix + action->toolTip());
    return menu;
}

// toggles Menu entries
void Menus::toggle_entries()
{  
    //return;
    Actions* actions = gGui->triggers;

    if (gSession->numExperiments() == 0){ // just disables everything
        actions->save_all_experiment->setDisabled(true);
        actions->save_experiment->setDisabled(true);
        actions->save_experiment_as->setDisabled(true);

        actions->remove_experiment->setDisabled(true);

        _view_menu->setDisabled(true);    
        _data_menu->setDisabled(true);    
        _peaks_menu->setDisabled(true);
        _cells_menu->setDisabled(true);
        return;
    }     
    
    auto prj = gSession->experimentAt(
    gSession->currentProjectNum());
    auto expt = prj->experiment();

    bool no_projects = (gSession->currentProjectNum() < 0);
    bool no_datasets = expt->numData() == 0;
    bool no_pcollections = (expt->numPeakCollections() == 0);
    bool no_unitcell = (expt->numUnitCells() == 0);
    
    actions->remove_data->setDisabled(no_datasets);
    
    actions->save_all_experiment->setDisabled(no_projects);
    actions->save_experiment->setDisabled(no_projects);
    actions->save_experiment_as->setDisabled(no_projects);

    actions->remove_experiment->setDisabled(no_projects);

    _view_menu->setDisabled(no_projects);    
    _data_menu->setDisabled(no_projects);    
    _peaks_menu->setDisabled(no_projects || no_pcollections);
    _cells_menu->setDisabled(no_projects || no_unitcell);
}
