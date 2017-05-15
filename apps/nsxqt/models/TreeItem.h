#ifndef EDITABLETREEITEM_H
#define EDITABLETREEITEM_H

#include <string>
#include <memory>

#include <QStandardItem>
#include <QVariant>
#include <QJsonObject>

#include <nsxlib/instrument/Experiment.h>

class QWidget;

using namespace nsx::Instrument;

class TreeItem : public QStandardItem
{

public:
    explicit TreeItem(std::shared_ptr<Experiment> experiment);
    std::shared_ptr<Experiment> getExperiment();
    virtual ~TreeItem();

    virtual void setData(const QVariant & value, int role=Qt::UserRole + 1) override;

    virtual QJsonObject toJson();
    virtual void fromJson(const QJsonObject& obj);

protected:
    std::shared_ptr<Experiment> _experiment;

};

#endif // EDITABLETREEITEM_H
