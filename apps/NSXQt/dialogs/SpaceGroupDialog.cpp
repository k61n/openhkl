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
#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/SpaceGroupSymbols.h>
#include "IData.h"

#include <nsxlib/crystal/RFactor.h>

using namespace std;
using namespace SX::Crystal;

SpaceGroupDialog::SpaceGroupDialog(std::vector<std::shared_ptr<SX::Data::IData>> numors, QWidget *parent):
    QDialog(parent),
    ui(new Ui::SpaceGroupDialog),
    _numors(numors),
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
    using SX::Crystal::sptrPeak3D;
    using std::tuple;

    SpaceGroupSymbols* spaceGroupSymbols = SpaceGroupSymbols::Instance();
    vector<string> symbols = spaceGroupSymbols->getAllSymbols();

    vector<array<double, 3>> hkls;
    vector<sptrPeak3D> peak_list;
    vector<vector<sptrPeak3D>> peak_equivs;

    //auto numors = getSelectedNumors();

    if ( _numors.size()  == 0) {
        qDebug() << "Need at least one numor to find space group!";
        return;
    }

    qDebug() << "Retrieving reflection list for space group calculation...";

    for (auto& numor: _numors) {
        auto peaks = numor->getPeaks();

        for (sptrPeak3D peak : peaks)
        {
            Eigen::RowVector3i hkl = peak->getIntegerMillerIndices();

            if (peak->isSelected() && !peak->isMasked()) {
                hkls.push_back(array<double, 3>{{double(hkl[0]), double(hkl[1]), double(hkl[2])}});
                peak_list.push_back(peak);
            }
        }
    }

    if (hkls.size() == 0) {
        qDebug() << "Need to have indexed peaks in order to find space group!";
        return;
    }

    // todo: how to we handle multiple samples??
    std::shared_ptr<SX::Instrument::Sample> sample = _numors[0]->getDiffractometer()->getSample();

    if (!sample) {
        qDebug() << "Need to have a sample in order to find space group!";
        return;
    }

    _groups.clear();

    qDebug() << "Evaluating space groups based on " << hkls.size() << " peaks";

    // int count = 0;
    // int total = symbols.size();

    for (auto& symbol: symbols) {

        SpaceGroup group = SpaceGroup(symbol);
        std::string bravais = sample->getUnitCell(0)->getBravaisTypeSymbol();

        // space group not compatible with bravais type
        // todo: what about multiple crystals??
        if (group.getBravaisTypeSymbol() != bravais)
            continue;

//        if ( group.fractionExtinct(hkls) > 0.0)
//            continue;

        //peak_equivs = group.findEquivalences(peak_list);
        //RFactor rfactor(peak_equivs);

        tuple<string, double> entry;

        get<0>(entry) = symbol;
        get<1>(entry) = 100.0*(1-group.fractionExtinct(hkls));
        //get<2>(entry) = rfactor.Rmerge();
        //get<3>(entry) = rfactor.Rmeas();
        //get<4>(entry) = rfactor.Rpim();

        // group is compatible with observed reflections, so add it to list
        _groups.push_back(entry);

//        qDebug() << "    evaluating group " << (++count) << " of " << total
//                 << ": " << symbol.c_str() << " "
//                 << rfactor.Rmerge() << " " << rfactor.Rmeas() << " " << rfactor.Rpim();

    }

    auto compare_fn = [](const tuple<string, double>& a,
            const tuple<string, double>& b) -> bool
    {
        double quality_a = get<1>(a);
        double quality_b = get<1>(b);

        // sort first by quality
        if (quality_a != quality_b)
            return quality_a > quality_b;

        // otherwise we sort by properties of the groups
        SX::Crystal::SpaceGroup grp_a(get<0>(a)), grp_b(get<0>(b));

        // sort by group ID
        return grp_a.getID() < grp_b.getID();
    };

    std::sort(_groups.begin(), _groups.end(), compare_fn);

    qDebug() << "Done evaluating space groups.";
}

void SpaceGroupDialog::buildTable()
{
    QStandardItemModel* model = new QStandardItemModel(_groups.size(), 2, this);

    model->setHorizontalHeaderItem(0,new QStandardItem("Symbol"));
    model->setHorizontalHeaderItem(1,new QStandardItem("Group ID"));
    model->setHorizontalHeaderItem(2,new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(3,new QStandardItem("Agreement"));
    //model->setHorizontalHeaderItem(4,new QStandardItem("R merge"));
    //model->setHorizontalHeaderItem(5,new QStandardItem("R meas"));
    //model->setHorizontalHeaderItem(6,new QStandardItem("R pim"));

    unsigned int row = 0;

    // Display solutions
    for (auto&& item: _groups) {

        const std::string& symbol = get<0>(item);
        double agreement = get<1>(item);
        //double Rmerge = get<2>(item);
        //double Rmeas = get<3>(item);
        //double Rpim = get<4>(item);
        SX::Crystal::SpaceGroup grp(symbol);

        QStandardItem* col0 = new QStandardItem(symbol.c_str());
        QStandardItem* col1 = new QStandardItem(to_string(grp.getID()).c_str());
        QStandardItem* col2 = new QStandardItem(grp.getBravaisTypeSymbol().c_str());
        QStandardItem* col3 = new QStandardItem(to_string((int)agreement).c_str());
        //QStandardItem* col4 = new QStandardItem(to_string(Rmerge).c_str());
        //QStandardItem* col5 = new QStandardItem(to_string(Rmeas).c_str());
        //QStandardItem* col6 = new QStandardItem(to_string(Rpim).c_str());

        model->setItem(row,0,col0);
        model->setItem(row,1,col1);
        model->setItem(row,2,col2);
        model->setItem(row,3,col3);
        //model->setItem(row,4,col4);
        //model->setItem(row,5,col5);
        //model->setItem(row,6,col6);

        ++row;
    }

   ui->tableView->setModel(model);
}

void SpaceGroupDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    _selectedGroup = get<0>(_groups[index.row()]);
    QMessageBox* box = new QMessageBox(this);
    box->setText(QString("Setting space group to ") + _selectedGroup.c_str());

    // todo: how to handle multiple samples and/or multiple unit cells???
    for (shared_ptr<SX::Data::IData> numor: _numors) {
        std::shared_ptr<SX::Instrument::Sample> sample = numor->getDiffractometer()->getSample();
        sample->getUnitCell(0)->setSpaceGroup(_selectedGroup);
    }

    box->exec();
}
