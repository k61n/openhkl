#include <QtDebug>
#include <QtMessageHandler>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>

#include <iostream>
#include <complex>
#include <limits>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Error.h"
#include "ComplexParser.h"
#include "include/Chemistry/IsotopeDatabaseDialog.h"
#include "ui_IsotopeDatabaseDialog.h"
#include "IsotopeManager.h"

IsotopeDatabaseDialog::IsotopeDatabaseDialog(QWidget *parent) : QDialog(parent), ui(new Ui::IsotopeDatabaseDialog)
{
    ui->setupUi(this);

    // Make sure that the user can not edit the content of the table
    ui->isotopeDatabaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    SX::Chemistry::IsotopeManager* imgr=SX::Chemistry::IsotopeManager::Instance();

    // Loads the database into a property tree
    boost::property_tree::ptree root;
    try
    {
        boost::property_tree::xml_parser::read_xml(imgr->getDatabasePath(),root);
    }
    catch (const std::runtime_error& error)
    {
        qCritical()<<error.what();
    }

    const SX::Chemistry::unitsMap& umap = imgr->getUnits();

    QStandardItemModel* model=new QStandardItemModel(0,umap.size(),this);

    model->setHorizontalHeaderItem(0,new QStandardItem("name"));
    model->setHorizontalHeaderItem(1,new QStandardItem("symbol"));
    model->setHorizontalHeaderItem(2,new QStandardItem("element"));
    model->setHorizontalHeaderItem(3,new QStandardItem("# protons"));
    model->setHorizontalHeaderItem(4,new QStandardItem("# nucleons"));
    model->setHorizontalHeaderItem(5,new QStandardItem("nuclear spin"));
    model->setHorizontalHeaderItem(6,new QStandardItem("molar mass ("+QString::fromStdString(umap.at("molar_mass"))+")"));
    model->setHorizontalHeaderItem(7,new QStandardItem("state"));
    model->setHorizontalHeaderItem(8,new QStandardItem("abundance ("+QString::fromStdString(umap.at("abundance"))+")"));
    model->setHorizontalHeaderItem(9,new QStandardItem("stable"));
    model->setHorizontalHeaderItem(10,new QStandardItem("half-life ("+QString::fromStdString(umap.at("half_life"))+")"));
    model->setHorizontalHeaderItem(11,new QStandardItem("b coherent ("+QString::fromStdString(umap.at("b_coherent"))+")"));
    model->setHorizontalHeaderItem(12,new QStandardItem("b incoherent ("+QString::fromStdString(umap.at("b_incoherent"))+")"));
    model->setHorizontalHeaderItem(13,new QStandardItem("b+ ("+QString::fromStdString(umap.at("b_plus"))+")"));
    model->setHorizontalHeaderItem(14,new QStandardItem("b- ("+QString::fromStdString(umap.at("b_minus"))+")"));
    model->setHorizontalHeaderItem(15,new QStandardItem("xs coherent ("+QString::fromStdString(umap.at("xs_coherent"))+")"));
    model->setHorizontalHeaderItem(16,new QStandardItem("xs incoherent ("+QString::fromStdString(umap.at("xs_incoherent"))+")"));
    model->setHorizontalHeaderItem(17,new QStandardItem("xs scattering ("+QString::fromStdString(umap.at("xs_scattering"))+")"));
    model->setHorizontalHeaderItem(18,new QStandardItem("xs absorption ("+QString::fromStdString(umap.at("xs_absorption"))+")"));

    int nRows = 0;

    BOOST_FOREACH(const boost::property_tree::ptree::value_type& v, root.get_child("isotopes"))
    {
        if (v.first.compare("isotope")!=0)
            continue;

        QStandardItem* col0 = new QStandardItem(QString::fromStdString(v.second.get<std::string>("<xmlattr>.name")));
        QStandardItem* col1 = new QStandardItem(QString::fromStdString(v.second.get<std::string>("symbol")));
        QStandardItem* col2 = new QStandardItem(QString::fromStdString(v.second.get<std::string>("element")));
        QStandardItem* col3 = new QStandardItem(QString::number(v.second.get<unsigned int>("n_protons")));
        QStandardItem* col4 = new QStandardItem(QString::number(v.second.get<unsigned int>("n_nucleons")));
        QStandardItem* col5 = new QStandardItem(QString::number(v.second.get<double>("nuclear_spin")));
        QStandardItem* col6 = new QStandardItem(QString::number(v.second.get<double>("molar_mass")));
        QStandardItem* col7 = new QStandardItem(QString::fromStdString(v.second.get<std::string>("state")));
        QStandardItem* col8 = new QStandardItem(QString::number(v.second.get<double>("abundance",0.0)));
        QStandardItem* col9 = new QStandardItem(QString::number(v.second.get<bool>("stable")));
        QStandardItem* col10= new QStandardItem(QString::number(v.second.get<double>("half_life",std::numeric_limits<double>::infinity())));
        QStandardItem* col11= new QStandardItem(QString::fromStdString(SX::Utils::complexToString(v.second.get<std::complex<double>>("b_coherent"))));
        QStandardItem* col12= new QStandardItem(QString::fromStdString(SX::Utils::complexToString(v.second.get<std::complex<double>>("b_incoherent"))));
        QStandardItem* col13= new QStandardItem(QString::fromStdString(SX::Utils::complexToString(v.second.get<std::complex<double>>("b_plus",0))));
        QStandardItem* col14= new QStandardItem(QString::fromStdString(SX::Utils::complexToString(v.second.get<std::complex<double>>("b_minus",0))));
        QStandardItem* col15= new QStandardItem(QString::number(v.second.get<double>("xs_coherent",0.0)));
        QStandardItem* col16= new QStandardItem(QString::number(v.second.get<double>("xs_incoherent",0.0)));
        QStandardItem* col17= new QStandardItem(QString::number(v.second.get<double>("xs_scattering",0.0)));
        QStandardItem* col18= new QStandardItem(QString::number(v.second.get<double>("xs_absorption",0.0)));

        model->setItem(nRows,0,col0);
        model->setItem(nRows,1,col1);
        model->setItem(nRows,2,col2);
        model->setItem(nRows,3,col3);
        model->setItem(nRows,4,col4);
        model->setItem(nRows,5,col5);
        model->setItem(nRows,6,col6);
        model->setItem(nRows,7,col7);
        model->setItem(nRows,8,col8);
        model->setItem(nRows,9,col9);
        model->setItem(nRows,10,col10);
        model->setItem(nRows,11,col11);
        model->setItem(nRows,12,col12);
        model->setItem(nRows,13,col13);
        model->setItem(nRows,14,col14);
        model->setItem(nRows,15,col15);
        model->setItem(nRows,16,col16);
        model->setItem(nRows,17,col17);
        model->setItem(nRows,18,col18);

        model->setRowCount(++nRows);

    }

    ui->isotopeDatabaseView->setModel(model);

    resize(1000,500);

    ui->isotopeDatabaseView->show();

}

IsotopeDatabaseDialog::~IsotopeDatabaseDialog()
{
    delete ui;
}



