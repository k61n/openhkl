#include <sstream>

#include "Axis.h"
#include "Diffractometer.h"
#include "Gonio.h"
#include "Sample.h"
#include "TransAxis.h"

#include "ui_SamplePropertyWidget.h"
#include "Tree/SampleItem.h"
#include "Tree/SamplePropertyWidget.h"
#include "Tree/UnitCellItem.h"

SamplePropertyWidget::SamplePropertyWidget(SampleItem* caller,QWidget *parent) :
    _sampleItem(caller),
    QWidget(parent),
    ui(new Ui::SamplePropertyWidget)
{
    ui->setupUi(this);
    auto sample=_sampleItem->getExperiment()->getDiffractometer()->getSample();
    auto gonio=sample->getGonio();

    ui->nsampleLabel->setText(QString::number(sample->getNCrystals()));

    ui->tableWidget_Sample->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_Sample->setRowCount(gonio->getNAxes());

    ui->tableWidget_Sample->setColumnCount(3);
    ui->tableWidget_Sample->verticalHeader()->setVisible(false);

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
        ui->tableWidget_Sample->setItem(i,0,item0);
        ui->tableWidget_Sample->setItem(i,1,item1);
        ui->tableWidget_Sample->setItem(i,2,item2);
    }

}

SamplePropertyWidget::~SamplePropertyWidget()
{
    delete ui;
}

void SamplePropertyWidget::on_pushButton_addCrystal_clicked()
{
     auto _sample=_sampleItem->getExperiment()->getDiffractometer()->getSample();
     auto cell=_sample->addUnitCell();
     std::size_t nsamples=_sample->getNCrystals();
     _sampleItem->appendRow(new UnitCellItem(_sampleItem->getExperiment(),cell));
     ui->nsampleLabel->setText(QString::number(nsamples));
     _sampleItem->child(0)->setEnabled(true);
}

