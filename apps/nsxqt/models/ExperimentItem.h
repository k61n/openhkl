#ifndef NSXQT_EXPERIMENTITEM_H
#define NSXQT_EXPERIMENTITEM_H

#include <string>

#include <QJsonObject>

#include <nsxlib/instrument/InstrumentTypes.h>

#include "TreeItem.h"

class DataItem;
class InstrumentItem;
class PeakListItem;

class ExperimentItem : public TreeItem
{
public:
    explicit ExperimentItem(nsx::sptrExperiment experiment);
    virtual ~ExperimentItem() = default;
    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
    InstrumentItem* getInstrumentItem();

private:
    InstrumentItem* _instr;
    DataItem* _data;
    PeakListItem* _peaks;
};

#endif // NSXQT_EXPERIMENTITEM_H
