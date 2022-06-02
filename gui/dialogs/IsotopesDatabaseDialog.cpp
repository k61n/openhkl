//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/IsotopesDatabaseDialog.cpp
//! @brief     Implements class IsotopesDatabaseDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/IsotopesDatabaseDialog.h"

#include "base/utils/Units.h"
#include "gui/MainWin.h"
#include <QHeaderView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <xsection/IsotopeDatabaseManager.h>

IsotopesDatabaseDialog::IsotopesDatabaseDialog() : QDialog{}
{
    QVBoxLayout* grid = new QVBoxLayout(this);
    isotopeDatabaseView = new QTableView;
    isotopeDatabaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    isotopeDatabaseView->setSortingEnabled(true);
    const auto* isotopesManager = xsection::IsotopeDatabaseManager::instance();
    const auto& properties = isotopesManager->properties();
    QStandardItemModel* model = new QStandardItemModel(0, properties.size());
    unsigned int comp(1);
    for (const auto& property : properties) {
        QString unit = QString("%1").arg(QString::fromStdString(property.first));
        QString unitlong = QString("%1 (%2)")
                               .arg(QString::fromStdString(property.first))
                               .arg(QString::fromStdString(property.second.first));
        QStandardItem* headerItem = new QStandardItem(unit);
        headerItem->setToolTip(unitlong);
        headerItem->setTextAlignment(Qt::AlignLeft);
        model->setHorizontalHeaderItem(comp++, headerItem);
    }
    const std::map<std::string, xsection::isotopeProperties>& isotopes =
        isotopesManager->isotopes();
    unsigned int isotopeCount(0);
    for (const auto& isotope : isotopes) {
        std::string isotopeName = isotope.first;
        model->setVerticalHeaderItem(
            isotopeCount, new QStandardItem(QString::fromStdString(isotopeName)));
        model->setItem(isotopeCount, 0, new QStandardItem(QString::fromStdString(isotopeName)));
        unsigned int propertyCount(1);
        for (const auto& prop : properties) {
            std::string pName = prop.first;
            std::string pType = prop.second.first;
            std::string pUnit = prop.second.second;
            QStandardItem* item = new QStandardItem;
            if (isotopesManager->hasProperty(isotopeName, pName)) {
                switch (xsection::IsotopeDatabaseManager::PropertyTypes.at(pType)) {
                    case xsection::ChemicalPropertyType::Int: {
                        item->setText(
                            QString::number(isotopesManager->property<int>(isotopeName, pName)));
                        break;
                    }
                    case xsection::ChemicalPropertyType::Double: {
                        double value = isotopesManager->property<double>(isotopeName, pName)
                            / nsx::UnitsManager::get(pUnit);
                        item->setText(QString::number(value));
                        break;
                    }
                    case xsection::ChemicalPropertyType::Complex: {
                        std::complex<double> value =
                            isotopesManager->property<std::complex<double>>(isotopeName, pName)
                            / nsx::UnitsManager::get(pUnit);
                        std::ostringstream os;
                        os << value;
                        item->setText(QString::fromStdString(os.str()));
                        break;
                    }
                    case xsection::ChemicalPropertyType::Bool: {
                        item->setText(
                            QString::number(isotopesManager->property<bool>(isotopeName, pName)));
                        break;
                    }
                    default:
                        item->setText(QString::fromStdString(
                            isotopesManager->property<std::string>(isotopeName, pName)));
                }
            } else
                item->setText("NaN");
            model->setItem(isotopeCount, propertyCount++, item);
        }
        isotopeCount++;
    }
    isotopeDatabaseView->setModel(model);
    isotopeDatabaseView->setColumnHidden(0, true);
    grid->addWidget(isotopeDatabaseView);
    connect(isotopeDatabaseView, &QTableView::clicked, this, &IsotopesDatabaseDialog::cellClicked);
    connect(
        isotopeDatabaseView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this,
        &IsotopesDatabaseDialog::sortingChanged);
    isotopeDatabaseView->show();
    resize(1000, 500);
}

void IsotopesDatabaseDialog::cellClicked(const QModelIndex& index)
{
    int row = index.row();
    int col = index.column();
    int totalCol = isotopeDatabaseView->model()->columnCount();
    int totalRow = isotopeDatabaseView->model()->rowCount();

    QModelIndex up = isotopeDatabaseView->model()->index(row, 0);
    QModelIndex down = isotopeDatabaseView->model()->index(row, totalCol - 1);
    QModelIndex left = isotopeDatabaseView->model()->index(0, col);
    QModelIndex right = isotopeDatabaseView->model()->index(totalRow - 1, col);

    QItemSelection selection(up, down);
    selection.merge(QItemSelection(left, right), QItemSelectionModel::Select);
    isotopeDatabaseView->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void IsotopesDatabaseDialog::sortingChanged(int index, Qt::SortOrder order)
{
    Q_UNUSED(index)
    Q_UNUSED(order)
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(isotopeDatabaseView->model());
    for (int i = 0; i < model->rowCount(); i++) {
        QString text = model->item(i, 0)->text();
        model->verticalHeaderItem(i)->setText(text);
    }
}
