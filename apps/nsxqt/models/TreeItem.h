#pragma once

#include <memory>
#include <string>

#include <QMenu>
#include <QStandardItem>
#include <QVariant>
#include <QJsonObject>

#include <nsxlib/Experiment.h>
#include <nsxlib/InstrumentTypes.h>


class QWidget;
class ExperimentItem;

class TreeItem: public QStandardItem
{
public:
    explicit TreeItem();

    virtual ~TreeItem();

    virtual void setData(const QVariant & value, int role=Qt::UserRole + 1) override;

    virtual QJsonObject toJson();
    virtual void fromJson(const QJsonObject& obj);

    nsx::sptrExperiment experiment();

    ExperimentItem& experimentItem();
};
