#include <sstream>

#include <nsxlib/Axis.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/Sample.h>
#include <nsxlib/TransAxis.h>
#include <nsxlib/Units.h>

#include "SampleItem.h"
#include "SamplePropertyWidget.h"
#include "UnitCellItem.h"

#include "ui_SamplePropertyWidget.h"

SamplePropertyWidget::SamplePropertyWidget(SampleItem* caller,QWidget *parent) :
    QWidget(parent),
    _sampleItem(caller),
    ui(new Ui::SamplePropertyWidget)
{

    ui->setupUi(this);

    const auto &sample = _sampleItem->experiment()->diffractometer()->sample();

    const auto &sample_gonio = sample.gonio();

    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    ui->tableWidget_Sample->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidget_Sample->setRowCount(n_sample_gonio_axes);

    ui->tableWidget_Sample->setColumnCount(3);
    ui->tableWidget_Sample->verticalHeader()->setVisible(false);

    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const auto &axis = sample_gonio.axis(i);

        QTableWidgetItem* item0 = new QTableWidgetItem();

        item0->setData(Qt::EditRole, QString(axis.name().c_str()));
        item0->setBackgroundColor(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));

        QTableWidgetItem* item1 = new QTableWidgetItem();

        std::ostringstream os;
        os << axis;

        item1->setData(Qt::EditRole, QString(os.str().c_str()));
        QTableWidgetItem* item2=new QTableWidgetItem();

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
    Q_UNUSED(i)
    Q_UNUSED(j)
}

