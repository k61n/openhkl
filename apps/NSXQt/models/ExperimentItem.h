#ifndef EXPERIMENTITEM_H
#define EXPERIMENTITEM_H

#include <string>
#include <QJsonValue>

#include "TreeItem.h"
#include "Experiment.h"

class InstrumentItem;
class DataItem;
class PeakListItem;

class ExperimentItem : public TreeItem
{
public:
    explicit ExperimentItem(std::shared_ptr<SX::Instrument::Experiment> experiment);
//<<<<<<< HEAD
    // void setData(const QVariant & value, int role=Qt::UserRole + 1 ) override;

//    ~ExperimentItem();
//=======

    virtual ~ExperimentItem();
//>>>>>>> feature/twins

    //QJsonValue toJson();

    QJsonObject toJson() override;
    void fromJson(const QJsonObject& obj) override;

public:

    InstrumentItem* getInstrumentItem();

private:
    InstrumentItem* _instr;
    DataItem* _data;
    PeakListItem* _peaks;
};

#endif // EXPERIMENTITEM_H
