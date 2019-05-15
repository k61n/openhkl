
#ifndef SUBFRAME_EXPERIMENTS_H
#define SUBFRAME_EXPERIMENTS_H

#include "nsxgui/qcr/widgets/views.h"

//! Part of the main window that controls the data and correction files.
class SubframeExperiments : public QcrDockWidget {
public:
    SubframeExperiments();
    void addExperiment(const QString&, const QString&);
    void removeExperiment();
};

#endif // SUBFRAME_EXPERIMENTS_H
