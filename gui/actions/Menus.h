//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

    QMenu* _file_menu;
    QMenu* _edit_menu;
    QMenu* _data_menu;
    QMenu* _peaks_menu;
    QMenu* _view_menu;
    QMenu* _help_menu;

 private:
    QMenuBar* _menu_bar;
    QAction* separator() const;
    QMenu* actionsToMenu(const char* menuName, QList<QAction*> actions);
};

#endif // NSX_GUI_ACTIONS_MENUS_H
