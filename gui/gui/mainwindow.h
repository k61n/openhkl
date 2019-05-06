#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDockWidget>
#include <QMainWindow>
#include "gui/panels/subframe_image.h"
#include "gui/panels/subframe_plot.h"
#include "gui/panels/subframe_experiments.h"
#include "gui/panels/subframe_logger.h"
#include "gui/panels/subframe_properties.h"

extern class MainWindow* gGui; //!< global pointer to the main window

class MainWindow : public QMainWindow {
public:
    MainWindow();

    class Triggers* triggers;
    class Toggles* toggles;
    class GuiState* state;

    void resetViews();

private:
	void refresh();

    SubframeImage* dockImage_;
    SubframePlot* dockPlot_;
    SubframeExperiments* dockExperiments_;
    SubframeProperties* dockProperties_;
    SubframeLogger* dockLogger_;
    class Menus* menus_;

    friend class Triggers;
};
#endif // MAINWINDOW_H
