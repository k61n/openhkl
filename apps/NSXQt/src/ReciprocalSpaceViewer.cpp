#include "include/ReciprocalSpaceViewer.h"
#include "ui_ReciprocalSpaceViewer.h"

#include <set>
#include <stdexcept>

#include "Detector.h"
#include "Diffractometer.h"
#include "Experiment.h"
#include "Gonio.h"
#include "IData.h"
#include "Sample.h"
#include "Source.h"
#include "UnitCell.h"
#include "Logger.h"

bool vecCompare (const Eigen::Vector3d& v, const Eigen::Vector3d& w)
{
    for (int i = 0; i < 3; ++i)
    {
        if (v(i) < w(i))
            return true;
        else
            return false;
    }
}

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

    if (dq < 0.0)
        throw std::runtime_error("Negative q resolution");

    double halfwidth(ui->width->value()/2.0);

    if (halfwidth < 0.0)
        throw std::runtime_error("Negative plane width");

//    std::shared_ptr<SX::Crystal::UnitCell> uc(_experiment->getDiffractometer()->getSample()->getUnitCell(ui->unitCell->value()));

    Eigen::Vector3d v1(1.0,0.0,0.0);
    Eigen::Vector3d v2(0.0,1.0,0.0);

    Eigen::Vector3d p(0.0,0.0,0.0);

    Eigen::Vector3d v3(v1.cross(v2));
    v3.normalize();

    // The distance from the point origin to the plane (P,v1,v2)
    double distOrigToPlane=v3.dot(p);

    SX::Instrument::Detector* detector(_experiment->getDiffractometer()->getDetector());
    int nDetRows = detector->getNRows();
    int nDetCols = detector->getNCols();

    SX::Instrument::Sample* sample(_experiment->getDiffractometer()->getSample());

    double lambda(_experiment->getDiffractometer()->getSource()->getWavelength());

    qDebug() << lambda;

    qDebug()<<_experiment->getDiffractometer()->getSource()->getNMonochromators();

    return;

    bool(*pf)(const Eigen::Vector3d&, const Eigen::Vector3d&)=vecCompare;

    std::set<Eigen::Vector3d,bool(*)(const Eigen::Vector3d&, const Eigen::Vector3d&)> ptsWithinPlanes(pf);

    for (auto d : _data)
    {
        for (int f=0;f<d->getNFrames();++f)
        {
            auto detectorStates(d->getDetectorState(f).getValues());
            Eigen::Matrix3d invM(sample->getGonio()->getInverseHomMatrix(d->getSampleState(f).getValues()).rotation());
            for (int i=0;i<nDetRows;++i)
            {
                double py=static_cast<double>(i)+0.5;
                for (int j=0;j<nDetCols;++j)
                {
                    double px=static_cast<double>(j)+0.5;
                    Eigen::Vector3d q(detector->getQ(px,py,lambda,detectorStates));
                    q = invM*q;
                    double distQToPlane=(std::fabs(v3.dot(q)+distOrigToPlane));
                    if (distQToPlane <= halfwidth)
                        ptsWithinPlanes.insert(q);
                }
            }
        }
    }

    qDebug()<<"HITS = "<<ptsWithinPlanes.size();


}
