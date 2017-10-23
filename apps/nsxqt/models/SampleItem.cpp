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

SampleItem::SampleItem(nsx::sptrExperiment experiment) : InspectableTreeItem(experiment)
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
    QJsonArray cells;

    auto sample = _experiment->getDiffractometer()->getSample();

    for (unsigned int i = 0; i < sample->getNCrystals(); ++i) {
        auto cell = sample->getUnitCell(i);
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
    auto sample = _experiment->getDiffractometer()->getSample();
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

        appendRow(new UnitCellItem(_experiment, cell));
    }
}

QList<UnitCellItem*> SampleItem::getUnitCellItems()
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
    auto sample=_experiment->getDiffractometer()->getSample();
    auto cell=sample->addUnitCell();
    appendRow(new UnitCellItem(_experiment,cell));
    child(0)->setEnabled(true);
}
