
#ifndef NSXGUI_GUI_PANELS_SUBFRAME_PROPERTIES_H
#define NSXGUI_GUI_PANELS_SUBFRAME_PROPERTIES_H

#pragma once

#include "nsxgui/gui/panels/subframe_setup.h"
#include <QCR/widgets/views.h>

class SubframeProperties : public QcrDockWidget {
public:
    SubframeProperties();
    SubframeSetup* tabsframe;
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_PROPERTIES_H
