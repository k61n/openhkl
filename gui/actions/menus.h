//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/menus.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_ACTIONS_MENUS_H
#define GUI_ACTIONS_MENUS_H

#include <QMenuBar>

//! Submenus and menu entries of the MainWin menu bar.

class Menus {
public:
    Menus() = delete;
    Menus(QMenuBar*);
    QMenu* file_;
    QMenu* export_;
    QMenu* options_;
    QMenu* experiment_;
    QMenu* help_;
    QMenu* view_;

private:
    QMenuBar* mbar_;
    QAction* separator() const;
    QMenu* actionsToMenu(const char* menuName, QList<QAction*> actions);
};

#endif // GUI_ACTIONS_MENUS_H
