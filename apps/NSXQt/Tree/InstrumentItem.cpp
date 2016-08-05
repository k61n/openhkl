#include <QIcon>
#include <QString>

#include "Tree/InstrumentItem.h"

InstrumentItem::InstrumentItem(std::shared_ptr<Experiment> experiment) : TreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getDiffractometerType()));

    QIcon icon(":/resources/instrumentIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);

    setSelectable(false);
}
