#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include <nsxlib/DataTypes.h>

class DataItem;
class ExperimentItem;
class MainWindow;
class SessionModel;

class ExperimentTree: public QTreeView {
    Q_OBJECT
public:
    explicit ExperimentTree(QWidget *parent);

    ~ExperimentTree();

    SessionModel* session();

signals:
    void plotData(nsx::sptrDataSet);
    void inspectWidget(QWidget*);
    void resetScene();
    void openPeakFindDialog(DataItem *data_item);

public slots:
    void keyPressEvent(QKeyEvent* event);
    void onCustomMenuRequested(const QPoint& point);
    void onDoubleClick(const QModelIndex& index);

    void onSingleClick(const QModelIndex& index);

private:

    void openInstrumentStatesDialog();
};
