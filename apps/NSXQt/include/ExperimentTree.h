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

class ExperimentTree : public QTreeView
{
    Q_OBJECT
public:
    explicit ExperimentTree(QWidget *parent = 0);

    void addExperiment(const std::string& experimentName, const std::string& instrumentName);
    std::vector<SX::Data::IData*> getSelectedNumors() const;

signals:
    void plotData(SX::Data::IData* data);

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
