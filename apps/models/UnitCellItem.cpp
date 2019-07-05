//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/UnitCellItem.cpp
//! @brief     Implements class UnitCellItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <memory>
#include <sstream>

#include <QDebug>
#include <QInputDialog>
#include <QtGlobal>

#include "apps/dialogs/DialogSpaceGroup.h"
#include "apps/dialogs/DialogTransformationMatrix.h"
#include "apps/dialogs/DialogUnitCellParameters.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/PeaksItem.h"
#include "apps/models/UnitCellItem.h"
#include "apps/models/UnitCellsItem.h"
#include "apps/tree/UnitCellPropertyWidget.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

UnitCellItem::UnitCellItem(nsx::sptrUnitCell unit_cell)
    : InspectableTreeItem(), _unit_cell(unit_cell)
{
    setText(QString::fromStdString(_unit_cell->name()));

    setIcon(QIcon(":/resources/unitCellIcon.png"));

    setEditable(true);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(true);

    setCheckState(Qt::Unchecked);
}

UnitCellItem::~UnitCellItem() {}

QVariant UnitCellItem::data(int role) const
{
    switch (role) {
        case (Qt::DecorationRole): {
            return QIcon(":/resources/unitCellIcon.png");
        }
        case (Qt::EditRole): {
            return QString::fromStdString(_unit_cell->name());
        }
        case (Qt::DisplayRole): {
            return QString::fromStdString(_unit_cell->name());
        }
        case (Qt::UserRole): {
            return QVariant::fromValue(_unit_cell);
        }
    }
    return InspectableTreeItem::data(role);
}

void UnitCellItem::setData(const QVariant& value, int role)
{
    switch (role) {
        case (Qt::DisplayRole): {
            _unit_cell->setName(value.toString().toStdString());
            break;
        }
        case (Qt::EditRole): {
            _unit_cell->setName(value.toString().toStdString());
            break;
        }
    }
    InspectableTreeItem::setData(value, role);
}

QWidget* UnitCellItem::inspectItem()
{
    return new UnitCellPropertyWidget(this);
}

void UnitCellItem::info() const
{
    std::ostringstream os;
    os << "" << *(_unit_cell);
    qDebug() << QString::fromStdString(os.str());
}

void UnitCellItem::openChangeUnitCellDialog()
{
    std::unique_ptr<DialogUnitCellParameters> dialog(new DialogUnitCellParameters(_unit_cell));
    dialog->exec();

    emitDataChanged();
}

void UnitCellItem::openTransformationMatrixDialog()
{
    std::unique_ptr<DialogTransformationMatrix> dialog(new DialogTransformationMatrix(_unit_cell));
    dialog->exec();

    emitDataChanged();
}

void UnitCellItem::openSpaceGroupDialog()
{
    auto selected_peaks = experimentItem()->peaksItem()->selectedPeaks();

    std::unique_ptr<DialogSpaceGroup> dialog(new DialogSpaceGroup(selected_peaks));

    if (!dialog->exec())
        return;

    auto group = dialog->getSelectedGroup();

    auto space_group = nsx::SpaceGroup(group);

    _unit_cell->setSpaceGroup(space_group);

    emitDataChanged();
}

void UnitCellItem::openIndexingToleranceDialog()
{
    bool ok;
    double tolerance = QInputDialog::getDouble(
        nullptr, "Enter indexing tolerance", "", _unit_cell->indexingTolerance(), 0.0, 1.0, 6, &ok);
    if (ok)
        _unit_cell->setIndexingTolerance(tolerance);

    emitDataChanged();
}
