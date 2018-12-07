#pragma once

#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <QAbstractTableModel>

#include <nsxlib/ITask.h>

class TaskManagerModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    TaskManagerModel(QObject *parent = nullptr);

    ~TaskManagerModel();

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    //! Check whether a task is registered into the model
    bool hasTask(const std::string& name) const;

    //! Abort a task registered into the model
    void abortTask(const QModelIndex& index);

    //! Remove a task registered into the model
    void removeTask(const QModelIndex& index);

    //! Submit a new task to the model
    void addTask(std::shared_ptr<nsx::ITask> task, bool run);

    //! Start a task registered into the model
    void runTask(const QModelIndex &index);

    //! Return the current state of a task registered into the model
    nsx::TaskState state(const QModelIndex &index);

signals:

    //! Emitted when a registered task is updated
    void updateTask(QModelIndex index1, QModelIndex index2);

    //! Emitted when a registered task is completed
    void completeTask(std::string task_name);

    //! Emitted when a completed task is send to the client views
    void sendCompletedTask(std::shared_ptr<nsx::ITask> task);

private:

    //! Fetch the current state of a registered task (called in a different thread)
    void fetchState(nsx::ITask *task);

    //! Prepare the completed task before sending it to the client views (called from the main thread)
    void _completeTask(std::string task_name);

private:

    mutable std::mutex _mutex;

    std::vector<std::string> _task_names;

    std::vector<nsx::TaskState> _task_states;

    std::vector<std::future<bool>> _task_workers;

    std::vector<std::shared_ptr<nsx::ITask>> _tasks;
};
