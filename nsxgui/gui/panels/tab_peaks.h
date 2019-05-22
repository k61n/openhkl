

#ifndef TAB_PEAKS_H
#define TAB_PEAKS_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/gui/models/peakstable.h"

class TabPeaks : public QcrWidget {
public:
    TabPeaks();
private:
    PeaksTableView* view;
};

#endif // TAB_PEAKS_H
