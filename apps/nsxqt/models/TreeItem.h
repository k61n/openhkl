#ifndef NSXQT_TREEITEM_H
#define NSXQT_TREEITEM_H

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
    //nsx::sptrExperiment getExperiment();
    virtual ~TreeItem();

    virtual void setData(const QVariant & value, int role=Qt::UserRole + 1) override;

    nsx::sptrExperiment experiment();

    ExperimentItem& experimentItem();
};

#endif // NSXQT_TREEITEM_H
