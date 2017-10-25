#include <QIcon>

#include "models/PeakListItem.h"
#include "tree/PeakListPropertyWidget.h"

PeakListItem::PeakListItem(std::shared_ptr<SessionModel> session, nsx::sptrExperiment experiment) : InspectableTreeItem(experiment), _session(session)
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
    return new PeakListPropertyWidget(_session, this);
}


