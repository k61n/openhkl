#pragma once

#include <vector>

#include <QObject>

class QAction;
class QActionGroup;
class QMenu;
class QMenuBar;
class MainWindow;

class NSXMenu : public QObject {

public:
    NSXMenu(MainWindow* main_window);

private slots:
    void onAboutApplication();

private:
    void createActions();
    void createMenus();

private:
    MainWindow* _main_window;

    QMenuBar* _menu_bar;

    QMenu* _file_menu;
    QAction* _new_experiment_action;
    QAction* _quit_action;

    QMenu* _detector_menu;
    QMenu* _view_from_menu;
    QActionGroup* _view_action_group;
    QAction* _view_detector_from_sample_action;
    QAction* _view_detector_from_behind_action;

    QMenu* _cursor_mode_menu;
    QActionGroup* _cursor_mode_action_group;
    QAction* _pixel_position_cursor_mode_action;
    QAction* _gamma_nu_cursor_mode_action;
    QAction* _two_theta_cursor_mode_action;
    QAction* _d_spacing_cursor_mode_action;
    QAction* _miller_indices_cursor_mode_action;

    QMenu* _peaks_menu;
    QAction* _display_peak_labels_action;
    QAction* _display_peak_centers_action;
    QAction* _display_peak_integration_areas_action;

    QMenu* _color_map_menu;
    QActionGroup* _color_map_action_group;
    std::vector<QAction*> _color_map_actions;

    QMenu* _panels_menu;
    QAction* _monitor_action;
    QAction* _plotter_action;
    QAction* _widget_property_action;

    QMenu* _help_menu;
    QAction* _about_action;
};
