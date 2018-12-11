#pragma once

#include <QPoint>
#include <QTreeView>

#include <core/DataTypes.h>

class DataItem;
class MainWindow;
class SessionModel;

class ExperimentTree : public QTreeView {
    Q_OBJECT
public:
    explicit ExperimentTree(QWidget* parent);

    ~ExperimentTree();

    void setMainWindow(MainWindow* main_window);

    SessionModel* session();

signals:
    void inspectWidget(QWidget*);
    void resetScene();
    void openPeakFindDialog(DataItem* data_item);

public slots:
    void keyPressEvent(QKeyEvent* event);
    void onCustomMenuRequested(const QPoint& point);
    void onDoubleClick(const QModelIndex& index);

    void onSingleClick(const QModelIndex& index);

private:
    void openInstrumentStatesDialog();

private:
    MainWindow* _main_window;
};
