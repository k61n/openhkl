#include "SampleItem.h"
#include <QIcon>
#include "Diffractometer.h"
#include "Sample.h"
#include "SamplePropertyWidget.h"
#include "SampleShapeItem.h"

SampleItem::SampleItem(Experiment* experiment) : InspectableTreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getDiffractometer()->getSample()->getName()));
    QIcon icon(":/resources/gonioIcon.png");
    setIcon(icon);

    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);
    SampleShapeItem* shape=new SampleShapeItem(_experiment);
    appendRow(shape);
}

void SampleItem::setData(const QVariant &value, int role)
{
    QStandardItem::setData(value,role);
    _experiment->getDiffractometer()->getSample()->setName(text().toStdString());
}

QWidget* SampleItem::inspectItem()
{
    return new SamplePropertyWidget(this);
}
