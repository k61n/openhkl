#include <QIcon>

#include "PeakListItem.h"
#include "PeakListPropertyWidget.h"

PeakListItem::PeakListItem(nsx::sptrExperiment experiment) : InspectableTreeItem(experiment)
{

	setText("Peaks");

    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);
}

QWidget* PeakListItem::inspectItem()
{
    return new PeakListPropertyWidget(this);
}


