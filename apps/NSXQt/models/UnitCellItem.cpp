#include <QtDebug>

#include "Diffractometer.h"
#include "Experiment.h"
#include "Logger.h"
#include "Sample.h"
#include "UnitCell.h"

#include "models/UnitCellItem.h"
#include "tree/UnitCellPropertyWidget.h"
#include "dialogs/DialogUnitCellParameters.h"
#include "dialogs/DialogTransformationMatrix.h"

UnitCellItem::UnitCellItem(std::shared_ptr<SX::Instrument::Experiment> experiment,std::shared_ptr<SX::Crystal::UnitCell> cell):
    InspectableTreeItem(experiment),
    _cell(cell)
{
    QIcon icon(":/resources/unitCellIcon.png");
    setText(QString::fromStdString(_cell->getName()));
    setIcon(icon);
    setEditable(true);
    setDragEnabled(false);
    setDropEnabled(false);
    setForeground(QBrush(Qt::red));
}

UnitCellItem::~UnitCellItem()
{
    _experiment->getDiffractometer()->getSample()->removeUnitCell(_cell);
}

QWidget* UnitCellItem::inspectItem()
{
    return new UnitCellPropertyWidget(this);
}


std::shared_ptr<SX::Crystal::UnitCell> UnitCellItem::getUnitCell()
{
    return _cell;
}

void UnitCellItem::info() const
{
    qDebug() << "" << *_cell;
}

void UnitCellItem::openChangeUnitCellDialog()
{
    DialogUnitCellParameters* dialog = new DialogUnitCellParameters(_cell);
    dialog->exec();
}

void UnitCellItem::openTransformationMatrixDialog()
{
    DialogTransformationmatrix* dialog=new DialogTransformationmatrix(_cell);
    dialog->exec();
}
