//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/mainwin.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSXGUI_GUI_MAINWIN_H
#define NSXGUI_GUI_MAINWIN_H

#include "nsxgui/gui/panels/subframe_experiments.h"
#include "nsxgui/gui/panels/subframe_image.h"
#include "nsxgui/gui/panels/subframe_logger.h"
#include "nsxgui/gui/panels/subframe_plot.h"
#include "nsxgui/gui/panels/subframe_properties.h"
#include <QCR/widgets/views.h>

extern class MainWin* gGui; //!< global pointer to the main window

class MainWin : public QcrMainWindow {
public:
    MainWin();
    ~MainWin();

    class Triggers* triggers;
    class Toggles* toggles;
    class GuiState* state;

    void resetViews();

private:
    void refresh();
    void readSettings();
    void saveSettings() const;

    SubframeImage* dockImage_;
    SubframePlot* dockPlot_;
    SubframeExperiments* dockExperiments_;
    SubframeProperties* dockProperties_;
    SubframeLogger* dockLogger_;
    class Menus* menus_;

    QByteArray initialState_;

    friend class Triggers;
    friend class Toggles;
};
#endif // NSXGUI_GUI_MAINWIN_H
