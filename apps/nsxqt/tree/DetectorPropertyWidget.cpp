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

    auto detector = _detectorItem->experiment()->diffractometer()->detector();

    auto gonio = detector->gonio();

    auto axes = gonio->axes();

    ui->lineEdit_H->setText(QString::number(detector->height())+" m");
    ui->lineEdit_W->setText(QString::number(detector->width())+" m");
    ui->lineEdit_WPixels->setText(QString::number(detector->nCols()));
    ui->lineEdit_HPixels->setText(QString::number(detector->nRows()));

    // rest position of the detector is along y
    ui->doubleSpinBox_Distance->setValue(detector->distance());

    //
    ui->tableWidget_Detector->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidget_Detector->setRowCount(axes.size());

    ui->tableWidget_Detector->setColumnCount(3);
    ui->tableWidget_Detector->verticalHeader()->setVisible(false);

    for (size_t i = 0; i < axes.size(); ++i) {
        auto axis = axes[i];
        QTableWidgetItem* item0 = new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->name().c_str()));
        if (axis->physical()) {
            item0->setBackgroundColor(QColor("#FFDDDD"));
        } else {
            item0->setBackgroundColor(QColor("#DDFFDD"));
        }
        QTableWidgetItem* item1 = new QTableWidgetItem();
        std::ostringstream os;

        if (nsx::RotAxis* rot=dynamic_cast<nsx::RotAxis*>(axis)) {
            os << "R(";
            os << rot->axis().transpose();
            os << ")";
            if (rot->rotationDirection()) {
                os << "CW";
            } else {
                os << "CCW";
            }
            //isRot = true;
        } else if(dynamic_cast<nsx::TransAxis*>(axis)) {
            os << "T(";
            os << axis->axis().transpose();
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
    Q_UNUSED(i)
    Q_UNUSED(j)
}

void DetectorPropertyWidget::on_doubleSpinBox_Distance_valueChanged(double arg1)
{
     auto detector=_detectorItem->experiment()->diffractometer()->detector();
     if (arg1 < 0) {
         return;
     }
    detector->setDistance(arg1);
}
