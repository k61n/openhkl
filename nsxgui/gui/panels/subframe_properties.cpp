
#include "nsxgui/gui/panels/subframe_properties.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/panels/subframe_setup.h"
#include "nsxgui/gui/view/toggles.h"

SubframeProperties::SubframeProperties() : QcrDockWidget {"Properties"}
{
    setWidget((tabsframe = new SubframeSetup));
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewProperties,
        SLOT(setChecked(bool)));
}
