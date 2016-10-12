#include <QIcon>
#include <QJsonArray>

#include "Diffractometer.h"
#include "Sample.h"

#include "Tree/SamplePropertyWidget.h"
#include "models/SampleShapeItem.h"
#include "models/SampleItem.h"

SampleItem::SampleItem(std::shared_ptr<Experiment> experiment) : InspectableTreeItem(experiment)
{
    setText("Sample");
    QIcon icon(":/resources/gonioIcon.png");
    setIcon(icon);

    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);
    SampleShapeItem* shape=new SampleShapeItem(_experiment);
    shape->setEnabled(false);
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

QJsonObject SampleItem::toJson()
{
    QJsonObject obj;

    obj["shapes"] = QJsonArray();

    return obj;
}
