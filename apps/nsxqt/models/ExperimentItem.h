#ifndef EXPERIMENTITEM_H
#define EXPERIMENTITEM_H

#include <string>
#include <QJsonValue>

#include "TreeItem.h"
#include <nsxlib/instrument/Experiment.h>

class InstrumentItem;
class DataItem;
class PeakListItem;

class ExperimentItem : public TreeItem
{
public:
    explicit ExperimentItem(std::shared_ptr<nsx::Instrument::Experiment> experiment);
    virtual ~ExperimentItem() = default;
    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
    InstrumentItem* getInstrumentItem();

private:
    InstrumentItem* _instr;
    DataItem* _data;
    PeakListItem* _peaks;
};

#endif // EXPERIMENTITEM_H
