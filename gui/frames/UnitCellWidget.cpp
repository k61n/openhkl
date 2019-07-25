//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/UnitCellWidget.cpp
//! @brief     Implements class UnitCellWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/UnitCellWidget.h"

#include "gui/models/Session.h"
#include "base/utils/Units.h"
#include "core/peak/Peak3D.h"
#include "core/analyse/PeakFilter.h"
#include "tables/crystal/UnitCell.h"
#include <QStandardItemModel>

UnitCellWidget::UnitCellWidget(nsx::sptrUnitCell cell, const QString& name)
    : QcrWidget {name}, unitCell_ {cell}, wasSpaceGroupSet {false}
{
    nsx::UnitCellCharacter character = unitCell_->character();
    // layout...
    QVBoxLayout* whole = new QVBoxLayout(this);
    QGroupBox* unitcellparams = new QGroupBox("Unit cell parameters", this);
    QGridLayout* unitgrid = new QGridLayout(unitcellparams);
    unitgrid->addWidget(new QLabel("Bravais type:  "), 0, 0, 1, 1);
    unitgrid->addWidget(new QLabel("a (" + QString(QChar(0x212B)) + ")"), 1, 0, 1, 1);
    unitgrid->addWidget(
        new QLabel(QString(QChar(0x03B1)) + "(" + QString(QChar(0x00B0)) + ")"), 2, 0, 1, 1);
    unitgrid->addWidget(new QLabel("b (" + QString(QChar(0x212B)) + ")"), 1, 2, 1, 1);
    unitgrid->addWidget(
        new QLabel(QString(QChar(0x03B2)) + "(" + QString(QChar(0x00B0)) + ")"), 2, 2, 1, 1);
    unitgrid->addWidget(new QLabel("c (" + QString(QChar(0x212B)) + ")"), 1, 4, 1, 1);
    unitgrid->addWidget(
        new QLabel(QString(QChar(0x03B3)) + "(" + QString(QChar(0x00B0)) + ")"), 2, 4, 1, 1);
    unitgrid->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 6);
    QLabel* bravais = new QLabel(QString::fromStdString(unitCell_->bravaisTypeSymbol()));
    unitgrid->addWidget(bravais, 0, 1, 1, 1);
    QcrDoubleSpinBox* a =
        new QcrDoubleSpinBox("adhoc_readA", new QcrCell<double>(character.a), 8, 4);
    a->setReadOnly(true);
    QcrDoubleSpinBox* b =
        new QcrDoubleSpinBox("adhoc_readB", new QcrCell<double>(character.b), 8, 4);
    b->setReadOnly(true);
    QcrDoubleSpinBox* c =
        new QcrDoubleSpinBox("adhoc_readC", new QcrCell<double>(character.c), 8, 4);
    c->setReadOnly(true);
    QcrDoubleSpinBox* alpha =
        new QcrDoubleSpinBox("adhoc_readAlpha",
                             new QcrCell<double>(character.alpha / nsx::deg), 8, 4);
    alpha->setReadOnly(true);
    QcrDoubleSpinBox* beta =
        new QcrDoubleSpinBox("adhoc_readBeta",
                             new QcrCell<double>(character.beta / nsx::deg), 8, 4);
    beta->setReadOnly(true);
    QcrDoubleSpinBox* gamma =
        new QcrDoubleSpinBox("adhoc_readGamma",
                             new QcrCell<double>(character.gamma / nsx::deg), 8, 4);
    gamma->setReadOnly(true);
    unitgrid->addWidget(a, 1, 1, 1, 1);
    unitgrid->addWidget(b, 1, 3, 1, 1);
    unitgrid->addWidget(c, 1, 5, 1, 1);
    unitgrid->addWidget(alpha, 2, 1, 1, 1);
    unitgrid->addWidget(beta, 2, 3, 1, 1);
    unitgrid->addWidget(gamma, 2, 5, 1, 1);
    whole->addWidget(unitcellparams);
    QHBoxLayout* horizontal = new QHBoxLayout();
    QGroupBox* matrixb = new QGroupBox("B matrix (row form)");
    QGridLayout* bmatrix = new QGridLayout(matrixb);
    Eigen::Matrix3d matrix_u = unitCell_->orientation();
    Eigen::Matrix3d matrix_b = unitCell_->reciprocalBasis() * matrix_u;
    QcrDoubleSpinBox* b00 =
        new QcrDoubleSpinBox("adhoc_b00", new QcrCell<double>(matrix_b(0, 0)), 8, 4);
    QcrDoubleSpinBox* b01 =
        new QcrDoubleSpinBox("adhoc_b01", new QcrCell<double>(matrix_b(0, 1)), 8, 4);
    QcrDoubleSpinBox* b02 =
        new QcrDoubleSpinBox("adhoc_b02", new QcrCell<double>(matrix_b(0, 2)), 8, 4);
    QcrDoubleSpinBox* b10 =
        new QcrDoubleSpinBox("adhoc_b10", new QcrCell<double>(matrix_b(1, 0)), 8, 4);
    QcrDoubleSpinBox* b11 =
        new QcrDoubleSpinBox("adhoc_b11", new QcrCell<double>(matrix_b(1, 1)), 8, 4);
    QcrDoubleSpinBox* b12 =
        new QcrDoubleSpinBox("adhoc_b12", new QcrCell<double>(matrix_b(1, 2)), 8, 4);
    QcrDoubleSpinBox* b20 =
        new QcrDoubleSpinBox("adhoc_b20", new QcrCell<double>(matrix_b(2, 0)), 8, 4);
    QcrDoubleSpinBox* b21 =
        new QcrDoubleSpinBox("adhoc_b21", new QcrCell<double>(matrix_b(2, 1)), 8, 4);
    QcrDoubleSpinBox* b22 =
        new QcrDoubleSpinBox("adhoc_b22", new QcrCell<double>(matrix_b(2, 2)), 8, 4);
    b00->setReadOnly(true);
    b01->setReadOnly(true);
    b02->setReadOnly(true);
    b10->setReadOnly(true);
    b11->setReadOnly(true);
    b12->setReadOnly(true);
    b20->setReadOnly(true);
    b21->setReadOnly(true);
    b22->setReadOnly(true);
    bmatrix->addWidget(b00, 0, 0, 1, 1);
    bmatrix->addWidget(b01, 0, 1, 1, 1);
    bmatrix->addWidget(b02, 0, 2, 1, 1);
    bmatrix->addWidget(b10, 1, 0, 1, 1);
    bmatrix->addWidget(b11, 1, 1, 1, 1);
    bmatrix->addWidget(b12, 1, 2, 1, 1);
    bmatrix->addWidget(b20, 2, 0, 1, 1);
    bmatrix->addWidget(b21, 2, 1, 1, 1);
    bmatrix->addWidget(b22, 2, 2, 1, 1);
    horizontal->addWidget(matrixb);
    QGroupBox* matrixu = new QGroupBox("U matrix (row form)");
    QGridLayout* umatrix = new QGridLayout(matrixu);
    Eigen::Matrix3d u = matrix_u.inverse();
    QcrDoubleSpinBox* u00 = new QcrDoubleSpinBox("adhoc_u00", new QcrCell<double>(u(0, 0)), 8, 4);
    QcrDoubleSpinBox* u01 = new QcrDoubleSpinBox("adhoc_u01", new QcrCell<double>(u(0, 1)), 8, 4);
    QcrDoubleSpinBox* u02 = new QcrDoubleSpinBox("adhoc_u02", new QcrCell<double>(u(0, 2)), 8, 4);
    QcrDoubleSpinBox* u10 = new QcrDoubleSpinBox("adhoc_u10", new QcrCell<double>(u(1, 0)), 8, 4);
    QcrDoubleSpinBox* u11 = new QcrDoubleSpinBox("adhoc_u11", new QcrCell<double>(u(1, 1)), 8, 4);
    QcrDoubleSpinBox* u12 = new QcrDoubleSpinBox("adhoc_u12", new QcrCell<double>(u(1, 2)), 8, 4);
    QcrDoubleSpinBox* u20 = new QcrDoubleSpinBox("adhoc_u20", new QcrCell<double>(u(2, 0)), 8, 4);
    QcrDoubleSpinBox* u21 = new QcrDoubleSpinBox("adhoc_u21", new QcrCell<double>(u(2, 1)), 8, 4);
    QcrDoubleSpinBox* u22 = new QcrDoubleSpinBox("adhoc_u22", new QcrCell<double>(u(2, 2)), 8, 4);
    u00->setReadOnly(true);
    u01->setReadOnly(true);
    u02->setReadOnly(true);
    u10->setReadOnly(true);
    u11->setReadOnly(true);
    u12->setReadOnly(true);
    u20->setReadOnly(true);
    u21->setReadOnly(true);
    u22->setReadOnly(true);
    umatrix->addWidget(u00, 0, 0, 1, 1);
    umatrix->addWidget(u01, 0, 1, 1, 1);
    umatrix->addWidget(u02, 0, 2, 1, 1);
    umatrix->addWidget(u10, 1, 0, 1, 1);
    umatrix->addWidget(u11, 1, 1, 1, 1);
    umatrix->addWidget(u12, 1, 2, 1, 1);
    umatrix->addWidget(u20, 2, 0, 1, 1);
    umatrix->addWidget(u21, 2, 1, 1, 1);
    umatrix->addWidget(u22, 2, 2, 1, 1);
    horizontal->addWidget(matrixu);
    horizontal->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    whole->addLayout(horizontal);
    spaceGroupView = new QTableView(this);
    whole->addWidget(spaceGroupView);
    whole->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    evaluateSpaceGroups();
    show();
}

void UnitCellWidget::evaluateSpaceGroups()
{
    nsx::PeakList list;
    nsx::PeakFilter filter;

    list = filter.enabled(gSession->selectedExperiment()->getPeakList(unitCell_), true);
    list = filter.indexed(list, *unitCell_, unitCell_->indexingTolerance());

    nsx::MillerIndexList hkls;
    for (nsx::sptrPeak3D peak : list)
        hkls.emplace_back(nsx::MillerIndex(peak->q(), *unitCell_).rowVector());
    std::vector<std::string> compatibleSpaceGroups = unitCell_->compatibleSpaceGroups();
    std::vector<std::pair<std::string, double>> groups;
    for (std::string& symbol : compatibleSpaceGroups) {
        nsx::SpaceGroup spacegroup = nsx::SpaceGroup(symbol);

        std::pair<std::string, double> entry =
            std::make_pair(symbol, 100.0 * (1 - spacegroup.fractionExtinct(hkls)));

        // group is compatible with observed reflections, so add it to list
        groups.push_back(entry);
    }

    auto compare_fn = [](const std::pair<std::string, double>& a,
                         const std::pair<std::string, double>& b) -> bool {
        double quality_a = a.second;
        double quality_b = b.second;

        // sort first by quality
        if (quality_a != quality_b)
            return quality_a > quality_b;

        // otherwise we sort by properties of the groups
        nsx::SpaceGroup grp_a(a.first), grp_b(b.first);

        // sort by group ID
        return grp_a.id() < grp_b.id();
    };

    std::sort(groups.begin(), groups.end(), compare_fn);

    qDebug() << "Done evaluating space groups";

    QStandardItemModel* model = new QStandardItemModel(groups.size(), 2, this);

    model->setHorizontalHeaderItem(0, new QStandardItem("Symbol"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Group ID"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Bravais Type"));
    model->setHorizontalHeaderItem(3, new QStandardItem("Agreement"));

    unsigned int row = 0;

    // Display solutions
    for (auto&& item : groups) {
        const std::string& symbol = std::get<0>(item);
        double agreement = std::get<1>(item);
        nsx::SpaceGroup grp(symbol);

        QStandardItem* col0 = new QStandardItem(QString::fromStdString(symbol));
        QStandardItem* col1 = new QStandardItem(QString::number(grp.id()));
        QStandardItem* col2 = new QStandardItem(QString::fromStdString(grp.bravaisTypeSymbol()));
        QStandardItem* col3 = new QStandardItem(QString::number(agreement));

        model->setItem(row, 0, col0);
        model->setItem(row, 1, col1);
        model->setItem(row, 2, col2);
        model->setItem(row, 3, col3);

        ++row;
    }
    spaceGroupOne = std::get<0>(groups[0]);

    spaceGroupView->setModel(model);
    spaceGroupView->setToolTip("Double click to select space group");

    QObject::connect(spaceGroupView, &QTableView::doubleClicked, [=](const QModelIndex& index) {
        std::string spaceGroupSymbol = std::get<0>(groups[index.row()]);
        unitCell_->setSpaceGroup(spaceGroupSymbol);
        wasSpaceGroupSet = true;
        qDebug() << "UnitCellWidget: set SpaceGroup of UnitCell "
                 << QString::fromStdString(unitCell_->name()) << " to "
                 << QString::fromStdString(spaceGroupSymbol);
    });
}

void UnitCellWidget::setSpaceGroup()
{
    unitCell_->setSpaceGroup(spaceGroupOne);
    qDebug() << "UnitCellWidget: set SpaceGroup of UnitCell "
             << QString::fromStdString(unitCell_->name()) << " to "
             << QString::fromStdString(spaceGroupOne);
}
