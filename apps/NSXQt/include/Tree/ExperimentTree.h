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

class ExperimentItem;

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
    ~ExperimentTree();

    void addExperiment(const std::string& experimentName, const std::string& instrumentName);
    std::vector<SX::Data::IData*> getSelectedNumors() const;
    std::vector<SX::Data::IData*> getSelectedNumors(ExperimentItem* item) const;
    ExperimentItem* getExperimentItem(Experiment* exp);

signals:
    void plotData(SX::Data::IData*);
    void showPeakList(std::vector<SX::Data::IData*>);
    void inspectWidget(QWidget*);
public slots:
    void keyPressEvent(QKeyEvent* event);
    void onCustomMenuRequested(const QPoint& point);
    void onDoubleClick(const QModelIndex& index);
    void importData();
    void createNewExperiment();
    void onSingleClick(const QModelIndex& index);
    void absorptionCorrection();
    void showPeaksOpenGL();

private:

    QStandardItemModel* _model;

};

#endif // EXPERIMENTTREE_H
