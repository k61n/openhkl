//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/actions/Menus.h
//! @brief     Defines class Menus
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_ACTIONS_MENUS_H
#define OHKL_GUI_ACTIONS_MENUS_H

#include <QMenuBar>

//! Submenus and menu entries of the MainWin menu bar.
class Menus {
 public:
    Menus() = delete;
    Menus(QMenuBar*);

    QMenu* _expt_menu;
    QMenu* _data_menu;
    QMenu* _peaks_menu;
    QMenu* _cells_menu;
    QMenu* _view_menu;
    QMenu* _help_menu;

    QMenu* _data_sub;
    QMenu* _exp_sub_uexport;
    QMenu* _exp_sub_mexport;

    void toggleEntries();

 private:
    QMenuBar* _menu_bar;
    QAction* separator() const;
    QMenu* actionsToMenu(const char* menuName, QList<QAction*> actions);
};

#endif // OHKL_GUI_ACTIONS_MENUS_H
