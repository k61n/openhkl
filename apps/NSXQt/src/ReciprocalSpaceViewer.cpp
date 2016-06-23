#include "include/ReciprocalSpaceViewer.h"
#include "ui_ReciprocalSpaceViewer.h"

#include <stdexcept>

#include "Diffractometer.h"
#include "Experiment.h"
#include "IData.h"
#include "Sample.h"
#include "UnitCell.h"

ReciprocalSpaceViewer::ReciprocalSpaceViewer(SX::Instrument::Experiment* experiment,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReciprocalSpaceViewer),
    _experiment(experiment),
    _data()
{
    ui->setupUi(this);

    int nUnitCells(_experiment->getDiffractometer()->getSample()->getNCrystals());

    if (!nUnitCells)
        throw std::runtime_error("No unit cells defined for the sample");


    ui->unitCell->setMaximum(nUnitCells);
}

ReciprocalSpaceViewer::~ReciprocalSpaceViewer()
{
    delete ui;
}

void ReciprocalSpaceViewer::setData(const std::vector<SX::Data::IData*>& data)
{
    _data.clear();
    _data.reserve(data.size());

    for (auto d : data)
    {
        std::string basename(d->getBasename());
        if (_experiment->hasData(basename))
            _data.push_back(_experiment->getData(basename));
    }
}

void ReciprocalSpaceViewer::on_view_clicked()
{
    double dq(ui->dq->value());

    double halfwidth(ui->width->value()/2.0);

    std::shared_ptr<SX::Crystal::UnitCell> uc(_experiment->getDiffractometer()->getSample()->getUnitCell(ui->unitCell->value()));

    for (auto d : _data)
    {
        for (int f=0;f<d->getNFrames();++f)
        {
            qDebug()<<f;
        }
    }


}
