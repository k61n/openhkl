#pragma once

#include <QModelIndex>
#include <QTableView>

class QWidget;
class QResizeEvent;

class TaskManagerView : public QTableView
{
    Q_OBJECT

public:

    TaskManagerView(QWidget *parent);

signals:

    void abortTask(QModelIndex index);

    void runTask(QModelIndex index);

private:

    void onActionButtonClicked(QModelIndex index);
};
