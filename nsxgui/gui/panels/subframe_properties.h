
#ifndef SUBFRAME_PORPERTIES_H
#define SUBFRAME_PROPERTIES_H

#pragma once

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/gui/panels/subframe_setup.h"

class SubframeProperties : public QcrDockWidget {
public:
	SubframeProperties();
    SubframeSetup* tabsframe;
};

#endif // SUBFRAME_PROPERTIES_H
