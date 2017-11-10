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

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/RFactor.h>
#include <nsxlib/Sample.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/UnitCell.h>

#include "SpaceGroupDialog.h"
#include "ui_SpaceGroupDialog.h"

SpaceGroupDialog::SpaceGroupDialog(const nsx::PeakSet& peaks, QWidget *parent):
    QDialog(parent),
    ui(new Ui::SpaceGroupDialog),
    _peaks(peaks),
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
    auto&& symbols = nsx::SpaceGroup::symbols();

    std::vector<std::array<double, 3>> hkls;
    nsx::PeakList peak_list;
    std::vector<nsx::PeakList> peak_equivs;

    if ( _peaks.size()  == 0) {
        nsx::error() << "Need at least one peak to find space group!";
        return;
    }

    nsx::info() << "Retrieving reflection list for space group calculation...";
    _cells.clear();

    for (nsx::sptrPeak3D peak : _peaks) {
        _cells.insert(peak->getActiveUnitCell());
        auto cell = peak->getActiveUnitCell();
        Eigen::RowVector3i hkl = cell->getIntegerMillerIndices(peak->getQ());

        if (peak->isSelected() && !peak->isMasked()) {
            hkls.push_back(std::array<double, 3>{{double(hkl[0]), double(hkl[1]), double(hkl[2])}});
            peak_list.push_back(peak);
        }
    }

    if (_cells.size() != 1) {
        nsx::error() << "ERROR: Only one unit cell is supported at this time";
        return;
    }


    if (hkls.size() == 0) {
        nsx::error() << "Need to have indexed peaks in order to find space group!";
        return;
    }

    _groups.clear();

    nsx::info() << "Evaluating " << symbols.size() << " space groups based on " << hkls.size() << " peaks";

    std::string bravais = (*_cells.begin())->getBravaisTypeSymbol();

    for (auto& symbol: symbols) {

        nsx::SpaceGroup group = nsx::SpaceGroup(symbol);       

        // space group not compatible with bravais type
        // todo: what about multiple crystals??
        if (group.bravaisTypeSymbol() != bravais)
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
        return grp_a.id() < grp_b.id();
    };

    std::sort(_groups.begin(), _groups.end(), compare_fn);

    nsx::info() << "Done evaluating space groups.";
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
        QStandardItem* col1 = new QStandardItem(QString::number(grp.id()));
        QStandardItem* col2 = new QStandardItem(QString::fromStdString(grp.bravaisTypeSymbol()));
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
    for (auto cell: _cells) {
        cell->setSpaceGroup(_selectedGroup);
    }

    box->exec();
}
