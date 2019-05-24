

#ifndef NSXGUI_GUI_PANELS_TAB_PEAKS_H
#define NSXGUI_GUI_PANELS_TAB_PEAKS_H

#include "nsxgui/gui/models/peaklists.h"
#include "nsxgui/gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class ListTab : public QcrWidget {
public:
    ListTab(FilteredPeaksModel* filteredModel);

private:
    PeaksTableView* view;
};

class TabPeaks : public QcrWidget {
    Q_OBJECT
public:
    TabPeaks();
private slots:
    void slotSelectedListChanged(int i);

private:
    QcrComboBox* foundPeaksLists;
    QcrTabWidget* filtered;
};

#endif // NSXGUI_GUI_PANELS_TAB_PEAKS_H
