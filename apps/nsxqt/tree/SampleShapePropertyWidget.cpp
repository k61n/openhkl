#include <nsxlib/chemistry/Material.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/geometry/MCAbsorption.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/utils/Units.h>

#include <QDebug>
#include <QMessageBox>

#include "../absorption/AbsorptionDialog.h"
#include "Logger.h"
#include "../models/SampleShapeItem.h"
#include "../tree/SampleShapePropertyWidget.h"

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
    AbsorptionDialog* dialog=new AbsorptionDialog(_caller->getExperiment(),nullptr);
    if (!dialog->exec())
    {
        auto sample=_caller->getExperiment()->getDiffractometer()->getSample();
        auto& hull=sample->getShape();
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
            qDebug() << "Coordinates of the Hull at rest:" << hull;
            setHullProperties();
        }
    }
}

void SampleShapePropertyWidget::setHullProperties()
{
    auto sample=_caller->getExperiment()->getDiffractometer()->getSample();
    auto& hull=sample->getShape();

    ui->lineEdit_Volume->setText(QString::number(hull.getVolume()/nsx::mm3)+" mm^3");
    ui->lineEdit_Faces->setText(QString::number(hull.getNFaces()));
    ui->lineEdit_Edges->setText(QString::number(hull.getNEdges()));
    ui->lineEdit_Vertices->setText(QString::number(hull.getNVertices()));
}
