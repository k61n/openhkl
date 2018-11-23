#include <QApplication>
#include <QDebug>
#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>

#include <nsxlib/ITask.h>

#include "TaskManagerModel.h"
#include "TaskManagerViewDelegate.h"

QColor statusColor(nsx::ITask::Status status)
{
    switch (status) {
    case nsx::ITask::Status::SUBMITTED:
        return Qt::green;
    case nsx::ITask::Status::STARTED:
        return Qt::green;
    case nsx::ITask::Status::ABORTED:
        return Qt::red;
    case  nsx::ITask::Status::COMPLETED:
        return Qt::blue;
    default:
        return Qt::white;
    }
}

QIcon buttonIcon(nsx::TaskState::Section col)
{
    switch (col) {
    case (nsx::TaskState::Section::START):
        return QIcon(":/resources/runTaskIcon.png");
    case (nsx::TaskState::Section::ABORT):
        return QIcon(":/resources/abortTaskIcon.png");
    case (nsx::TaskState::Section::REMOVE):
        return QIcon(":/resources/removeTaskIcon.png");
    default:
        return QIcon();
    }
}

TaskManagerViewDelegate::TaskManagerViewDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
}

void TaskManagerViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto task_manager_model = dynamic_cast<TaskManagerModel*>(const_cast<QAbstractItemModel*>(index.model()));

    auto task_state = task_manager_model->state(index);

    auto col = static_cast<nsx::TaskState::Section>(index.column());

    int cell_pos_x = option.rect.x();
    int cell_pos_y = option.rect.y();
    int cell_width = option.rect.width();
    int cell_height = option.rect.height();

    if (col == nsx::TaskState::Section::PROGRESS) {

        int progress = index.model()->data(index).toInt();

        QRect progress_bar_rect(cell_pos_x,cell_pos_y,progress * cell_width / 100,cell_height);

        painter->save();

        painter->setRenderHint(QPainter::Antialiasing,true);
        painter->setBrush(statusColor(task_state.status));
        int text_pos_x = cell_pos_x + cell_width/2;
        int text_pos_y = cell_pos_y + cell_height/2;

        QFont font;
        font.setBold(true);
        painter->setFont(font);
        painter->drawRoundedRect(progress_bar_rect,5,5);
        painter->drawText(text_pos_x-20,text_pos_y-6,40,12,Qt::AlignCenter,QString::number(progress) + "%");
        painter->restore();

    } else if (col == nsx::TaskState::Section::START ||
               col == nsx::TaskState::Section::ABORT ||
               col == nsx::TaskState::Section::REMOVE) {

        painter->save();

        painter->setRenderHint(QPainter::Antialiasing, true);

        auto it = _button_states.find(index);
        if (it == _button_states.end()) {
            QStyleOptionButton button;
            button.rect = QRect(cell_pos_x,cell_pos_y,cell_width,cell_height);
            button.icon = buttonIcon(col);
            button.iconSize = QSize(cell_height,cell_height);
            button.state = QStyle::State_Raised | QStyle::State_Enabled;
            auto p = _button_states.emplace(index,button);
            it = p.first;
        } else {
            it->second.rect = QRect(cell_pos_x,cell_pos_y,cell_width,cell_height);
        }

        QApplication::style()->drawControl(QStyle::CE_PushButton,&it->second, painter);

        painter->restore();

    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

bool TaskManagerViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseButtonRelease) {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    auto col = static_cast<nsx::TaskState::Section>(index.column());

    if (col == nsx::TaskState::Section::START ||
        col == nsx::TaskState::Section::ABORT ||
        col == nsx::TaskState::Section::REMOVE) {

        auto it = _button_states.find(index);

        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

        if (!option.rect.contains(mouse_event->pos())) {
            return false;
        }

        if( event->type() == QEvent::MouseButtonPress) {
            it->second.state = QStyle::State_Sunken | QStyle::State_Enabled;
        } else if( event->type() == QEvent::MouseButtonRelease) {
            it->second.state = QStyle::State_Raised | QStyle::State_Enabled;
            emit actionButtonPressed(index);
        }
    }

    return true;
}
