#include <functional>
#include <iostream>

#include <QModelIndex>

#include <nsxlib/ITask.h>

#include "MetaTypes.h"
#include "TaskManagerModel.h"
#include "TaskManagerModel.h"

using registered_task = std::pair<std::string,std::shared_ptr<nsx::ITask*>>;

TaskManagerModel::TaskManagerModel(QObject *parent) : QAbstractTableModel(parent)
{

    QObject::connect(this, &TaskManagerModel::updateTask,
                     this,[=](QModelIndex index1, QModelIndex index2) {emit dataChanged(index1,index2);},
                     Qt::QueuedConnection);

    QObject::connect(this, &TaskManagerModel::completeTask,
                     this,[=](std::string task_name){_completeTask(task_name);},
                     Qt::QueuedConnection);
}

TaskManagerModel::~TaskManagerModel()
{
    for (int i = 0; i < rowCount(QModelIndex()); ++i) {
        removeTask(index(i,0));
    }
}

int TaskManagerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _tasks.size();
}

int TaskManagerModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return static_cast<int>(nsx::TaskState::Section::COUNT);
}

QVariant TaskManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(role)

    if (orientation == Qt::Vertical) {
        return QVariant();
    }

    auto task_state_section = static_cast<nsx::TaskState::Section>(section);

    switch (task_state_section) {
    case nsx::TaskState::Section::NAME:
        return QString("name");
    case nsx::TaskState::Section::TYPE:
        return QString("type");
    case nsx::TaskState::Section::PROGRESS:
        return QString("progress");
    case nsx::TaskState::Section::INFO:
        return QString("info");
    case nsx::TaskState::Section::START:
        return QString("start");
    case nsx::TaskState::Section::ABORT:
        return QString("abort");
    case nsx::TaskState::Section::REMOVE:
        return QString("remove");
    default:
        return QVariant();
    }
}

QVariant TaskManagerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    auto task_state = _task_states[row];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (static_cast<nsx::TaskState::Section>(col)) {
        case nsx::TaskState::Section::NAME: {
            return QString::fromStdString(task_state.name);
        }
        case nsx::TaskState::Section::TYPE: {
            return QString::fromStdString(task_state.type);
        }
        case nsx::TaskState::Section::PROGRESS: {
            int progress = task_state.n_steps > 0 ? task_state.current_step * 100 / task_state.n_steps : 0;
            return progress;
        }
        case nsx::TaskState::Section::INFO: {
            return QString::fromStdString(task_state.info);
        }
        default:
            break;
        }
    }

    return QVariant();
}

bool TaskManagerModel::hasTask(const std::string &name) const
{
    auto it = std::find_if(_task_states.begin(),_task_states.end(),[name](nsx::TaskState task_state){
        return name.compare(task_state.name) == 0;
    });

    return (it != _task_states.end());
}

void TaskManagerModel::addTask(std::shared_ptr<nsx::ITask> new_task, bool run)
{
    // If the task is a null pointer, return
    if (!new_task) {
        return;
    }

    // If this task is already registered, return
    auto it = std::find(_tasks.begin(),_tasks.end(),new_task);
    if (it != _tasks.end()) {
        return;
    }

    // Set the callback for this task
    new_task->setCallBack(std::bind(&TaskManagerModel::fetchState,this,new_task.get()));

    size_t row = _tasks.size();

    // Update the data
    beginInsertRows(QModelIndex(),row,row);
    _tasks.push_back(new_task);
    _task_states.push_back(new_task->state());
    _task_workers.push_back(std::future<bool>());
    endInsertRows();

    // Start the task if start argument is true
    if (run) {
        QModelIndex index = createIndex(row,0);
        runTask(index);
    }
}

void TaskManagerModel::abortTask(const QModelIndex &index)
{
    auto row = index.row();

    _tasks[row]->abort();

    if (_task_workers[row].valid()) {
        _task_workers[row].wait();
        _task_workers[row].get();
    }
}

void TaskManagerModel::removeTask(const QModelIndex &index)
{
    auto row = index.row();

    abortTask(index);

    // Update the model
    beginRemoveRows(QModelIndex(),row,row);
    _tasks.erase(_tasks.begin() + row);
    _task_states.erase(_task_states.begin() + row);
    _task_workers.erase(_task_workers.begin() + row);
    endRemoveRows();
}

void TaskManagerModel::runTask(const QModelIndex &index)
{
    auto row = index.row();

    auto task_state = _task_states[row];

    // The task is already running
    if (task_state.status == nsx::ITask::Status::STARTED) {
        return;
    }

    _task_workers[row] = std::async(std::launch::async,&nsx::ITask::run,_tasks[row]);
}

nsx::TaskState TaskManagerModel::state(const QModelIndex &index)
{
    auto row = index.row();

    return _task_states[row];
}

void TaskManagerModel::fetchState(nsx::ITask *task)
{
    // This code block is called from the threaded task

    // Fetch the state of the updated task
    auto state = task->state();

    // Lock the mutex while updating the TaskManager::_task_states resources
    std::unique_lock<std::mutex> lock(_mutex);

    // Find which task has been updated
    auto it = std::find_if(_task_states.begin(),_task_states.end(),[state](nsx::TaskState task_state){
        return state.name.compare(task_state.name) == 0;
    });
    auto row = std::distance(_task_states.begin(),it);

    // Replace the current task state by the new one
    _task_states[row] = state;

    // This signal is emitted from the threaded task
    emit updateTask(index(row,0,QModelIndex()),index(row,3,QModelIndex()));

    if (state.status == nsx::ITask::Status::COMPLETED) {
        emit completeTask(state.name);
    }

    // Unlock the mutex
    lock.unlock();
}

void TaskManagerModel::_completeTask(std::string task_name)
{
    // Find which task has been updated
    auto it = std::find_if(_task_states.begin(),_task_states.end(),[task_name](nsx::TaskState task_state){
        return task_name.compare(task_state.name) == 0;
    });
    auto row = std::distance(_task_states.begin(),it);

    auto task = _tasks[row];

    auto&& worker = _task_workers[row];
    if (worker.valid()) {
        worker.wait();
        if (worker.get()) {
            emit sendCompletedTask(task);
        }
    }
}

