//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/actions/Menus.h
//! @brief     Defines class Menus
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_ACTIONS_MENUS_H
#define NSX_GUI_ACTIONS_MENUS_H

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

    void toggleEntries();

 private:
    QMenuBar* _menu_bar;
    QAction* separator() const;
    QMenu* actionsToMenu(const char* menuName, QList<QAction*> actions);
};

#endif // NSX_GUI_ACTIONS_MENUS_H
