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
#include "core/crystal/UnitCell.h"

UnitCellWidget::UnitCellWidget(nsx::sptrUnitCell cell, const QString& name)
    : QcrWidget {name}, unitCell_ {cell}
{
    nsx::UnitCellCharacter character = unitCell_->character();
    // layout...
    QVBoxLayout* whole = new QVBoxLayout(this);
    QGroupBox* unitcellparams = new QGroupBox("Unit cell parameters", this);
    QGridLayout* unitgrid = new QGridLayout(unitcellparams);
    unitgrid->addWidget(new QLabel("Bravais type"), 0, 0, 1, 1);
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
    QcrLineDisplay* bravais = new QcrLineDisplay([]() { return QString(); });
    unitgrid->addWidget(bravais, 0, 1, 1, 1);
    QcrDoubleSpinBox* a =
        new QcrDoubleSpinBox("adhoc_readA", new QcrCell<double>(character.a), 8, 4);
    QcrDoubleSpinBox* b =
        new QcrDoubleSpinBox("adhoc_readB", new QcrCell<double>(character.b), 8, 4);
    QcrDoubleSpinBox* c =
        new QcrDoubleSpinBox("adhoc_readC", new QcrCell<double>(character.c), 8, 4);
    QcrDoubleSpinBox* alpha =
        new QcrDoubleSpinBox("adhoc_readAlpha", new QcrCell<double>(character.alpha), 8, 4);
    QcrDoubleSpinBox* beta =
        new QcrDoubleSpinBox("adhoc_readBeta", new QcrCell<double>(character.beta), 8, 4);
    QcrDoubleSpinBox* gamma =
        new QcrDoubleSpinBox("adhoc_readGamma", new QcrCell<double>(character.gamma), 8, 4);
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
    whole->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    show();
}
