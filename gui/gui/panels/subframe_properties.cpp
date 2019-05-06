
#include "gui/panels/subframe_properties.h"
#include "gui/panels/subframe_setup.h"

SubframeProperties::SubframeProperties()
    : QDockWidget{"Properties"}
{
        setWidget((tabsframe = new SubframeSetup));
}
