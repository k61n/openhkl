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
#include "gui/frames/AutoIndexerFrame.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/subframe_experiment/SubframeExperiment.h"
#include "gui/subframe_home/SubframeHome.h"
#include <QCR/widgets/mainwindow.h>
#include <QStackedWidget>

extern class MainWin* gGui; //!< global pointer to the main window

//! Main window of the application
class MainWin : public QcrMainWindow {
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
    //! change the detector image view
    void changeView(int option) { experimentScreen_->image->changeView(option); }
    //! update the plot, plot the plottable item p
    void updatePlot(PlottableItem* p) { experimentScreen_->plot->updatePlot(p); }
    //! change the cursor tooltip on the detector scene
    void cursormode(int i)
    {
        experimentScreen_->image->imageView->getScene()->changeCursorMode(i);
    }
    //! export current plot to ASCII
    void exportPlot() { experimentScreen_->plot->exportPlot(); }
    //! plot the x and y data, e is the error to y
    void plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e)
    {
        experimentScreen_->plot->plotData(x, y, e);
    }

 private:
    void refresh();
    void readSettings();
    void saveSettings() const;
    void closeEvent(QCloseEvent* event) override;

    QStackedWidget* layoutStack_;
    SubframeExperiment* experimentScreen_;
    SubframeHome* homeScreen_;
    PeakFinderFrame* finder_;
    PeakFilterDialog* filter_;
    AutoIndexerFrame* indexer_;
    class Menus* menus_;

    QByteArray initialState_;

    friend class Actions;
    friend class SideBar;
};
#endif // GUI_MAINWIN_H
