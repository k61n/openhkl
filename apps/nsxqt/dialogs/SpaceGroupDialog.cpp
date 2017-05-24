// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include <array>
#include <tuple>

#include <Eigen/Core>

#include <QDebug>
#include <QImage>
#include <QList>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTreeView>

#include <nsxlib/crystal/CrystalTypes.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/RFactor.h>
#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/SpaceGroupSymbols.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Sample.h>

#include "SpaceGroupDialog.h"
#include "ui_SpaceGroupDialog.h"

SpaceGroupDialog::SpaceGroupDialog(nsx::DataList numors, QWidget *parent):
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
}

std::string SpaceGroupDialog::getSelectedGroup()
{
    return _selectedGroup;
}


void SpaceGroupDialog::evaluateSpaceGroups()
{
    nsx::SpaceGroupSymbols* spaceGroupSymbols = nsx::SpaceGroupSymbols::Instance();
    std::vector<std::string> symbols = spaceGroupSymbols->getAllSymbols();

    std::vector<std::array<double, 3>> hkls;
    nsx::PeakList peak_list;
    std::vector<nsx::PeakList> peak_equivs;

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
                hkls.push_back(std::array<double, 3>{{double(hkl[0]), double(hkl[1]), double(hkl[2])}});
                peak_list.push_back(peak);
            }
        }
    }

    if (hkls.size() == 0) {
        qDebug() << "Need to have indexed peaks in order to find space group!";
        return;
    }

    // todo: how to we handle multiple samples??
    auto sample = _numors[0]->getDiffractometer()->getSample();

    if (!sample) {
        qDebug() << "Need to have a sample in order to find space group!";
        return;
    }

    _groups.clear();

    qDebug() << "Evaluating " << symbols.size() << " space groups based on " << hkls.size() << " peaks";

    for (auto& symbol: symbols) {

        nsx::SpaceGroup group = nsx::SpaceGroup(symbol);
        std::string bravais = sample->getUnitCell(0)->getBravaisTypeSymbol();

        // space group not compatible with bravais type
        // todo: what about multiple crystals??
        if (group.getBravaisTypeSymbol() != bravais)
            continue;

        std::tuple<std::string, double> entry;

        std::get<0>(entry) = symbol;
        std::get<1>(entry) = 100.0*(1-group.fractionExtinct(hkls));

        // group is compatible with observed reflections, so add it to list
        _groups.push_back(entry);
    }

    auto compare_fn = [](const std::tuple<std::string, double>& a, const std::tuple<std::string, double>& b) -> bool
    {
        double quality_a = std::get<1>(a);
        double quality_b = std::get<1>(b);

        // sort first by quality
        if (quality_a != quality_b)
            return quality_a > quality_b;

        // otherwise we sort by properties of the groups
        nsx::SpaceGroup grp_a(std::get<0>(a)), grp_b(std::get<0>(b));

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

        const std::string& symbol = std::get<0>(item);
        double agreement = std::get<1>(item);
        nsx::SpaceGroup grp(symbol);

        QStandardItem* col0 = new QStandardItem(QString::fromStdString(symbol));
        QStandardItem* col1 = new QStandardItem(QString::number(grp.getID()));
        QStandardItem* col2 = new QStandardItem(QString::fromStdString(grp.getBravaisTypeSymbol()));
        QStandardItem* col3 = new QStandardItem(QString::number(agreement));

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
    _selectedGroup = std::get<0>(_groups[index.row()]);
    QMessageBox* box = new QMessageBox(this);
    box->setText(QString("Setting space group to ") + _selectedGroup.c_str());

    // todo: how to handle multiple samples and/or multiple unit cells???
    for (auto numor: _numors) {
        auto sample = numor->getDiffractometer()->getSample();
        sample->getUnitCell(0)->setSpaceGroup(_selectedGroup);
    }

    box->exec();
}
