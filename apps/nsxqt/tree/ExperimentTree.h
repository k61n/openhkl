#ifndef NSXQT_EXPERIMENTTREE_H
#define NSXQT_EXPERIMENTTREE_H

#include <memory>
#include <string>
#include <vector>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include <nsxlib/data/DataTypes.h>

class ExperimentItem;
class SessionModel;

class ExperimentTree : public QTreeView {
    Q_OBJECT
public:
    explicit ExperimentTree(QWidget *parent = 0);
    ~ExperimentTree();

    void setSession(std::shared_ptr<SessionModel> session);

signals:
    void plotData(nsx::sptrDataSet);
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
    void peakFitDialog();
    void incorporateCalculatedPeaks();
    void setHKLTolerance();

private:

    std::shared_ptr<SessionModel> _session;
};

#endif // NSXQT_EXPERIMENTTREE_H
