//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/actions/Menus.cpp
//! @brief     Implements class Menus
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/actions/Menus.h"

#include "core/experiment/Experiment.h"
#include "gui/MainWin.h"
#include "gui/actions/Actions.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/SideBar.h"

#include <QAction>
#include <QMenu>

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
    _expt_menu->addAction(actions->new_strategy);
    _expt_menu->addAction(actions->load_experiment);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->save_experiment);
    _expt_menu->addAction(actions->save_experiment_as);
    _expt_menu->addAction(actions->save_all_experiment);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->export_peaks);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->quit);

    // View menu
    _view_menu->addAction(actions->detector_window);
    _view_menu->addAction(actions->instrumentstate_window);
    _view_menu->addAction(actions->log_window);
    _view_menu->addSeparator();
    _view_menu->addAction(actions->close_peak_windows);

    QMenu* _data_sub = _data_menu->addMenu("Add data set");
    _data_sub->addAction(actions->add_raw);
    _data_sub->addAction(actions->add_tiff);
    _data_sub->addAction(actions->add_hdf5);
    _data_sub->addAction(actions->add_nexus);
    _data_menu->addAction(actions->add_single_raw);
    _data_menu->addAction(actions->add_single_tiff);
    _data_menu->addAction(actions->show_input_files);
    _data_menu->addAction(actions->remove_data);

    _peaks_menu->addAction(actions->show_peaks);
    _peaks_menu->addAction(actions->clone_peaks);
    _peaks_menu->addAction(actions->remove_peaks);

    _cells_menu->addAction(actions->add_cell);
    _cells_menu->addAction(actions->remove_cell);

    _help_menu->addAction(actions->documentation);
    _help_menu->addAction(actions->about);

    // adding shortcuts
    actions->new_experiment->setShortcuts(QKeySequence::New);
    actions->load_experiment->setShortcuts(QKeySequence::Open);
    actions->save_experiment->setShortcuts(QKeySequence::Save);
    actions->save_experiment_as->setShortcuts(QKeySequence::SaveAs);
    // actions->save_all_experiment->setShortcuts(QKeySequence::SaveAll)
    actions->remove_experiment->setShortcuts(QKeySequence::Delete);
    actions->quit->setShortcuts(QKeySequence::Quit);

    actions->detector_window->setShortcut(QKeySequence("Ctrl+D"));
    actions->instrumentstate_window->setShortcut(QKeySequence("Ctrl+I"));
    actions->log_window->setShortcut(QKeySequence("Ctrl+L"));
    actions->close_peak_windows->setShortcut(QKeySequence("Ctrl+C"));

    // apparently this doesnt work for three keys ???
    actions->clone_peaks->setShortcut(QKeySequence("Ctrl+P+C"));
    actions->remove_peaks->setShortcut(QKeySequence("Ctrl+P+X"));

    actions->add_cell->setShortcut(QKeySequence("Ctrl+U+C"));
    actions->remove_cell->setShortcut(QKeySequence("Ctrl+U+X"));

    actions->add_raw->setShortcut(QKeySequence("Ctrl+R"));
    actions->add_tiff->setShortcut(QKeySequence("Ctrl+T"));
    actions->add_hdf5->setShortcut(QKeySequence("Ctrl+H"));
    actions->add_nexus->setShortcut(QKeySequence("Ctrl+X"));

    actions->documentation->setShortcut(QKeySequence("F1"));

    toggleEntries();
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
void Menus::toggleEntries()
{
    // return;
    Actions* actions = gGui->triggers;

    if (gSession->numExperiments() == 0) { // just disables everything
        actions->save_all_experiment->setDisabled(true);
        actions->save_experiment->setDisabled(true);
        actions->save_experiment_as->setDisabled(true);
        actions->export_peaks->setDisabled(true);
        actions->remove_experiment->setDisabled(true);

        _view_menu->setDisabled(true);
        _data_menu->setDisabled(true);
        _peaks_menu->setDisabled(true);
        _cells_menu->setDisabled(true);
        return;
    }

    bool no_projects = !gSession->hasProject();
    bool no_datasets = !gSession->currentProject()->hasDataSet();
    bool no_pcollections = !gSession->currentProject()->hasPeakCollection();
    bool no_unitcell = !gSession->currentProject()->hasUnitCell();
    bool strategy_mode = gSession->currentProject()->strategyMode();

    actions->add_raw->setDisabled(strategy_mode);
    actions->add_tiff->setDisabled(strategy_mode);
    actions->add_nexus->setDisabled(strategy_mode);
    actions->add_hdf5->setDisabled(strategy_mode);
    actions->add_single_raw->setDisabled(!strategy_mode);
    actions->add_single_tiff->setDisabled(!strategy_mode);

    actions->remove_data->setDisabled(no_datasets);

    actions->save_all_experiment->setDisabled(true); // not implemented yet
    actions->save_experiment->setDisabled(no_projects);
    actions->save_experiment_as->setDisabled(no_projects);
    actions->export_peaks->setDisabled(no_pcollections);
    actions->remove_experiment->setDisabled(no_projects);

    actions->show_input_files->setDisabled(no_datasets);
    actions->show_peaks->setDisabled(no_pcollections);
    _view_menu->setDisabled(no_projects);
    _data_menu->setDisabled(no_projects);
    _peaks_menu->setDisabled(no_pcollections);
    _cells_menu->setDisabled(no_unitcell);
}
