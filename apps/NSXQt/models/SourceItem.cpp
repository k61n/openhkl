#include <QIcon>
#include <QString>

#include "Diffractometer.h"
#include "Source.h"
#include "Experiment.h"
#include "models/SourceItem.h"
#include "Tree/SourcePropertyWidget.h"

SourceItem::SourceItem(std::shared_ptr<SX::Instrument::Experiment> experiment) : InspectableTreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getDiffractometer()->getSource()->getName()));

    QIcon icon(":/resources/sourceIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);
}

QWidget* SourceItem::inspectItem()
{
    return new SourcePropertyWidget(this);
}
