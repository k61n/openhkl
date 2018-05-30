#include <memory>

#include <QIcon>
#include <QJsonArray>

#include <nsxlib/Diffractometer.h>
#include <nsxlib/Sample.h>
#include <nsxlib/UnitCell.h>

#include "SampleItem.h"
#include "SamplePropertyWidget.h"
#include "SampleShapeItem.h"
#include "UnitCellItem.h"

SampleItem::SampleItem() : InspectableTreeItem()
{
    setText("Sample");
    QIcon icon(":/resources/gonioIcon.png");
    setIcon(icon);

    setEditable(false);
    setSelectable(false);
    setDragEnabled(false);
    setDropEnabled(false);
    SampleShapeItem* shape = new SampleShapeItem;
    shape->setEnabled(false);
    appendRow(shape);

}

void SampleItem::setData(const QVariant &value, int role)
{
    if (parent()) {
        experiment()->getDiffractometer()->getSample()->setName(text().toStdString());
    }
    QStandardItem::setData(value,role);
}

QWidget* SampleItem::inspectItem()
{
    return new SamplePropertyWidget(this);
}

QList<UnitCellItem*> SampleItem::unitCellItems()
{
    QList<UnitCellItem*> unitCellItems;

    QModelIndex sampleItemIdx = model()->indexFromItem(this);

    for (int i=0;i<model()->rowCount(sampleItemIdx);++i)
    {
        QModelIndex idx = model()->index(i,0,sampleItemIdx);
        QStandardItem* item = model()->itemFromIndex(idx);
        UnitCellItem* ucItem = dynamic_cast<UnitCellItem*>(item);
        if (ucItem)
            unitCellItems << ucItem;
    }

    return unitCellItems;
}

void SampleItem::addUnitCell()
{
    auto sample = experiment()->getDiffractometer()->getSample();
    auto cell = sample->addUnitCell();
    appendRow(new UnitCellItem(cell));
    child(0)->setEnabled(true);
}
