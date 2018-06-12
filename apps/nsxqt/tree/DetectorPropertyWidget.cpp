#include <nsxlib/Detector.h>
#include <nsxlib/DirectVector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/RotAxis.h>
#include <nsxlib/TransAxis.h>
#include <nsxlib/Units.h>

#include "DetectorItem.h"
#include "DetectorPropertyWidget.h"

#include "ui_DetectorPropertyWidget.h"

DetectorPropertyWidget::DetectorPropertyWidget(DetectorItem* caller,QWidget *parent) :
    QWidget(parent),
    _detectorItem(caller),
    ui(new Ui::DetectorPropertyWidget)
{
    ui->setupUi(this);

    auto detector=_detectorItem->experiment()->diffractometer()->detector();
    auto gonio=detector->getGonio();

    ui->lineEdit_H->setText(QString::number(detector->getHeight())+" m");
    ui->lineEdit_W->setText(QString::number(detector->getWidth())+" m");
    ui->lineEdit_WPixels->setText(QString::number(detector->getNCols()));
    ui->lineEdit_HPixels->setText(QString::number(detector->getNRows()));

    nsx::DirectVector restpos = detector->getRestPosition();
    // rest position of the detector is along y
    ui->doubleSpinBox_Distance->setValue(restpos[1]);

    //
    ui->tableWidget_Detector->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidget_Detector->setRowCount(gonio->getNAxes());

    ui->tableWidget_Detector->setColumnCount(3);
    ui->tableWidget_Detector->verticalHeader()->setVisible(false);

    for (unsigned int i = 0; i < gonio->getNAxes(); ++i) {
        auto axis=gonio->getAxis(i);
        QTableWidgetItem* item0=new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->getLabel().c_str()));
        if (axis->isPhysical()) {
            item0->setBackgroundColor(QColor("#FFDDDD"));
        } else {
            item0->setBackgroundColor(QColor("#DDFFDD"));
        }
        QTableWidgetItem* item1 = new QTableWidgetItem();
        std::ostringstream os;

        if (nsx::RotAxis* rot=dynamic_cast<nsx::RotAxis*>(axis)) {
            os << "R(";
            os << rot->getAxis().transpose();
            os << ")";
            if (rot->getRotationDirection()) {
                os << "CW";
            } else {
                os << "CCW";
            }
            //isRot = true;
        } else if(dynamic_cast<nsx::TransAxis*>(axis)) {
            os << "T(";
            os << axis->getAxis().transpose();
            os << ")";
        }
        item1->setData(Qt::EditRole, QString(os.str().c_str()));

        QTableWidgetItem* item2=new QTableWidgetItem();

        // First two columns non-editable
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);

        ui->tableWidget_Detector->setItem(i,0,item0);
        ui->tableWidget_Detector->setItem(i,1,item1);
        ui->tableWidget_Detector->setItem(i,2,item2);
    }

    connect(ui->tableWidget_Detector,SIGNAL(cellChanged(int,int)),this,SLOT(cellHasChanged(int,int)));
}

DetectorPropertyWidget::~DetectorPropertyWidget()
{
    delete ui;
}

void DetectorPropertyWidget::cellHasChanged(int i, int j)
{
    auto detector=_detectorItem->experiment()->diffractometer()->detector();
    auto axis=detector->getGonio()->getAxis(i);
    // todo: fix this after offset refactor
}

void DetectorPropertyWidget::on_doubleSpinBox_Distance_valueChanged(double arg1)
{
     auto detector=_detectorItem->experiment()->diffractometer()->detector();
     if (arg1>0)
        detector->setRestPosition(nsx::DirectVector(0,arg1,0));
}
