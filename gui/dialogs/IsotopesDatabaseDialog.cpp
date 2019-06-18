//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#include "gui/MainWin.h"
#include "base/utils/Units.h"
#include "tables/chemistry/ChemistryTypes.h"
#include "tables/chemistry/IsotopeDatabaseManager.h"
#include <QVBoxLayout>
#include <QStandardItemModel>

IsotopesDatabaseDialog::IsotopesDatabaseDialog()
    : QDialog{}
{
    QVBoxLayout* grid = new QVBoxLayout(this);
    isotopeDatabaseView = new QTableView;
    isotopeDatabaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    nsx::IsotopeDatabaseManager* isotopesManager = nsx::IsotopeDatabaseManager::Instance();
    const auto& properties = isotopesManager->properties();
    QStandardItemModel* model = new QStandardItemModel(0, properties.size());
    unsigned int comp(0);
    for (const auto& property : properties) {
        QString unit = QString("%1 (%2)")
                .arg(QString::fromStdString(property.first))
                .arg(QString::fromStdString(property.second.first));
        model->setHorizontalHeaderItem(comp++, new QStandardItem(unit));
    }
    nsx::UnitsManager* unitsMgr = nsx::UnitsManager::Instance();
    const std::map<std::string, nsx::isotopeProperties>& isotopes = isotopesManager->isotopes();
    unsigned int isotopeCount(0);
    for (const auto& isotope : isotopes) {
        std::string isotopeName = isotope.first;
        unsigned int propertyCount(0);
        for (const auto& prop : properties) {
            std::string pName = prop.first;
            std::string pType = prop.second.first;
            std::string pUnit = prop.second.second;
            QStandardItem* item = new QStandardItem;
            if (isotopesManager->hasProperty(isotopeName, pName)) {
                switch (nsx::IsotopeDatabaseManager::PropertyTypes.at(pType)) {
                case nsx::ChemicalPropertyType::Int: {
                    item->setText(QString::number(
                                      isotopesManager->property<int>(isotopeName, pName)));
                    break;
                }
                case nsx::ChemicalPropertyType::Double: {
                    double value = isotopesManager->property<double>(isotopeName, pName)/
                            unitsMgr->get(pUnit);
                    item->setText(QString::number(value));
                    break;
                }
                case nsx::ChemicalPropertyType::Complex: {
                    std::complex<double> value =
                            isotopesManager->property<std::complex<double>>(isotopeName, pName)/
                                                                           unitsMgr->get(pUnit);
                    std::ostringstream os;
                    os << value;
                    item->setText(QString::fromStdString(os.str()));
                    break;
                }
                case nsx::ChemicalPropertyType::Bool: {
                    item->setText(QString::number(
                                      isotopesManager->property<bool>(isotopeName, pName)));
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
        model->setVerticalHeaderItem(isotopeCount++,
                                     new QStandardItem(QString::fromStdString(isotopeName)));
    }
    isotopeDatabaseView->setModel(model);
    grid->addWidget(isotopeDatabaseView);
    isotopeDatabaseView->show();
    resize(1000, 500);
}
