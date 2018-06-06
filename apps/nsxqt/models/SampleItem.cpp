#include <algorithm>
#include <memory>

#include <QIcon>
#include <QJsonArray>

#include <nsxlib/Diffractometer.h>
#include <nsxlib/Sample.h>
#include <nsxlib/UnitCell.h>

#include "DialogIsotopesDatabase.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "MetaTypes.h"
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
    appendRow(shape);

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
        if (ucItem) {
            unitCellItems << ucItem;
        }
    }

    return unitCellItems;
}

void SampleItem::openIsotopesDatabase()
{
    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogIsotopesDatabase> dialog_ptr(new DialogIsotopesDatabase());

    if (!dialog_ptr->exec()) {
        return;
    }
}

void SampleItem::setData(const QVariant& value, int role)
{
    switch (role)
    {
    case Qt::UserRole:
        // Fetch the unit cell that been either updated either created
        auto uc = value.value<nsx::sptrUnitCell>();
        experimentItem()->experiment()->diffractometer()->getSample()->addUnitCell(uc);
        appendRow(new UnitCellItem(uc));
        break;
    }
    QStandardItem::setData(value,role);
}

