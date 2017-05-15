#include <QIcon>
#include <QJsonArray>

#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Sample.h>

#include "tree/SamplePropertyWidget.h"
#include "models/SampleShapeItem.h"
#include "models/SampleItem.h"
#include "models/UnitCellItem.h"

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
    QJsonArray cells;

    std::shared_ptr<nsx::Instrument::Sample> sample = _experiment->getDiffractometer()->getSample();

    for (unsigned int i = 0; i < sample->getNCrystals(); ++i) {
        std::shared_ptr<nsx::Crystal::UnitCell> cell = sample->getUnitCell(i);

        Eigen::Vector3d v[3];
        v[0] = cell->getAVector();
        v[1] = cell->getBVector();
        v[2] = cell->getCVector();

        QJsonArray params;

        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                params.push_back(v[j][k]);

        cells.push_back(params);
    }

    obj["shapes"] = cells;

    return obj;
}

void SampleItem::fromJson(const QJsonObject &obj)
{
    std::shared_ptr<nsx::Instrument::Sample> sample = _experiment->getDiffractometer()->getSample();
    QJsonArray shapes = obj["shapes"].toArray();

    for(QJsonValueRef shape: shapes) {
        Eigen::Vector3d v[3];
        QJsonArray params = shape.toArray();

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                v[i][j] = params[i*3+j].toDouble();

        std::shared_ptr<nsx::Crystal::UnitCell> cell = sample->addUnitCell();
        cell->setLatticeVectors(v[0], v[1], v[2]);

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
