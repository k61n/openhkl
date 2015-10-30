#include "ui_DetectorPropertyWidget.h"
#include "Tree/DetectorPropertyWidget.h"
#include "Tree/DetectorItem.h"
#include "Diffractometer.h"
#include "Detector.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "TransAxis.h"

DetectorPropertyWidget::DetectorPropertyWidget(DetectorItem* caller,QWidget *parent) :
    QWidget(parent),
    _detectorItem(caller),
    ui(new Ui::DetectorPropertyWidget)
{
    ui->setupUi(this);

    auto detector=_detectorItem->getExperiment()->getDiffractometer()->getDetector();
    auto gonio=detector->getGonio();

    ui->lineEdit_H->setText(QString::number(detector->getHeight())+" m");
    ui->lineEdit_W->setText(QString::number(detector->getWidth())+" m");
    ui->lineEdit_WPixels->setText(QString::number(detector->getNCols()));
    ui->lineEdit_HPixels->setText(QString::number(detector->getNRows()));

    ui->tableWidget_Detector->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_Detector->setRowCount(gonio->getNAxes());

    ui->tableWidget_Detector->setColumnCount(3);
    ui->tableWidget_Detector->verticalHeader()->setVisible(false);
\
    for (unsigned int i=0;i<gonio->getNAxes();++i)
    {
        auto axis=gonio->getAxis(i);
        QTableWidgetItem* item0=new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->getLabel().c_str()));
        if (axis->isPhysical())
            item0->setBackgroundColor(QColor("#FFDDDD"));
        else
            item0->setBackgroundColor(QColor("#DDFFDD"));
        QTableWidgetItem* item1=new QTableWidgetItem();

        std::ostringstream os;
        if (RotAxis* rot=dynamic_cast<RotAxis*>(axis))
        {
            os << "R(";
            os << rot->getAxis().transpose();
            os << ")";
            if (rot->getRotationDirection())
                os << "CW";
            else
                os << "CCW";
        }
        else if(dynamic_cast<TransAxis*>(axis))
        {
            os << "T(";
            os << axis->getAxis().transpose();
            os << ")";
        }
        item1->setData(Qt::EditRole, QString(os.str().c_str()));
        QTableWidgetItem* item2=new QTableWidgetItem();
        item2->setData(Qt::EditRole, double(axis->getOffset()));
        ui->tableWidget_Detector->setItem(i,0,item0);
        ui->tableWidget_Detector->setItem(i,1,item1);
        ui->tableWidget_Detector->setItem(i,2,item2);
    }
}

DetectorPropertyWidget::~DetectorPropertyWidget()
{
    delete ui;
}
