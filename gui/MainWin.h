//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/MainWin.h
//! @brief     Defines class MainWin
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_MAINWIN_H
#define OHKL_GUI_MAINWIN_H

#include <QMainWindow>

#include <QLabel>
#include <QPixmap>
#include <QSettings>
#include <QStackedWidget>
#include <QStatusBar>

class DetectorWindow;
class LogWindow;
class PeakWindow;
class InputFilesWindow;
class InstrumentStateWindow;
class PeaklistWindow;
class PlottableItem;
class Sentinel;
class SideBar;
class SubframeHome;
class SubframeExperiment;
class SubframeFindPeaks;
class SubframeFilter;
class SubframeAutoIndexer;
class SubframeMerge;
class SubframeRefiner;
class SubframeIntegrate;
class SubframePredict;
class SubframeShapes;
class SubframeReject;
class SubframeRescale;

extern class MainWin* gGui; //!< global pointer to the main window

//! Main window of the application
class MainWin : public QMainWindow {
 public:
    MainWin();

    class Actions* triggers;
    class GuiState* state;

    //! reset window geometry to initial state
    void resetViews();
    //! Refreshes the parts of the main window that depend on the data
    void onDataChanged() const;
    //! Refreshes the parts of the main window that depend on the experiment
    void onExperimentChanged() const;
    //! Refreshes the parts of the main window that depend on the peaks
    void onPeaksChanged() const;
    //! Refreshes the parts of the main window that depend on the peaks
    void onUnitCellChanged() const;
    //! update the plot, plot the plottable item p
    void updatePlot(PlottableItem* p) const;
    //! export current plot to ASCII
    void exportPlot() const;
    //! plot the x and y data, e is the error to y
    void plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e) const;
    //! determine if the system theme is dark
    bool isDark();

    void refreshMenu();

    //! Get the Sidebar
    SideBar* sideBar() { return _side_bar; };
    //! Get a QSettings object
    QSettings qSettings() const;

 public slots:
    void setReady(bool ready);

 private:
    void readSettings();
    void saveSettings() const;
    void closeEvent(QCloseEvent* event) override;

 public:
    // main window widgets
    SubframeExperiment* experiment;
    SubframeHome* home;
    SubframeFindPeaks* finder;
    SubframeFilter* filter;
    SubframeAutoIndexer* indexer;
    SubframeShapes* shapes;
    SubframePredict* predictor;
    SubframeRefiner* refiner;
    SubframeIntegrate* integrator;
    SubframeReject* rejector;
    SubframeMerge* merger;
    SubframeRescale* rescaler;

    // modeless detector dialog
    DetectorWindow* detector_window;

    // modeless dialog to show instrument states
    InstrumentStateWindow* instrumentstate_window;

    // modeless window to show a single peak
    QVector<PeakWindow*> peak_windows;

    // log window
    LogWindow* log_window;

    InputFilesWindow* input_files_window;

    PeaklistWindow* peak_list_window;

    Sentinel* sentinel;


 private:
    void initStatusBar();

    QPixmap _red_circle;
    QPixmap _green_circle;
    QLabel* _status;
    QLabel* _light;
    QStackedWidget* _layout_stack;
    SideBar* _side_bar;
    class Menus* menus_;
    QByteArray initialState_;
    friend class Actions;
    friend class SideBar;
};

#endif // OHKL_GUI_MAINWIN_H
