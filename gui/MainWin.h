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

class MainWin : public QcrMainWindow {
public:
    MainWin();
    ~MainWin();

    class Actions* triggers;
    class GuiState* state;

    void resetViews();
    void onDataChanged();
    void onExperimentChanged();
    void onPeaksChanged();
    void changeView(int option) { dockImage_->centralWidget->changeView(option); }
    void updatePlot(PlottableItem* p) { dockPlot_->updatePlot(p); }
    void cursormode(int i)
    {
        dockImage_->centralWidget->imageView->getScene()->changeCursorMode(i);
    }
    void exportPlot() { dockPlot_->exportPlot(); }

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
