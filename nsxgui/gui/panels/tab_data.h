

#ifndef NSXGUI_GUI_PANELS_TAB_DATA_H
#define NSXGUI_GUI_PANELS_TAB_DATA_H

#include "nsxgui/gui/properties/numorproperty.h"
#include <QCR/widgets/views.h>

class TabData : public QcrWidget {
public:
    TabData();

private:
    NumorProperty* property;
};

#endif // NSXGUI_GUI_PANELS_TAB_DATA_H
