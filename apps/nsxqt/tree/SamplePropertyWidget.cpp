#include <sstream>

#include <nsxlib/instrument/Axis.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/TransAxis.h>
#include <nsxlib/utils/Units.h>

#include "models/SampleItem.h"
#include "models/UnitCellItem.h"
#include "tree/SamplePropertyWidget.h"

#include "ui_SamplePropertyWidget.h"

SamplePropertyWidget::SamplePropertyWidget(SampleItem* caller,QWidget *parent) :
    QWidget(parent),
    _sampleItem(caller),
    ui(new Ui::SamplePropertyWidget)
{
    ui->setupUi(this);
    auto sample=_sampleItem->getExperiment()->getDiffractometer()->getSample();
    auto gonio=sample->getGonio();

    ui->tableWidget_Sample->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidget_Sample->setRowCount(gonio->getNAxes());

    ui->tableWidget_Sample->setColumnCount(3);
    ui->tableWidget_Sample->verticalHeader()->setVisible(false);

    for (unsigned int i=0;i<gonio->getNAxes();++i) {
        auto axis=gonio->getAxis(i);
        QTableWidgetItem* item0=new QTableWidgetItem();
        item0->setData(Qt::EditRole, QString(axis->getLabel().c_str()));
        if (axis->isPhysical())
            item0->setBackgroundColor(QColor("#FFDDDD"));
        else
            item0->setBackgroundColor(QColor("#DDFFDD"));
        QTableWidgetItem* item1=new QTableWidgetItem();

        std::ostringstream os;

        bool isrot=false;

        if (nsx::RotAxis* rot=dynamic_cast<nsx::RotAxis*>(axis)) {
            os << "R(";
            os << rot->getAxis().transpose();
            os << ")";
            if (rot->getRotationDirection()) {
                os << "CW";
            } else {
                os << "CCW";
            }
            isrot=true;
        }
        else if(dynamic_cast<nsx::TransAxis*>(axis)) {
            os << "T(";
            os << axis->getAxis().transpose();
            os << ")";
        }
        item1->setData(Qt::EditRole, QString(os.str().c_str()));
        QTableWidgetItem* item2=new QTableWidgetItem();

        if (isrot) {
            item2->setData(Qt::EditRole, double(axis->getOffset()/nsx::deg));
        } else {
            item2->setData(Qt::EditRole, double(axis->getOffset()/nsx::mm));
        }
        item0->setFlags(item0->flags() &~Qt::ItemIsEditable);
        item1->setFlags(item1->flags() &~Qt::ItemIsEditable);
        ui->tableWidget_Sample->setItem(i,0,item0);
        ui->tableWidget_Sample->setItem(i,1,item1);
        ui->tableWidget_Sample->setItem(i,2,item2);
    }
    connect(ui->tableWidget_Sample,SIGNAL(cellChanged(int,int)),this,SLOT(cellHasChanged(int,int)));
}

SamplePropertyWidget::~SamplePropertyWidget()
{
    delete ui;
}

void SamplePropertyWidget::cellHasChanged(int i,int j)
{
    auto sample=_sampleItem->getExperiment()->getDiffractometer()->getSample();
    auto axis=sample->getGonio()->getAxis(i);
    if (dynamic_cast<nsx::TransAxis*>(axis)) {
         axis->setOffset(ui->tableWidget_Sample->item(i,j)->data(Qt::EditRole).toDouble()*nsx::mm); // Given in mm
    } else {
         axis->setOffset(ui->tableWidget_Sample->item(i,j)->data(Qt::EditRole).toDouble()*nsx::deg); // Given in degs
    }
}

