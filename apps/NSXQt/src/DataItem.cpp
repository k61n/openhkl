#include "DataItem.h"

#include <QIcon>
#include <QString>

DataItem::DataItem(Experiment* experiment) : TreeItem(experiment)
{
    setText("Data");

    QIcon icon(":/resources/dataIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);
}
