
#ifndef SUBTAB_DETECTOR_H
#define SUBTAB_DETECTOR_H

#include "nsxgui/gui/properties/detectorproperty.h"
#include <QCR/widgets/views.h>

class SubtabDetector : public QcrWidget {
public:
    SubtabDetector();

private:
    DetectorProperty* property;
};

#endif // SUBTAB_DETECTOR_H
