
#include "gui/panels/tab_instrument.h"

//-------------------------------------------------------------------------------------------------
//! @class TabInstrument

TabInstrument::TabInstrument()
{
    //layout
    setTabPosition(QTabWidget::North);
    addTab((detector = new SubtabDetector), "Detector");
    addTab((monoSource = new SubtabMonochromatic), "Monochromatic source");
    addTab((sample = new SubtabSample), "Sample");

    for (int i=0; i<3; i++){
        setTabEnabled(i, true);
    }
    show();
}
