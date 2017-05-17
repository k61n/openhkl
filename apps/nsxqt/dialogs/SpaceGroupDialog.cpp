// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include <array>
#include <tuple>

#include <Eigen/Core>

#include <QImage>
#include <QList>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTreeView>

#include <nsxlib/data/IData.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/crystal/RFactor.h>
#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/SpaceGroupSymbols.h>

#include "SpaceGroupDialog.h"
#include "ui_SpaceGroupDialog.h"

SpaceGroupDialog::SpaceGroupDialog(std::vector<std::shared_ptr<nsx::DataSet>> numors, QWidget *parent):
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
    nsx::SpaceGroupSymbols* spaceGroupSymbols = nsx::SpaceGroupSymbols::Instance();
    std::vector<std::string> symbols = spaceGroupSymbols->getAllSymbols();

    std::vector<std::array<double, 3>> hkls;
    std::vector<nsx::sptrPeak3D> peak_list;
    std::vector<std::vector<sptrPeak3D>> peak_equivs;

    if ( _numors.size()  == 0) {
        qDebug() << "Need at least one numor to find space group!";
        return;
    }

    qDebug() << "Retrieving reflection list for space group calculation...";

    for (auto& numor: _numors) {
        auto peaks = numor->getPeaks();

        for (nsx::sptrPeak3D peak : peaks)
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
    std::shared_ptr<nsx::Sample> sample = _numors[0]->getDiffractometer()->getSample();

    if (!sample) {
        qDebug() << "Need to have a sample in order to find space group!";
        return;
    }

    _groups.clear();

    qDebug() << "Evaluating space groups based on " << hkls.size() << " peaks";

    for (auto& symbol: symbols) {

        nsx::SpaceGroup group = nsx::SpaceGroup(symbol);
        std::string bravais = sample->getUnitCell(0)->getBravaisTypeSymbol();

        // space group not compatible with bravais type
        // todo: what about multiple crystals??
        if (group.getBravaisTypeSymbol() != bravais)
            continue;

        tuple<string, double> entry;

        std::get<0>(entry) = symbol;
        std::get<1>(entry) = 100.0*(1-group.fractionExtinct(hkls));

        // group is compatible with observed reflections, so add it to list
        _groups.push_back(entry);
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
        nsx::SpaceGroup grp_a(get<0>(a)), grp_b(get<0>(b));

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

    unsigned int row = 0;

    // Display solutions
    for (auto&& item: _groups) {

        const std::string& symbol = get<0>(item);
        double agreement = get<1>(item);
        nsx::SpaceGroup grp(symbol);

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
    for (shared_ptr<nsx::DataSet> numor: _numors) {
        std::shared_ptr<nsx::Sample> sample = numor->getDiffractometer()->getSample();
        sample->getUnitCell(0)->setSpaceGroup(_selectedGroup);
    }

    box->exec();
}
