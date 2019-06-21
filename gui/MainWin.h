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

#include "gui/graphics/DetectorScene.h"
#include "gui/panels/SubframeExperiments.h"
#include "gui/panels/SubframeImage.h"
#include "gui/panels/SubframeLogger.h"
#include "gui/panels/SubframePlot.h"
#include "gui/panels/SubframeProperties.h"

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
    void onDataChanged();
    void onExperimentChanged();
    void onPeaksChanged();
    //! change the detector image view
    void changeView(int option) { dockImage_->centralWidget->changeView(option); }
    //! update the plot, plot the plottable item p
    void updatePlot(PlottableItem* p) { dockPlot_->updatePlot(p); }
    //! change the cursor tooltip on the detector scene
    void cursormode(int i)
    {
        dockImage_->centralWidget->imageView->getScene()->changeCursorMode(i);
    }
    //! export current plot to ASCII
    void exportPlot() { dockPlot_->exportPlot(); }
    //! plot the x and y data, e is the error to y
    void plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e)
    {   dockPlot_->plotData(x, y, e);   }

private:
    void refresh();
    void readSettings();
    void saveSettings() const;
    void closeEvent(QCloseEvent *event) override;

    SubframeImage* dockImage_;
    SubframePlot* dockPlot_;
    SubframeExperiments* dockExperiments_;
    SubframeProperties* dockProperties_;
    SubframeLogger* dockLogger_;
    class Menus* menus_;

    QByteArray initialState_;

    friend class Actions;
};
#endif // GUI_MAINWIN_H
