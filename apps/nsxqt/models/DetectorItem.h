#ifndef NSXQT_DETECTORITEM_H
#define NSXQT_DETECTORITEM_H

#include <memory>
#include <string>

#include <QString>

#include <nsxlib/InstrumentTypes.h>

#include "InspectableTreeItem.h"

class QWidget;

class DetectorItem : public InspectableTreeItem
{
public:

    explicit DetectorItem();
    QWidget* inspectItem() override;
};

#endif // NSXQT_DETECTORITEM_H
