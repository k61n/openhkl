#pragma once

#include <QStandardItem>
#include <QVariant>

#include <nsxlib/Experiment.h>
#include <nsxlib/InstrumentTypes.h>

#include "SessionModel.h"

class QWidget;
class ExperimentItem;

class TreeItem: public QStandardItem
{
public:
    explicit TreeItem();

    virtual ~TreeItem();

    virtual void setData(const QVariant& value, int role=Qt::UserRole + 1) override;

    virtual SessionModel* model() const;

    nsx::sptrExperiment experiment();

    ExperimentItem* experimentItem() const;
};
