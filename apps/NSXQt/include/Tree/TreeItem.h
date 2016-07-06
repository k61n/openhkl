#ifndef EDITABLETREEITEM_H
#define EDITABLETREEITEM_H

#include <string>
#include <memory>

#include <QStandardItem>
#include <QVariant>

#include "Experiment.h"

class QWidget;

using namespace SX::Instrument;

class TreeItem : public QStandardItem
{

public:
    explicit TreeItem(std::shared_ptr<Experiment> experiment);
    std::shared_ptr<Experiment> getExperiment();
    virtual ~TreeItem();


protected:
    std::shared_ptr<Experiment> _experiment;

};

#endif // EDITABLETREEITEM_H
