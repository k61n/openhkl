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

#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/utils/ProgressHandler.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/utils/Types.h>

class ExperimentItem;
class SessionModel;

namespace nsx {
namespace Data {
    class DataSet;
}
}

class ExperimentTree : public QTreeView {
    Q_OBJECT
public:
    using sptrUnitCell = nsx::Crystal::sptrUnitCell;
    explicit ExperimentTree(QWidget *parent = 0);
    ~ExperimentTree();

    void setSession(std::shared_ptr<SessionModel> session);

signals:
    void plotData(std::shared_ptr<nsx::Data::DataSet>);
    void inspectWidget(QWidget*);
    void resetScene();

public slots:
    void keyPressEvent(QKeyEvent* event);
    void onCustomMenuRequested(const QPoint& point);
    void onDoubleClick(const QModelIndex& index);
    void importData();
    void importRawData();
    void findPeaks(const QModelIndex& index);
    void viewReciprocalSpace(const QModelIndex& index);
    void createNewExperiment();
    void onSingleClick(const QModelIndex& index);
    void absorptionCorrection();
    void showPeaksOpenGL();
    void findSpaceGroup();
    void computeRFactors();
    void findFriedelPairs();
    //void integrateCalculatedPeaks();
    void peakFitDialog();
    void incorporateCalculatedPeaks();
    void setHKLTolerance();

private:

    std::shared_ptr<nsx::Utils::ProgressHandler> _progressHandler;
    std::shared_ptr<nsx::Data::PeakFinder> _peakFinder;
    std::shared_ptr<SessionModel> _session;
};

#endif // EXPERIMENTTREE_H
