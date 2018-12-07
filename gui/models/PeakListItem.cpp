#include <QIcon>

#include "PeakListItem.h"
#include "PeakListPropertyWidget.h"

PeakListItem::PeakListItem(const nsx::PeakList& peaks)
: InspectableTreeItem(),
  _peaks(peaks)
{
	setText("Peaks");

    setEditable(true);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);

    setCheckable(true);
}

QWidget* PeakListItem::inspectItem()
{
    return new PeakListPropertyWidget(this);
}

void PeakListItem::removePeak(nsx::sptrPeak3D peak)
{
    auto it = std::find(_peaks.begin(), _peaks.end(), peak);
    if (it != _peaks.end()) {
        _peaks.erase(it);
    }
}

nsx::PeakList& PeakListItem::peaks()
{
    return _peaks;
}
