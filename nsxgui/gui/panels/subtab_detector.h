
#ifndef NSXGUI_GUI_PANELS_SUBTAB_DETECTOR_H
#define NSXGUI_GUI_PANELS_SUBTAB_DETECTOR_H

#include "nsxgui/gui/properties/detectorproperty.h"
#include <QCR/widgets/views.h>

class SubtabDetector : public QcrWidget {
public:
    SubtabDetector();

private:
    DetectorProperty* property;
};

#endif // NSXGUI_GUI_PANELS_SUBTAB_DETECTOR_H
