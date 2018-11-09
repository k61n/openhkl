#include <QMessageBox>

#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Material.h>
#include <nsxlib/MCAbsorption.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/Units.h>

#include "DialogAbsorption.h"
#include "SampleShapeItem.h"
#include "SampleShapePropertyWidget.h"

#include "ui_SampleShapePropertyWidget.h"

SampleShapePropertyWidget::SampleShapePropertyWidget(SampleShapeItem* caller,QWidget *parent) :
     QWidget(parent),
    _caller(caller),
    ui(new Ui::SampleShapePropertyWidget)
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
    DialogAbsorption* dialog=new DialogAbsorption(_caller->experiment(),nullptr);
    if (!dialog->exec())
    {
        auto &sample =_caller->experiment()->diffractometer()->sample();
        auto &hull = sample.shape();
        if (hull.checkEulerConditions())
        {
            ui->lineEdit_MovieFilename->setText(QString::fromStdString(dialog->getMovieFilename()));
            // The hull is translated to its center
            hull.translateToCenter();
            // The hull is rotated of -90 along chi axis (Y axis)
            Eigen::Matrix3d mat;
            mat << 0, 0, 1,
                   0, 1, 0,
                  -1, 0, 0;
            hull.rotate(mat);
            //Convert to m
            hull.scale(nsx::mm);
            nsx::debug() << "Coordinates of the Hull at rest:" << hull;
            setHullProperties();
        }
    }
}

void SampleShapePropertyWidget::setHullProperties()
{
    const auto &sample =_caller->experiment()->diffractometer()->sample();
    const auto& hull = sample.shape();

    ui->lineEdit_Volume->setText(QString::number(hull.volume()/nsx::mm3)+" mm^3");
    ui->lineEdit_Faces->setText(QString::number(hull.nFaces()));
    ui->lineEdit_Edges->setText(QString::number(hull.nEdges()));
    ui->lineEdit_Vertices->setText(QString::number(hull.nVertices()));
}
