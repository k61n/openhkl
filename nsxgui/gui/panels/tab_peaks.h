

#ifndef TAB_PEAKS_H
#define TAB_PEAKS_H

#include <QCR/widgets/views.h>
#include <QCR/widgets/controls.h>
#include "nsxgui/gui/models/peakstable.h"
#include "nsxgui/gui/models/peaklists.h"

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

#endif // TAB_PEAKS_H
