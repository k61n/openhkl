

#ifndef TAB_INSTRUMENT_H
#define TAB_INSTRUMENT_H

#include "nsxgui/gui/panels/subtab_detector.h"
#include "nsxgui/gui/panels/subtab_monochromaticsource.h"
#include "nsxgui/gui/panels/subtab_sample.h"
#include <QCR/widgets/controls.h>

class TabInstrument : public QcrTabWidget {
public:
    TabInstrument();
    SubtabDetector* detector;
    SubtabMonochromatic* monoSource;
    SubtabSample* sample;

    void setCurrent(int);
};

#endif // TAB_INSTRUMENT_H
