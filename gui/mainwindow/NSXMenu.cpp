#include <algorithm>

#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>

#include "ColorMap.h"
#include "DialogAbout.h"
#include "GlobalInfo.h"
#include "MainWindow.h"
#include "NSXMenu.h"

NSXMenu::NSXMenu(MainWindow* main_window) : _main_window(main_window)
{
    createActions();
    createMenus();
}

void NSXMenu::createActions()
{
    _new_experiment_action = new QAction("&New experiment", _main_window);
    _new_experiment_action->setStatusTip("Create new experiment");
    _new_experiment_action->setShortcut(QKeySequence("Ctrl+E"));
    connect(
        _new_experiment_action, &QAction::triggered, _main_window, &MainWindow::onNewExperiment);

    _quit_action = new QAction("&Quit", _main_window);
    _quit_action->setStatusTip("Quit application");
    _quit_action->setShortcut(QKeySequence::Quit);
    connect(_quit_action, &QAction::triggered, _main_window, &MainWindow::close);

    _view_action_group = new QActionGroup(_main_window);

    _view_detector_from_sample_action = new QAction("sample", _main_window);
    _view_detector_from_sample_action->setStatusTip("View the detector looking from the sample");
    _view_detector_from_sample_action->setCheckable(true);
    _view_detector_from_sample_action->setChecked(true);
    _view_detector_from_sample_action->setActionGroup(_view_action_group);
    connect(
        _view_detector_from_sample_action, &QAction::triggered, _main_window,
        &MainWindow::onViewDetectorFromSample);

    _view_detector_from_behind_action = new QAction("behind", _main_window);
    _view_detector_from_behind_action->setStatusTip("View the detector looking to the sample");
    _view_detector_from_behind_action->setCheckable(true);
    _view_detector_from_behind_action->setChecked(false);
    _view_detector_from_behind_action->setActionGroup(_view_action_group);
    connect(
        _view_detector_from_behind_action, &QAction::triggered, _main_window,
        &MainWindow::onViewDetectorFromBehind);

    _cursor_mode_action_group = new QActionGroup(_main_window);

    _pixel_position_cursor_mode_action = new QAction("pixel position", _main_window);
    _pixel_position_cursor_mode_action->setStatusTip(
        "Show pixel position when mouse pointing the detector view");
    _pixel_position_cursor_mode_action->setCheckable(true);
    _pixel_position_cursor_mode_action->setChecked(true);
    _pixel_position_cursor_mode_action->setActionGroup(_cursor_mode_action_group);
    connect(
        _pixel_position_cursor_mode_action, &QAction::triggered, _main_window,
        &MainWindow::onSelectPixelPositionCursorMode);

    _gamma_nu_cursor_mode_action =
        new QAction(QString("%1 %2").arg(QChar(0x03B3)).arg(QChar(0x03B7)), _main_window);
    _gamma_nu_cursor_mode_action->setStatusTip(
        QString("Show %1 and %2 when mouse pointing the detector view")
            .arg(QChar(0x03B3))
            .arg(QChar(0x03B7)));
    _gamma_nu_cursor_mode_action->setCheckable(true);
    _gamma_nu_cursor_mode_action->setChecked(false);
    _gamma_nu_cursor_mode_action->setActionGroup(_cursor_mode_action_group);
    connect(
        _gamma_nu_cursor_mode_action, &QAction::triggered, _main_window,
        &MainWindow::onSelectGammaNuCursorMode);

    _two_theta_cursor_mode_action = new QAction(QString("2%1").arg(QChar(0x03B8)), _main_window);
    _two_theta_cursor_mode_action->setStatusTip(
        QString("Show 2%1 when mouse pointing the detector view").arg(QChar(0x03B8)));
    _two_theta_cursor_mode_action->setCheckable(true);
    _two_theta_cursor_mode_action->setChecked(false);
    _two_theta_cursor_mode_action->setActionGroup(_cursor_mode_action_group);
    connect(
        _two_theta_cursor_mode_action, &QAction::triggered, _main_window,
        &MainWindow::onSelect2ThetaCursorMode);

    _d_spacing_cursor_mode_action = new QAction("d spacing", _main_window);
    _d_spacing_cursor_mode_action->setStatusTip(
        "Show d-spacing when mouse pointing the detector view");
    _d_spacing_cursor_mode_action->setCheckable(true);
    _d_spacing_cursor_mode_action->setChecked(false);
    _d_spacing_cursor_mode_action->setActionGroup(_cursor_mode_action_group);
    connect(
        _d_spacing_cursor_mode_action, &QAction::triggered, _main_window,
        &MainWindow::onSelectDSpacingCursorMode);

    _miller_indices_cursor_mode_action = new QAction("miller indices", _main_window);
    _miller_indices_cursor_mode_action->setStatusTip(
        "Show miller indices when mouse pointing the detector view");
    _miller_indices_cursor_mode_action->setCheckable(true);
    _miller_indices_cursor_mode_action->setChecked(false);
    _miller_indices_cursor_mode_action->setActionGroup(_cursor_mode_action_group);
    connect(
        _miller_indices_cursor_mode_action, &QAction::triggered, _main_window,
        &MainWindow::onSelectMillerIndicesCursorMode);

    _display_peak_labels_action = new QAction("Display peak labels", _main_window);
    _display_peak_labels_action->setCheckable(true);
    _display_peak_labels_action->setChecked(false);
    connect(
        _display_peak_labels_action, &QAction::triggered, _main_window,
        &MainWindow::onDisplayPeakLabels);

    _display_peak_centers_action = new QAction("Display peak areas", _main_window);
    _display_peak_centers_action->setCheckable(true);
    _display_peak_centers_action->setChecked(false);
    connect(
        _display_peak_centers_action, &QAction::triggered, _main_window,
        &MainWindow::onDisplayPeakAreas);

    _display_peak_integration_areas_action =
        new QAction("Display peak integration areas", _main_window);
    _display_peak_integration_areas_action->setCheckable(true);
    _display_peak_integration_areas_action->setChecked(false);
    connect(
        _display_peak_integration_areas_action, &QAction::triggered, _main_window,
        &MainWindow::onDisplayPeakIntegrationAreas);

    auto color_maps = ColorMap::colorMaps();

    _color_map_action_group = new QActionGroup(_main_window);

    for (auto&& color_map : color_maps) {

        QAction* color_map_action = new QAction(color_map.c_str(), _main_window);
        color_map_action->setCheckable(true);
        color_map_action->setChecked(false);
        color_map_action->setActionGroup(_color_map_action_group);
        _color_map_actions.push_back(color_map_action);
        connect(color_map_action, &QAction::triggered, [=]() -> void {
            _main_window->onSetColorMap(color_map);
        });
    }
    auto idx = std::distance(
        _color_map_actions.begin(),
        std::find_if(_color_map_actions.begin(), _color_map_actions.end(), [](QAction* action) {
            return action->text().toStdString().compare("blue white") == 0;
        }));
    _color_map_actions[idx]->setChecked(true);

    _monitor_action = new QAction("Monitor panel", _main_window);
    _monitor_action->setStatusTip("Show/hide monitor panel");
    _monitor_action->setCheckable(true);
    _monitor_action->setChecked(true);
    connect(_monitor_action, &QAction::triggered, _main_window, &MainWindow::onToggleMonitorPanel);

    _plotter_action = new QAction("Logger panel", _main_window);
    _plotter_action->setStatusTip("Show/hide plotter panel");
    _plotter_action->setCheckable(true);
    _plotter_action->setChecked(true);
    connect(_plotter_action, &QAction::triggered, _main_window, &MainWindow::onTogglePlotterPanel);

    _widget_property_action = new QAction("Widget property panel", _main_window);
    _widget_property_action->setStatusTip("Show/hide widget property panel");
    _widget_property_action->setCheckable(true);
    _widget_property_action->setChecked(true);
    connect(
        _widget_property_action, &QAction::triggered, _main_window,
        &MainWindow::onToggleWidgetPropertyPanel);

    _about_action = new QAction("&About", _main_window);
    _about_action->setStatusTip("About application");
    _about_action->setShortcut(QKeySequence("Ctrl+A"));
    connect(_about_action, &QAction::triggered, this, &NSXMenu::onAboutApplication);
}

void NSXMenu::createMenus()
{
    _menu_bar = new QMenuBar(nullptr);

    if (hostOS() != OSType::MAC_OS) {
        _main_window->setMenuBar(_menu_bar);
    }

    _file_menu = _menu_bar->addMenu("&File");
    _file_menu->addAction(_new_experiment_action);
    _file_menu->addSeparator();
    _file_menu->addAction(_quit_action);

    _detector_menu = _menu_bar->addMenu("Detector");
    _view_from_menu = _detector_menu->addMenu("&View from");
    _view_from_menu->addAction(_view_detector_from_sample_action);
    _view_from_menu->addAction(_view_detector_from_behind_action);

    _cursor_mode_menu = _detector_menu->addMenu("Cursor mode");
    _cursor_mode_menu->addAction(_pixel_position_cursor_mode_action);
    _cursor_mode_menu->addAction(_gamma_nu_cursor_mode_action);
    _cursor_mode_menu->addAction(_two_theta_cursor_mode_action);
    _cursor_mode_menu->addAction(_d_spacing_cursor_mode_action);
    _cursor_mode_menu->addAction(_miller_indices_cursor_mode_action);

    _peaks_menu = _detector_menu->addMenu("Peaks");
    _peaks_menu->addAction(_display_peak_labels_action);
    _peaks_menu->addAction(_display_peak_centers_action);
    _peaks_menu->addAction(_display_peak_integration_areas_action);

    _detector_menu->addSeparator();

    _color_map_menu = _detector_menu->addMenu("Color maps");
    for (auto color_map_action : _color_map_actions) {
        _color_map_menu->addAction(color_map_action);
    }

    _panels_menu = _menu_bar->addMenu("&Panels");
    _panels_menu->addAction(_monitor_action);
    _panels_menu->addAction(_plotter_action);
    _panels_menu->addAction(_widget_property_action);

    _help_menu = _menu_bar->addMenu("&Help");
    _help_menu->addAction(_about_action);
}

void NSXMenu::onAboutApplication()
{
    DialogAbout dlg;

    dlg.exec();
}
