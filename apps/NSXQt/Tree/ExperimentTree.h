#ifndef EXPERIMENTTREE_H
#define EXPERIMENTTREE_H

#include <memory>
#include <string>
#include <vector>
#include <memory>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include "Experiment.h"
#include "ProgressHandler.h"
#include "PeakFinder.h"

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
    std::vector<std::shared_ptr<SX::Data::IData>> getSelectedNumors() const;
    std::vector<std::shared_ptr<SX::Data::IData>> getSelectedNumors(ExperimentItem* item) const;

    // ExperimentItem* getExperimentItem(Experiment* exp); // no longer used?

signals:
    void plotData(std::shared_ptr<SX::Data::IData>);
    void showPeakList(std::vector<std::shared_ptr<SX::Data::IData>>);
    void inspectWidget(QWidget*);
public slots:
    void keyPressEvent(QKeyEvent* event);
    void onCustomMenuRequested(const QPoint& point);
    void onDoubleClick(const QModelIndex& index);
    void importData();
    void findPeaks(const QModelIndex& index);
    void viewReciprocalSpace(const QModelIndex& index);
    void createNewExperiment();
    void onSingleClick(const QModelIndex& index);
    void absorptionCorrection();
    void showPeaksOpenGL();
    void findSpaceGroup();

private:

    QStandardItemModel* _model;

    std::shared_ptr<SX::Utils::ProgressHandler> _progressHandler;
    std::shared_ptr<SX::Data::PeakFinder> _peakFinder;
};

#endif // EXPERIMENTTREE_H
