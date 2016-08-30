// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "SpaceGroupDialog.h"
#include "ui_SpaceGroupDialog.h"

#include <QImage>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QList>
#include <QString>

#include <QModelIndex>

#include <Eigen/Core>
#include <QDebug>

#include <iostream>

#include <QMessageBox>

#include "Sample.h"
#include "SpaceGroup.h"
#include "SpaceGroupSymbols.h"
#include "IData.h"

using namespace std;

SpaceGroupDialog::SpaceGroupDialog(std::vector<std::shared_ptr<SX::Data::IData>> numors, QWidget *parent) :
    _numors(numors),
    QDialog(parent),
    ui(new Ui::SpaceGroupDialog),
    _selectedGroup("")
{
    ui->setupUi(this);

    // Make sure that the user can not edit the content of the table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Selection of a cellin the table select the whole line.
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    evaluateSpaceGroups();
    buildTable();

    //ui->treeView->setModel(_peakFindModel);
}

SpaceGroupDialog::~SpaceGroupDialog()
{
    delete ui;
    // this should be handled by Qt. check with valgrind?
    // delete _peakFindModel;
}

string SpaceGroupDialog::getSelectedGroup()
{
    return _selectedGroup;
}


void SpaceGroupDialog::evaluateSpaceGroups()
{
    // testing for now
    using SX::Crystal::SpaceGroup;
    using SX::Crystal::SpaceGroupSymbols;
    using std::vector;
    using std::string;
    using std::map;
    using std::array;
    using SX::Crystal::Peak3D;
    using std::tuple;

    SpaceGroupSymbols* spaceGroupSymbols = SpaceGroupSymbols::Instance();

    vector<string> symbols = spaceGroupSymbols->getAllSymbols();

    vector<array<double, 3>> hkls;

    //auto numors = getSelectedNumors();

    if ( _numors.size()  == 0) {
        qDebug() << "Need at least one numor to find space group!";
        return;
    }

    qDebug() << "Retrieving reflection list for space group calculation...";

    for (auto& numor: _numors) {
        auto peaks = numor->getPeaks();

        for ( Peak3D* peak : peaks) {
            Eigen::RowVector3d hkl = peak->getMillerIndices();
            hkls.push_back(array<double, 3>{hkl[0], hkl[1], hkl[2]});
        }
    }

    // todo: how to we handle multiple samples??
    std::shared_ptr<SX::Instrument::Sample> sample = _numors[0]->getDiffractometer()->getSample();
    _groups.clear();

    for (auto& symbol: symbols) {
        SpaceGroup group = SpaceGroup(symbol);
        std::string bravais = sample->getUnitCell(0)->getBravaisTypeSymbol();

        // space group not compatible with bravais type
        // todo: what about multiple crystals??
        if (group.getBravaisTypeSymbol() != bravais)
            continue;

        if ( group.fractionExtinct(hkls) > 0.0)
            continue;

        // group is compatible with observed reflections, so add it to list
        _groups.push_back(make_pair(symbol, 100.0*(1-group.fractionExtinct(hkls))));

    }

    auto compare_fn = [](const pair<string, double>& a, const pair<string, double>& b) -> bool
    {
        // first sort by quality
        if (a.second > b.second)
            return true;
        else if (a.second < b.second)
            return false;

        // otherwise we sort by properties of the groups
        SX::Crystal::SpaceGroup grp_a(a.first), grp_b(b.first);

        // sort by size
        return grp_a.getGenerators().size() > grp_b.getGenerators().size();
    };

    std::sort(_groups.begin(), _groups.end(), compare_fn);

//    qDebug() << "FOUND MATCHES:";

//    for (auto&& grp: _groups) {
//        qDebug() << grp.getSymbol().c_str()
//                 << " " << grp.getGroupElements().size()
//                 << " " << grp.getBravaisTypeSymbol().c_str();
//    }
}

void SpaceGroupDialog::buildTable()
{
    QStandardItemModel* model = new QStandardItemModel(_groups.size(), 4, this);

    model->setHorizontalHeaderItem(0,new QStandardItem("Symbol"));
    model->setHorizontalHeaderItem(1,new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(2,new QStandardItem("Size"));
    model->setHorizontalHeaderItem(3,new QStandardItem("Agreement"));

    unsigned int row = 0;

    // Display solutions
    for (auto&& item: _groups) {

        const std::string& symbol = item.first;
        double agreement = item.second;
        SX::Crystal::SpaceGroup grp(symbol);

        QStandardItem* col0 = new QStandardItem(symbol.c_str());
        QStandardItem* col1 = new QStandardItem(grp.getBravaisTypeSymbol().c_str());
        QStandardItem* col2 = new QStandardItem(to_string(grp.getGenerators().size()).c_str());
        QStandardItem* col3 = new QStandardItem(to_string((int)agreement).c_str());

        model->setItem(row,0,col0);
        model->setItem(row,1,col1);
        model->setItem(row,2,col2);
        model->setItem(row,3,col3);

        ++row;
    }

   ui->tableView->setModel(model);
}

void SpaceGroupDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    _selectedGroup = _groups[index.row()].first;
    QMessageBox* box = new QMessageBox(this);
    box->setText(QString("Setting space group to ") + _selectedGroup.c_str());

    // todo: how to handle multiple samples and/or multiple unit cells???
    for (shared_ptr<SX::Data::IData> numor: _numors) {
        std::shared_ptr<SX::Instrument::Sample> sample = numor->getDiffractometer()->getSample();
        sample->getUnitCell(0)->setSpaceGroup(_selectedGroup);
    }

    box->exec();
}
