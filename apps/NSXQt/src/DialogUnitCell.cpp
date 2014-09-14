#include <QtDebug>
#include <QMessageBox>
#include <Eigen/Dense>
#include "DialogTransformationMatrix.h"
#include "DialogUnitCell.h"
#include "LatticeFinder.h"
#include "NiggliReduction.h"
#include "GruberReduction.h"
#include "Units.h"
#include "DialogUnitCellSolutions.h"

QDebug& operator<<(QDebug &dbg, const SX::Crystal::UnitCell& cell)
{
    std::ostringstream os;
    os <<cell;
    dbg << QString::fromStdString(os.str());
    return dbg;
}

QDebug& operator<<(QDebug &dbg, const Eigen::Matrix3d& m)
{
    std::ostringstream os;
    os <<m;
    dbg << QString::fromStdString(os.str());
    return dbg;
}


DialogUnitCell::DialogUnitCell(QWidget *parent):QDialog(parent),ui(new Ui::DialogUnitCell)
{
    ui->setupUi(this);
    ui->labelalpha->setText(QString((QChar) 0x03B1));
    ui->labelbeta->setText(QString((QChar) 0x03B2));
    ui->labelgamma->setText(QString((QChar) 0x03B3));
    connect(ui->pushButtonFindUnitCell,SIGNAL(clicked()),this,SLOT(getUnitCell()));
    connect(ui->pushButtonFindReindexHKL,SIGNAL(clicked()),this,SLOT(reindexHKL()));
    connect(ui->pushButtonGivePMatrix,SIGNAL(clicked()),this,SLOT(setTransformationMatrix()));
}
void DialogUnitCell::setPeaks(const std::vector<std::reference_wrapper<SX::Geometry::Peak3D>>& peaks)
{
    _peaks.clear();
    _peaks=peaks;
    ui->spinBoxNumberPeaks->setValue(_peaks.size());

    SX::Crystal::LatticeFinder finder(0.02,0.01);
    for (auto peak : _peaks)
    {
        Eigen::Vector3d realQ=peak.get().getQ();
        finder.addPoint(realQ[0],realQ[1],realQ[2]);
    }

    finder.run(3.0);
    Eigen::Vector3d as,bs,cs;
    std::vector<SX::Crystal::LatticeSolution> solutions=finder.determineLattice(30,30);

    for (auto solution : solutions)
    {
        SX::Crystal::UnitCell cell=SX::Crystal::UnitCell::fromReciprocalVectors(std::get<0>(solution),std::get<1>(solution),std::get<2>(solution));
        SX::Crystal::NiggliReduction n(cell.getMetricTensor(),0.02);
        Eigen::Matrix3d newg,P;
        n.reduce(newg,P);
        cell.transform(P);
        SX::Crystal::UnitCell niggli=cell;
        SX::Crystal::GruberReduction gr(cell.getMetricTensor(),1.5);
        Eigen::Matrix3d Pprime;
        SX::Crystal::LatticeCentring centring;
        SX::Crystal::BravaisType bravais;
        gr.reduce(Pprime,centring,bravais);
        cell.transform(Pprime);
        cell.setLatticeCentring(centring);
        cell.setBravaisType(bravais);
        double quality=0.0;
        std::shared_ptr<SX::Crystal::UnitCell> pcell(new SX::Crystal::UnitCell(cell));
        for (auto peak: _peaks)
        {
            if (peak.get().setBasis(pcell))
                quality+=1.0;
        }
        _unitcells.push_back(std::pair<SX::Crystal::UnitCell,double>(cell,quality/_peaks.size()));
    }
    // Sort the Quality of the solutions decreasing
    std::sort(_unitcells.begin(),
              _unitcells.end(),
              [](const std::pair<SX::Crystal::UnitCell,double>& cell1,const std::pair<SX::Crystal::UnitCell,double>& cell2)->bool
                {
                    return (cell1.second>cell2.second);
                });
}

DialogUnitCell::~DialogUnitCell()
{}

void DialogUnitCell::getUnitCell()
{

    DialogUnitCellSolutions* ucs=new DialogUnitCellSolutions(this);
    ucs->setSolutions(_unitcells);
    ucs->show();
    connect(ucs,SIGNAL(selectSolution(int)),this,SLOT(acceptSolution(int)));
}

void DialogUnitCell::acceptSolution(int i)
{
    _basis=_unitcells[i].first;
    setUpValues();
}

void DialogUnitCell::setUpValues()
{
    ui->doubleSpinBoxa->setValue(_basis.gete1Norm());
    ui->doubleSpinBoxb->setValue(_basis.gete2Norm());
    ui->doubleSpinBoxc->setValue(_basis.gete3Norm());
    ui->doubleSpinBoxalpha->setValue(_basis.gete2e3Angle()/SX::Units::deg);
    ui->doubleSpinBoxbeta->setValue(_basis.gete1e3Angle()/SX::Units::deg);
    ui->doubleSpinBoxgamma->setValue(_basis.gete1e2Angle()/SX::Units::deg);
    Eigen::Matrix3d M=_basis.getStandardM().inverse();
    ui->doubleSpinBoxUB00->setValue(M(0,0));
    ui->doubleSpinBoxUB01->setValue(M(0,1));
    ui->doubleSpinBoxUB02->setValue(M(0,2));
    ui->doubleSpinBoxUB10->setValue(M(1,0));
    ui->doubleSpinBoxUB11->setValue(M(1,1));
    ui->doubleSpinBoxUB12->setValue(M(1,2));
    ui->doubleSpinBoxUB20->setValue(M(2,0));
    ui->doubleSpinBoxUB21->setValue(M(2,1));
    ui->doubleSpinBoxUB22->setValue(M(2,2));
}

void DialogUnitCell::setTransformationMatrix()
{
    DialogTransformationmatrix* dialog=new DialogTransformationmatrix(this);
    dialog->exec();
}

void DialogUnitCell::reindexHKL()
{
    int success=0;

    std::shared_ptr<SX::Crystal::UnitCell> sptr(new SX::Crystal::UnitCell(_basis));
    for (SX::Geometry::Peak3D& peak : _peaks)
    {
        if (peak.setBasis(sptr))
            success++;
    }
    QMessageBox::information(this,"Indexation","Successfully indexed"+QString::number(success)+" peaks out of "+QString::number(_peaks.size()));
}
