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

#ifndef GUI_MAINWIN_H
#define GUI_MAINWIN_H

#include "gui/subframe_filter/SubframeFilterPeaks.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_predict/SubframePredictPeaks.h"
#include "gui/subframe_experiment/SubframeExperiment.h"
#include "gui/subframe_home/SubframeHome.h"
#include "gui/subframe_combine/SubframeMergedPeaks.h"
#include "gui/utility/SideBar.h"
#include "gui/graphics/DetectorScene.h"

#include <QStackedWidget>

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
    void changeView(int option) { _experiment->getImage()->changeView(option); }
    //! update the plot, plot the plottable item p
    void updatePlot(PlottableItem* p) { _experiment->getPlot()->updatePlot(p); }
    //! change the cursor tooltip on the detector scene
    void cursormode(int i)
    {
        _experiment->getImage()->imageView->getScene()->changeCursorMode(i);
    }
    //! export current plot to ASCII
    void exportPlot() { _experiment->getPlot()->exportPlot(); }
    //! plot the x and y data, e is the error to y
    void plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e)
    {
        _experiment->getPlot()->plotData(x, y, e);
    }

    //! Get the Sidebar
    SideBar* sideBar() {return _side_bar;};

 private:

    void refresh();
    void readSettings();
    void saveSettings() const;
    void closeEvent(QCloseEvent* event) override;

    QStackedWidget* _layout_stack;
    SubframeExperiment* _experiment;
    SubframeHome* _home;
    PeakFinderFrame* _finder;
    SubframeFilterPeaks* _filter;
    SubframeAutoIndexer* _indexer;
    SubframePredictPeaks* _predictor;
    SubframeMergedPeaks* _merger;
    SideBar* _side_bar;

    class Menus* menus_;

    QByteArray initialState_;

    friend class Actions;
    friend class SideBar;
};

#endif // GUI_MAINWIN_H
