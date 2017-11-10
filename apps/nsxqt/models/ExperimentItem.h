#ifndef NSXQT_EXPERIMENTITEM_H
#define NSXQT_EXPERIMENTITEM_H

#include <string>
#include <memory>

#include <QJsonObject>

#include <nsxlib/InstrumentTypes.h>

#include "TreeItem.h"
#include "models/SessionModel.h"

class DataItem;
class InstrumentItem;
class PeakListItem;

class ExperimentItem : public TreeItem
{
public:
    explicit ExperimentItem(std::shared_ptr<SessionModel> session, nsx::sptrExperiment experiment);
    virtual ~ExperimentItem() = default;
    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;
    InstrumentItem* getInstrumentItem();

private:
    InstrumentItem* _instr;
    DataItem* _data;
    PeakListItem* _peaks;
    std::shared_ptr<SessionModel> _session;
};

#endif // NSXQT_EXPERIMENTITEM_H
