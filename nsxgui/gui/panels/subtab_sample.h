
#ifndef SUBTAB_SAMPLE_H
#define SUBTAB_SAMPLE_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/gui/properties/sampleshapeproperties.h"

class SubtabSample : public QcrWidget {
public:
    SubtabSample();
private:
    SampleShapeProperties* property;
};

#endif // SUBTAB_SAMPLE_H
