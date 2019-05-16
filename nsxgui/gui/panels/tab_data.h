

#ifndef TAB_DATA_H
#define TAB_DATA_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/gui/properties/numorproperty.h"

class TabData : public QcrWidget {
public:
    TabData();
private:
    NumorProperty* property;
};

#endif // TAB_DATA_H
