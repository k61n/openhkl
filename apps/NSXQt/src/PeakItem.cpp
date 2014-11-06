#include "PeakItem.h"

#include <QIcon>

PeakItem::PeakItem(Experiment* experiment) : TreeItem(experiment)
{
//    setText("Data");

    QIcon icon(":/resources/dataIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);
}
