//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/WidgetUnitCell.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <core/UnitCell.h>
#include <core/Units.h>

#include "WidgetUnitCell.h"

#include "ui_WidgetUnitCell.h"

WidgetUnitCell::WidgetUnitCell(nsx::sptrUnitCell unit_cell)
    : _ui(new Ui::WidgetUnitCell), _unit_cell(unit_cell)
{
    _ui->setupUi(this);

    auto character = _unit_cell->character();

    _ui->label_a->setText("a (" + QString(QChar(0x212B)) + ")");
    _ui->label_b->setText("b (" + QString(QChar(0x212B)) + ")");
    _ui->label_c->setText("c (" + QString(QChar(0x212B)) + ")");

    _ui->label_alpha->setText(QString(QChar(0x03B1)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_beta->setText(QString(QChar(0x03B2)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_gamma->setText(QString(QChar(0x03B3)) + "(" + QString(QChar(0x00B0)) + ")");

    _ui->a->setValue(character.a);
    _ui->b->setValue(character.b);
    _ui->c->setValue(character.c);

    _ui->alpha->setValue(character.alpha / nsx::deg);
    _ui->beta->setValue(character.beta / nsx::deg);
    _ui->gamma->setValue(character.gamma / nsx::deg);

    _ui->bravais->setText(QString::fromStdString(_unit_cell->bravaisTypeSymbol()));

    Eigen::Matrix3d u_matrix = _unit_cell->orientation();

    Eigen::Matrix3d b_matrix = _unit_cell->reciprocalBasis() * u_matrix;
    _ui->b00->setValue(b_matrix(0, 0));
    _ui->b01->setValue(b_matrix(0, 1));
    _ui->b02->setValue(b_matrix(0, 2));
    _ui->b10->setValue(b_matrix(1, 0));
    _ui->b11->setValue(b_matrix(1, 1));
    _ui->b12->setValue(b_matrix(1, 2));
    _ui->b20->setValue(b_matrix(2, 0));
    _ui->b21->setValue(b_matrix(2, 1));
    _ui->b22->setValue(b_matrix(2, 2));

    Eigen::Matrix3d u_matrix_inverse = u_matrix.inverse();
    _ui->u00->setValue(u_matrix_inverse(0, 0));
    _ui->u01->setValue(u_matrix_inverse(0, 1));
    _ui->u02->setValue(u_matrix_inverse(0, 2));
    _ui->u10->setValue(u_matrix_inverse(1, 0));
    _ui->u11->setValue(u_matrix_inverse(1, 1));
    _ui->u12->setValue(u_matrix_inverse(1, 2));
    _ui->u20->setValue(u_matrix_inverse(2, 0));
    _ui->u21->setValue(u_matrix_inverse(2, 1));
    _ui->u22->setValue(u_matrix_inverse(2, 2));
}

WidgetUnitCell::~WidgetUnitCell()
{
    delete _ui;
}

nsx::sptrUnitCell WidgetUnitCell::unitCell() const
{
    return _unit_cell;
}
