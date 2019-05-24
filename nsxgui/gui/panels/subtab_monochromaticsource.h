
#ifndef NSXGUI_GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H
#define NSXGUI_GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H

#include "nsxgui/gui/properties/sourceproperty.h"
#include <QCR/widgets/views.h>

class SubtabMonochromatic : public QcrWidget {
public:
    SubtabMonochromatic();

private:
    SourceProperty* property;
};

#endif // NSXGUI_GUI_PANELS_SUBTAB_MONOCHROMATICSOURCE_H
