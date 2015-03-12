#include "SamplePropertyWidget.h"
#include "ui_SamplePropertyWidget.h"
#include "Sample.h"
#include "Gonio.h"
#include "Axis.h"
#include <sstream>
#include "SampleItem.h"
#include "Diffractometer.h"
#include "UnitCellItem.h"


SamplePropertyWidget::SamplePropertyWidget(SampleItem* caller,QWidget *parent) :
    _sampleItem(caller),
    QWidget(parent),
    ui(new Ui::SamplePropertyWidget)
{
    ui->setupUi(this);
    auto _sample=_sampleItem->getExperiment()->getDiffractometer()->getSample();
    auto gonio=_sample->getGonio();
    auto naxes=gonio->getNAxes();

    for (int i=0;i<naxes;++i)
    {
        auto axis=gonio->getAxis(i);
        QLabel* label=new QLabel(axis->getLabel().c_str());
        label->setFrameStyle(QFrame::Box);
        ui->axisLayout->addWidget(label,i+1,0);
        std::ostringstream os;
        os << axis->getAxis().transpose();
        QLabel* rotation=new QLabel(os.str().c_str());
        ui->axisLayout->addWidget(rotation,i+1,1);
        QLabel* offset=new QLabel(QString::number(axis->getOffset()));
        ui->axisLayout->addWidget(offset,i+1,2);
    }

    ui->nsampleLabel->setText(QString::number(_sample->getNCrystals()));

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
}

