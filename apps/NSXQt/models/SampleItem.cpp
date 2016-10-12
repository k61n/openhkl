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
    QJsonArray cells;

    std::shared_ptr<SX::Instrument::Sample> sample = _experiment->getDiffractometer()->getSample();

    for (int i = 0; i < sample->getNCrystals(); ++i) {
        std::shared_ptr<SX::Crystal::UnitCell> cell = sample->getUnitCell(i);

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
