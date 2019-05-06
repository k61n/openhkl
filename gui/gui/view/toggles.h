
#ifndef TOGGLES_H
#define TOGGLES_H

//! Collection of toggle actions, for use as member of MainWindow.

#include <QAction>

class Toggles {
public:
    Toggles();

//Options/cursor modes
    QAction pixelPosition {"pixel position"};
    QAction gammaNu {"gamma nu"};
    QAction twoTheta {"2 theta"};
    QAction dSpacing {"d-spacing"};
    QAction millerIndices {"miller indices"};
//Options/setView
    QAction fromSample {"from sample"};
    QAction behindDetector {"behind detector"};
    QAction logarithmicScale {"logarithmic scale"};
//Options/peak
    QAction showLabels {"show labels"};
    QAction showAreas {"show areas"};
    QAction drawPeakArea {"draw peak integration area"};
};

#endif // TOGGLES_H
