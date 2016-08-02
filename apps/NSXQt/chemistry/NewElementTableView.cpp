#include "NewElementTableView.h"

#include "DragElementModel.h"

#include <QDropEvent>

NewElementTableView::NewElementTableView(QWidget *parent) :
    QTableView(parent)
{
}

void NewElementTableView::dropEvent(QDropEvent *event)
{
    auto p=dynamic_cast<DragElementModel*>(model());
    p->setSender(event->source());
    QTableView::dropEvent(event);
}
