#ifndef EXPERIMENTITEM_H
#define EXPERIMENTITEM_H

#include <string>
#include <QJsonValue>

#include "TreeItem.h"
#include "Experiment.h"

//using namespace SX::Instrument;

class ExperimentItem : public TreeItem
{
public:
    explicit ExperimentItem(std::shared_ptr<SX::Instrument::Experiment> experiment);

    void setData(const QVariant & value, int role=Qt::UserRole + 1 );

    ~ExperimentItem();

    //QJsonValue toJson();

    QJsonObject toJson() override;
    void fromJson(QJsonObject& obj) override;

signals:

};

#endif // EXPERIMENTITEM_H
