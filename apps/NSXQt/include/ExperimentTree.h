#ifndef EXPERIMENTTREE_H
#define EXPERIMENTTREE_H

#include <string>
#include <vector>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include "Experiment.h"

using namespace SX::Instrument;

namespace SX
{
namespace Data
{
    class IData;
}
}

using namespace SX::Data;

class ExperimentTree : public QTreeView
{
    Q_OBJECT
public:
    explicit ExperimentTree(QWidget *parent = 0);

    void addExperiment(const std::string& experimentName, const std::string& instrumentName);
    std::vector<IData*> getSelectedNumors() const;

signals:
    void plotData(IData* data);

public slots:
    void keyPressEvent(QKeyEvent* event);
    void onCustomMenuRequested(const QPoint& point);
    void onDoubleClick(const QModelIndex& index);
    void importData();

//    void treat(QModelIndex, QModelIndex);

private:

    QStandardItemModel* _model;

};

#endif // EXPERIMENTTREE_H
