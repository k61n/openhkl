

#ifndef TAB_DATA_H
#define TAB_DATA_H

#include "nsxgui/gui/properties/numorproperty.h"
#include <QCR/widgets/views.h>

class TabData : public QcrWidget {
public:
    TabData();

private:
    NumorProperty* property;
};

#endif // TAB_DATA_H
