#include <QIcon>

#include "models/PeakListItem.h"
#include "Tree/PeakListPropertyWidget.h"

PeakListItem::PeakListItem(std::shared_ptr<Experiment> experiment) : InspectableTreeItem(experiment)
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


