
#ifndef NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H
#define NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H

#include <QCR/widgets/views.h>

//! Part of the main window that controls the data and correction files.
class SubframeExperiments : public QcrDockWidget {
public:
    SubframeExperiments();
    void addExperiment(const QString&, const QString&);
    void removeExperiment();
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H
