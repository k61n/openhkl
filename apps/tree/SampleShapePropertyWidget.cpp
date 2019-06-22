//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/SampleShapePropertyWidget.cpp
//! @brief     Implements class SampleShapePropertyWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QMessageBox>

#include "base/logger/Logger.h"
#include "base/utils/Units.h"
#include "core/experiment/DataSet.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/monte-carlo/MCAbsorption.h"
#include "core/peak/Peak3D.h"
#include <xsection/Material.h>

#include "apps/dialogs/DialogAbsorption.h"
#include "apps/models/SampleShapeItem.h"
#include "apps/tree/SampleShapePropertyWidget.h"

#include "ui_SampleShapePropertyWidget.h"

SampleShapePropertyWidget::SampleShapePropertyWidget(SampleShapeItem* caller, QWidget* parent)
    : QWidget(parent), _caller(caller), ui(new Ui::SampleShapePropertyWidget)
{
    ui->setupUi(this);
    setHullProperties();
}

SampleShapePropertyWidget::~SampleShapePropertyWidget()
{
    delete ui;
}

void SampleShapePropertyWidget::on_pushButton_LoadMovie_clicked()
{
    DialogAbsorption* dialog = new DialogAbsorption(_caller->experiment(), nullptr);
    if (!dialog->exec()) {
        auto& sample = _caller->experiment()->diffractometer()->sample();
        auto& hull = sample.shape();
        if (hull.checkEulerConditions()) {
            ui->lineEdit_MovieFilename->setText(QString::fromStdString(dialog->getMovieFilename()));
            // The hull is translated to its center
            hull.translateToCenter();
            // The hull is rotated of -90 along chi axis (Y axis)
            Eigen::Matrix3d mat;
            mat << 0, 0, 1, 0, 1, 0, -1, 0, 0;
            hull.rotate(mat);
            // Convert to m
            hull.scale(nsx::mm);
            nsx::debug() << "Coordinates of the Hull at rest:" << hull;
            setHullProperties();
        }
    }
}

void SampleShapePropertyWidget::setHullProperties()
{
    const auto& sample = _caller->experiment()->diffractometer()->sample();
    const auto& hull = sample.shape();

    ui->lineEdit_Volume->setText(QString::number(hull.volume() / nsx::mm3) + " mm^3");
    ui->lineEdit_Faces->setText(QString::number(hull.nFaces()));
    ui->lineEdit_Edges->setText(QString::number(hull.nEdges()));
    ui->lineEdit_Vertices->setText(QString::number(hull.nVertices()));
}
