
#ifndef SUBTAB_MONOCHROMATICSOURCE_H
#define SUBTAB_MONOCHROMATICSOURCE_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/gui/properties/sourceproperty.h"

class SubtabMonochromatic : public QcrWidget {
public:
    SubtabMonochromatic();
private:
    SourceProperty* property;
};

#endif // SUBTAB_MONOCHROMATICSOURCE_H
