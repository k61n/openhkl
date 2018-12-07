#include <QHeaderView>
#include <QMessageBox>
#include <QScrollBar>

#include "TaskManagerModel.h"
#include "TaskManagerView.h"
#include "TaskManagerViewDelegate.h"

TaskManagerView::TaskManagerView(QWidget *parent)
    : QTableView(parent)
{
    auto delegate = new TaskManagerViewDelegate(parent);

    setItemDelegate(delegate);

    connect(delegate,&TaskManagerViewDelegate::actionButtonPressed,[this](QModelIndex index){onActionButtonClicked(index);});
}

void TaskManagerView::setModel(TaskManagerModel *task_manager_model)
{
    QTableView::setModel(task_manager_model);

    verticalHeader()->hide();

    horizontalHeader()->hide();

    horizontalHeader()->setSectionResizeMode(static_cast<int>(nsx::TaskState::Section::PROGRESS),QHeaderView::Stretch);

    horizontalHeader()->resizeSection(static_cast<int>(nsx::TaskState::Section::ABORT),30);
    horizontalHeader()->resizeSection(static_cast<int>(nsx::TaskState::Section::START),30);
    horizontalHeader()->resizeSection(static_cast<int>(nsx::TaskState::Section::REMOVE),30);
}

void TaskManagerView::onActionButtonClicked(QModelIndex index)
{
    auto task_manager_model = dynamic_cast<TaskManagerModel*>(model());
    if (!task_manager_model) {
        return;
    }

    auto col = static_cast<nsx::TaskState::Section>(index.column());

    switch(col) {
    case nsx::TaskState::Section::ABORT: {
        task_manager_model->abortTask(index);
        break;
    }
    case nsx::TaskState::Section::START: {
        task_manager_model->runTask(index);
        break;
    }
    case nsx::TaskState::Section::REMOVE: {
        task_manager_model->removeTask(index);
        break;
    }
    default:
        break;
    }
    return;
}
