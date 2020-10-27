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
#include <QStackedWidget>

class DetectorScene;
class PlottableItem;
class SideBar;
class SubframeHome;
class SubframeExperiment;
class SubframeFindPeaks;
class SubframeFilterPeaks;
class SubframeAutoIndexer;
class SubframeMergedPeaks;
class SubframeRefiner;
class SubframePredictPeaks;

extern class MainWin* gGui; //!< global pointer to the main window

//! Main window of the application
class MainWin : public QMainWindow {
 public:
    MainWin();
    ~MainWin();

    class Actions* triggers;
    class GuiState* state;

    //! reset window geometry to initial state
    void resetViews();
    //! Refreshes the parts of the main window that depend on the data
    void onDataChanged();
    //! Refreshes the parts of the main window that depend on the experiment
    void onExperimentChanged();
    //! Refreshes the parts of the main window that depend on the peaks
    void onPeaksChanged();
    //! Refreshes the parts of the main window that depend on the peaks
    void onUnitCellChanged();
    //! change the detector image view
    void changeView(int option);
    //! update the plot, plot the plottable item p
    void updatePlot(PlottableItem* p);
    //! change the cursor tooltip on the detector scene
    void cursormode(int i);
    //! export current plot to ASCII
    void exportPlot();
    //! plot the x and y data, e is the error to y
    void plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e);

    //! Get the Sidebar
    SideBar* sideBar() { return _side_bar; };

 private:
    void readSettings();
    void saveSettings() const;
    void closeEvent(QCloseEvent* event) override;

 public:
    SubframeExperiment* experiment;
    SubframeHome* home;
    SubframeFindPeaks* finder;
    SubframeFilterPeaks* filter;
    SubframeAutoIndexer* indexer;
    SubframePredictPeaks* predictor;
    SubframeRefiner* refiner;
    SubframeMergedPeaks* merger;

 private:
    QStackedWidget* _layout_stack;
    SideBar* _side_bar;
    class Menus* menus_;
    QByteArray initialState_;
    friend class Actions;
    friend class SideBar;
};

#endif // NSX_GUI_MAINWIN_H
