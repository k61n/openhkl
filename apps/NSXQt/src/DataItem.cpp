#include "DataItem.h"
#include "Experiment.h"
#include <QIcon>

DataItem::DataItem(Experiment* experiment) : TreeItem(experiment)
{
    setText("Data");

    QIcon icon(":/resources/dataIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);
}
