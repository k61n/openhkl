#include "OpenGL/GLWidget.h"
#include "OpenGL/GLSphere.h"
#include "include/LatticeIndexer.h"
#include "ui_LatticeIndexer.h"
#include "IData.h"
#include "OpenGL/GLReciprocalLattice.h"

LatticeIndexer::LatticeIndexer(std::shared_ptr<SX::Crystal::UnitCell> ptrCell,
                               std::shared_ptr<SX::Instrument::Experiment> experiment,
                               QWidget *parent)
    : QWidget(parent),
      _ptrCell(ptrCell),
      _experiment(experiment),
    ui(new Ui::LatticeIndexer)
{
    ui->setupUi(this);

    if (_experiment)
    {
        updatePeaks();
        updateCell();
    }
}

void LatticeIndexer::updatePeaks()
{
    auto& scene=ui->glScene->getScene();
    auto datav=_experiment->getData();
    double amin=_ptrCell->getReciprocalA();
    double bmin=_ptrCell->getReciprocalB();
    double cmin=_ptrCell->getReciprocalC();
    double min=std::min(amin,bmin);
    min=std::min(min,cmin);

    for (auto idata : datav)
    {
       auto peaks=idata.second->getPeaks();
       for (auto peak: peaks)
       {
           GLSphere* sphere=new GLSphere("");
           Eigen::Vector3d pos=peak->getQ();
           sphere->setPos(pos[0],pos[1],pos[2]);
           sphere->setColor(0,1,0);
           sphere->setScale(0.02*min);
           scene.addActor(sphere);
       }
    }
    ui->glScene->show();
}

void LatticeIndexer::updateCell()
{
    auto& scene=ui->glScene->getScene();

    GLReciprocalLattice* rcell=new GLReciprocalLattice("");

    rcell->setSingleCell();


    rcell->setUnitCell(_ptrCell);

    scene.addActor(rcell);

    ui->glScene->show();

}

LatticeIndexer::~LatticeIndexer()
{
    delete ui;
}
