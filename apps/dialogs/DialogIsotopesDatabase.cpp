#include <sstream>

#include <QStandardItem>
#include <QStandardItemModel>

#include <core/ChemistryTypes.h>
#include <core/IsotopeDatabaseManager.h>
#include <core/Units.h>

#include "DialogIsotopesDatabase.h"

#include "ui_DialogIsotopesDatabase.h"

DialogIsotopesDatabase::DialogIsotopesDatabase(QWidget *parent)
: QDialog(parent), 
  ui(new Ui::DialogIsotopesDatabase)
{
    ui->setupUi(this);

    // Make sure that the user can not edit the content of the table
    ui->isotopeDatabaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    nsx::IsotopeDatabaseManager* imgr=nsx::IsotopeDatabaseManager::Instance();

    const auto& properties = imgr->properties();

    QStandardItemModel* model = new QStandardItemModel(0,properties.size());

    unsigned int comp(0);
    for (const auto& prop : properties) {
        QString unit = QString("%1 (%2)").arg(QString::fromStdString(prop.first)).arg(QString::fromStdString(prop.second.first));
        model->setHorizontalHeaderItem(comp++,new QStandardItem(unit));
    }

    nsx::UnitsManager* um=nsx::UnitsManager::Instance();

    const auto& isotopes = imgr->isotopes();
    unsigned int isotopeCount = 0;
    for (const auto& isotope : isotopes) {
        std::string isotopeName = isotope.first;
        unsigned int propertyCount = 0;
        for (const auto& prop : properties) {
            std::string pName = prop.first;
            std::string pType = prop.second.first;
            std::string pUnit = prop.second.second;
            QStandardItem* item = new QStandardItem();
            if (imgr->hasProperty(isotopeName,pName)) {
                switch (nsx::IsotopeDatabaseManager::PropertyTypes.at(pType)) {
                case nsx::ChemicalPropertyType::Int:
                {
                    item->setText(QString::number(imgr->property<int>(isotopeName,pName)));
                    break;
                }
                case nsx::ChemicalPropertyType::Double:
                {
                    auto value = imgr->property<double>(isotopeName,pName)/um->get(pUnit);
                    item->setText(QString::number(value));
                    break;
                }
                case nsx::ChemicalPropertyType::Complex:
                {
                    auto value = imgr->property<std::complex<double>>(isotopeName,pName)/um->get(pUnit);
                    std::ostringstream os;
                    os << value;
                    item->setText(QString::fromStdString(os.str()));
                    break;
                }
                case nsx::ChemicalPropertyType::Bool:
                {
                    item->setText(QString::number(imgr->property<bool>(isotopeName,pName)));
                    break;
                }
                default:
                    item->setText(QString::fromStdString(imgr->property<std::string>(isotopeName,pName)));
                }
            } else
                item->setText("NaN");

            model->setItem(isotopeCount,propertyCount++,item);
        }
        model->setVerticalHeaderItem(isotopeCount++,new QStandardItem(QString::fromStdString(isotope.first)));
    }

    ui->isotopeDatabaseView->setModel(model);

    resize(1000,500);
    ui->isotopeDatabaseView->show();
}

DialogIsotopesDatabase::~DialogIsotopesDatabase()
{
    delete ui;
}
