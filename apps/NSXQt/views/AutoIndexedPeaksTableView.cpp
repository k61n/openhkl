#include "views/AutoIndexedPeaksTableView.h"

#include <QtDebug>

#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QModelIndexList>

#include "CollectedPeaksModel.h"
#include "CollectedPeaksDelegate.h"

AutoIndexedPeaksTableView::AutoIndexedPeaksTableView(QWidget *parent) :
    QTableView(parent)
{
//    setEditTriggers(QAbstractItemView::DoubleClicked);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::MultiSelection);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setMinimumSize(800,400);
    setFocusPolicy(Qt::StrongFocus);

    CollectedPeaksDelegate *delegate = new CollectedPeaksDelegate(this);
    setItemDelegate(delegate);
}

void AutoIndexedPeaksTableView::contextMenuEvent(QContextMenuEvent* event)
{
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty())
        return;

    // Show all peaks as selected when context menu is requested
    QMenu* menu=new QMenu(this);

    QAction* clearSelectedPeaks=new QAction("clear selection",menu);
    QAction* selectedUnindexedPeaks=new QAction("select unindexed peaks",menu);
    menu->addAction(clearSelectedPeaks);
    menu->addAction(selectedUnindexedPeaks);
    menu->popup(event->globalPos());

    connect(clearSelectedPeaks,SIGNAL(triggered()),this,SLOT(clearSelectedPeaks()));
    connect(selectedUnindexedPeaks,SIGNAL(triggered()),this,SLOT(selectUnindexedPeaks()));
}

void AutoIndexedPeaksTableView::selectUnindexedPeaks()
{
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    QModelIndexList unindexedPeaks = peaksModel->getUnindexedPeaks();

    for (QModelIndex index : unindexedPeaks)
        selectRow(index.row());
}

void AutoIndexedPeaksTableView::clearSelectedPeaks()
{
    selectionModel()->clear();
}
