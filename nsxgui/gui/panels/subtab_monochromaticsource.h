
#ifndef SUBTAB_MONOCHROMATICSOURCE_H
#define SUBTAB_MONOCHROMATICSOURCE_H

#include "nsxgui/gui/properties/sourceproperty.h"
#include <QCR/widgets/views.h>

class SubtabMonochromatic : public QcrWidget {
public:
    SubtabMonochromatic();

private:
    SourceProperty* property;
};

#endif // SUBTAB_MONOCHROMATICSOURCE_H
