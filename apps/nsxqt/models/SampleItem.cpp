#include <algorithm>
#include <memory>

#include <QIcon>
#include <QJsonArray>

#include <nsxlib/Diffractometer.h>
#include <nsxlib/Sample.h>
#include <nsxlib/UnitCell.h>

#include "DialogIsotopesDatabase.h"
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
    shape->setEnabled(false);
    appendRow(shape);

}

QWidget* SampleItem::inspectItem()
{
    return new SamplePropertyWidget(this);
}

QJsonObject SampleItem::toJson()
{
    QJsonObject obj;
    QJsonArray cells;

    auto sample = experiment()->diffractometer()->getSample();

    for (unsigned int i = 0; i < sample->getNCrystals(); ++i) {
        auto cell = sample->unitCell(i);
        Eigen::Matrix3d A = cell->basis();
        QJsonArray params;

        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                params.push_back(A(k, j));

        cells.push_back(params);
    }

    obj["shapes"] = cells;

    return obj;
}

void SampleItem::fromJson(const QJsonObject &obj)
{
    auto sample = experiment()->diffractometer()->getSample();
    QJsonArray shapes = obj["shapes"].toArray();

    for(QJsonValueRef shape: shapes) {
        Eigen::Vector3d v[3];
        QJsonArray params = shape.toArray();

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                v[i][j] = params[i*3+j].toDouble();

        auto cell = sample->addUnitCell();
        Eigen::Matrix3d A;
        A.col(0) = v[0];
        A.col(1) = v[1];
        A.col(2) = v[2];
        cell->setBasis(A);

        appendRow(new UnitCellItem(cell));
    }
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
        auto updated_cell = value.value<nsx::sptrUnitCell>();
        bool new_unit_cell(true);
        for (size_t i=1; i< rowCount();++i) {
            auto unit_cell_item = model()->itemFromIndex(index());
            QVariant variant = unit_cell_item->data(Qt::UserRole);
            auto unit_cell = variant.value<nsx::sptrUnitCell>();
            // Case of un updated cell
            if (updated_cell == unit_cell) {
                model()->setData(unit_cell_item->index(),QVariant::fromValue(updated_cell),Qt::UserRole);
                new_unit_cell = false;
                break;
            }
        }
        // Case of a new unit cell
        if (new_unit_cell) {
            appendRow(new UnitCellItem(updated_cell));
        }

        break;
    }
    QStandardItem::setData(value,role);
}

