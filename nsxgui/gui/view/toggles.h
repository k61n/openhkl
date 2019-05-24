
#ifndef NSXGUI_GUI_VIEW_TOGGLES_H
#define NSXGUI_GUI_VIEW_TOGGLES_H

//! Collection of toggle actions, for use as member of MainWin.

#include <QCR/widgets/actions.h>

class Toggles {
public:
    Toggles();

    // Options/cursor modes
    QcrToggle pixelPosition {"pixel position", "pixel position", false};
    QcrToggle gammaNu {"gamma nu", "gamma nu", false};
    QcrToggle twoTheta {"2 theta", "2 theta", false};
    QcrToggle dSpacing {"d-spacing", "d-spacing", false};
    QcrToggle millerIndices {"miller indices", "miller indices", false};
    // Options/setView
    QcrToggle fromSample {"from sample", "from sample", false};
    QcrToggle behindDetector {"behind detector", "behind detector", false};
    QcrToggle logarithmicScale {"logarithmic scale", "logarithmic scale", false};
    // Options/peak
    QcrToggle showLabels {"show labels", "show labels", false};
    QcrToggle showAreas {"show areas", "show areas", false};
    QcrToggle drawPeakArea {"draw peak area", "draw peak integration area", false};
    // View
    QcrToggle viewExperiment {"adhoc_viewExperiment", "show experiment", true};
    QcrToggle viewProperties {"adhoc_viewProperties", "show Properties", true};
    QcrToggle viewLogger {"adhoc_viewLogger", "show Logger", true};
    QcrToggle viewImage {"adhoc_viewImage", "show Image", true};
    QcrToggle viewPlotter {"adhoc_viewPlotter", "show Plot", true};
};

#endif // NSXGUI_GUI_VIEW_TOGGLES_H
