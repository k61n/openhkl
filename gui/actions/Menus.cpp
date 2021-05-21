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

#include <QAction>
#include <QMenu>

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
    _expt_menu->addAction(actions->save_all_experiment);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->remove_experiment);
    _expt_menu->addAction(actions->quit);

    _view_menu->addAction(actions->detector_window);

    QMenu* _data_sub = _data_menu->addMenu("Add data set");
    _data_sub->addAction(actions->add_raw);
    _data_sub->addAction(actions->add_hdf5);
    _data_sub->addAction(actions->add_nexus);
    _data_menu->addAction(actions->remove_data);

    _peaks_menu->addAction(actions->remove_peaks);

    _cells_menu->addAction(actions->add_cell);
    _cells_menu->addAction(actions->remove_cell);

    _help_menu->addAction(actions->about);
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
