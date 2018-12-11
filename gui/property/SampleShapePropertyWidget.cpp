#include "SampleShapePropertyWidget.h"
#include "DialogAbsorption.h"
#include <QMessageBox>
#include <core/Logger.h>
#include <core/Units.h>


SampleShapePropertyWidget::SampleShapePropertyWidget(const nsx::sptrExperiment& experiment)
    : QWidget()
    , _experiment(experiment)
    , _shape(experiment->diffractometer()->sample().shape())
    , ui(new Ui::SampleShapePropertyWidget)
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
    DialogAbsorption* dialog = new DialogAbsorption(_experiment, nullptr);
    if (!dialog->exec()) {
        if (_shape.checkEulerConditions()) {
            ui->lineEdit_MovieFilename->setText(QString::fromStdString(dialog->getMovieFilename()));
            // The hull is translated to its center
            _shape.translateToCenter();
            // The hull is rotated of -90 along chi axis (Y axis)
            Eigen::Matrix3d mat;
            mat << 0, 0, 1,
                   0, 1, 0,
                  -1, 0, 0;
            _shape.rotate(mat);
            //Convert to m
            _shape.scale(nsx::mm);
            nsx::debug() << "Coordinates of the Hull at rest:" << _shape;
            setHullProperties();
        }
    }
}

void SampleShapePropertyWidget::setHullProperties()
{
    ui->lineEdit_Volume->setText(QString::number(_shape.volume()/nsx::mm3)+" mm^3");
    ui->lineEdit_Faces->setText(QString::number(_shape.nFaces()));
    ui->lineEdit_Edges->setText(QString::number(_shape.nEdges()));
    ui->lineEdit_Vertices->setText(QString::number(_shape.nVertices()));
}
