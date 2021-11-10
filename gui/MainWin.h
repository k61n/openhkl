//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/MainWin.h
//! @brief     Defines class MainWin
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_MAINWIN_H
#define NSX_GUI_MAINWIN_H

#include <QMainWindow>

#include <QLabel>
#include <QPixmap>
#include <QStackedWidget>
#include <QStatusBar>

class DetectorScene;
class DetectorWindow;
class LogWindow;
class PeakWindow;
class PlottableItem;
class Sentinel;
class SideBar;
class SubframeHome;
class SubframeExperiment;
class SubframeFindPeaks;
class SubframeFilterPeaks;
class SubframeAutoIndexer;
class SubframeMergedPeaks;
class SubframeRefiner;
class SubframeIntegrate;
class SubframePredictPeaks;

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
    //! change the detector image view
    void changeView(int option) const;
    //! update the plot, plot the plottable item p
    void updatePlot(PlottableItem* p) const;
    //! change the cursor tooltip on the detector scene
    void cursormode(int i) const;
    //! export current plot to ASCII
    void exportPlot() const;
    //! plot the x and y data, e is the error to y
    void plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e) const;
    //! determine if the system theme is dark
    bool isDark();

    //! Get the Sidebar
    SideBar* sideBar() { return _side_bar; };

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
    SubframeFilterPeaks* filter;
    SubframeAutoIndexer* indexer;
    SubframePredictPeaks* predictor;
    SubframeRefiner* refiner;
    SubframeIntegrate* integrator;
    SubframeMergedPeaks* merger;

    // modeless detector dialog
    DetectorWindow* detector_window;

    // modeless window to show a single peak
    PeakWindow* peak_window;

    // log window
    LogWindow* log_window;

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

#endif // NSX_GUI_MAINWIN_H
