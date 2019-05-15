#ifndef MAINWIN_H
#define MAINWIN_H

#include "nsxgui/gui/panels/subframe_image.h"
#include "nsxgui/gui/panels/subframe_plot.h"
#include "nsxgui/gui/panels/subframe_experiments.h"
#include "nsxgui/gui/panels/subframe_logger.h"
#include "nsxgui/gui/panels/subframe_properties.h"
#include "nsxgui/qcr/widgets/views.h"

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
#endif // MAINWIN_H
