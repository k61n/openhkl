

#ifndef TAB_INSTRUMENT_H
#define TAB_INSTRUMENT_H

#include <QTabWidget>
#include "gui/panels/subtab_detector.h"
#include "gui/panels/subtab_monochromaticsource.h"
#include "gui/panels/subtab_sample.h"

class TabInstrument : public QTabWidget {
public:
    TabInstrument();
    SubtabDetector* detector;
    SubtabMonochromatic* monoSource;
    SubtabSample* sample;
};

#endif // TAB_INSTRUMENT_H
