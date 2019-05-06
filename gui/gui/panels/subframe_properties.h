
#ifndef SUBFRAME_PORPERTIES_H
#define SUBFRAME_PROPERTIES_H

#include <QDockWidget>
#include "gui/panels/subframe_setup.h"

class SubframeProperties : public QDockWidget {
public:
	SubframeProperties();
    SubframeSetup* tabsframe;
};

#endif // SUBFRAME_PROPERTIES_H
