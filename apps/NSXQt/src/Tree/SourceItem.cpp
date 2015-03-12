#include <QIcon>
#include <QString>

#include "Diffractometer.h"
#include "Source.h"

#include "Tree/SourceItem.h"

SourceItem::SourceItem(Experiment* experiment) : TreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getDiffractometer()->getSource()->getName()));

    QIcon icon(":/resources/sourceIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);
}
