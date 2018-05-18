#ifndef NSXQT_TREEITEM_H
#define NSXQT_TREEITEM_H

#include <memory>
#include <string>

#include <QStandardItem>
#include <QVariant>
#include <QJsonObject>

#include <nsxlib/Experiment.h>
#include <nsxlib/InstrumentTypes.h>

class QWidget;

class TreeItem : public QStandardItem
{

public:

    explicit TreeItem();
    //nsx::sptrExperiment getExperiment();
    virtual ~TreeItem();

    virtual void setData(const QVariant & value, int role=Qt::UserRole + 1) override;

    virtual QJsonObject toJson();
    virtual void fromJson(const QJsonObject& obj);

    nsx::sptrExperiment experiment();

};

#endif // NSXQT_TREEITEM_H
