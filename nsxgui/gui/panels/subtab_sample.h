
#ifndef NSXGUI_GUI_PANELS_SUBTAB_SAMPLE_H
#define NSXGUI_GUI_PANELS_SUBTAB_SAMPLE_H

#include "nsxgui/gui/properties/sampleshapeproperties.h"
#include <QCR/widgets/views.h>

class SubtabSample : public QcrWidget {
public:
    SubtabSample();

private:
    SampleShapeProperties* property;
};

#endif // NSXGUI_GUI_PANELS_SUBTAB_SAMPLE_H
