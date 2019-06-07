
#include "nsxgui/gui/panels/tab_instrument.h"

//-------------------------------------------------------------------------------------------------
//! @class TabInstrument

TabInstrument::TabInstrument() : QcrTabWidget {"instrument"}
{
    // layout
    addTab((detector = new SubtabDetector), "Detector");
    addTab((monoSource = new SubtabMonochromatic), "Monochromatic source");
    addTab((sample = new SubtabSample), "Sample");
}

void TabInstrument::setCurrent(int index)
{
    QTabWidget::setCurrentIndex(index);
}
