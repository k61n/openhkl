
#ifndef NSXGUI_GUI_ACTIONS_MENUS_H
#define NSXGUI_GUI_ACTIONS_MENUS_H

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

#endif // NSXGUI_GUI_ACTIONS_MENUS_H
