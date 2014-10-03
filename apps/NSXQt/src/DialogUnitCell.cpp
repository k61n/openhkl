#include <QtDebug>
#include <QMessageBox>
#include <Eigen/Dense>
#include "DialogTransformationMatrix.h"
#include "DialogUnitCell.h"
#include "FFTIndexing.h"
#include "NiggliReduction.h"
#include "GruberReduction.h"
#include "Units.h"
#include "UBMinimizer.h"
#include "DialogUnitCellSolutions.h"
#include <unsupported/Eigen/FFT>
#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>
#include "Gonio.h"
#include "Logger.h"


using namespace SX::Crystal;
using namespace SX::Geometry;



DialogUnitCell::DialogUnitCell(QWidget *parent):QDialog(parent),ui(new Ui::DialogUnitCell)
{
    ui->setupUi(this);
    ui->labelalpha->setText(QString((QChar) 0x03B1));
    ui->labelbeta->setText(QString((QChar) 0x03B2));
    ui->labelgamma->setText(QString((QChar) 0x03B3));
    connect(ui->pushButtonFindUnitCell,SIGNAL(clicked()),this,SLOT(getUnitCell()));
    connect(ui->pushButtonGivePMatrix,SIGNAL(clicked()),this,SLOT(setTransformationMatrix()));
}
void DialogUnitCell::setPeaks(const std::vector<std::reference_wrapper<SX::Crystal::Peak3D>>& peaks)
{
    _peaks.clear();
    _peaks=peaks;
    ui->spinBoxNumberPeaks->setValue(_peaks.size());

}

DialogUnitCell::~DialogUnitCell()
{}

void DialogUnitCell::getUnitCell()
{
    qWarning() << "Find possible Unit-Cells";
    _unitcells.clear();

    if (!_peaks.size())
        return;
    std::vector<Eigen::Vector3d> qvects;
    qvects.reserve(_peaks.size());
    for (const auto& peak : _peaks)
    qvects.push_back(peak.get().getQ());

    qDebug() << "Searching direct lattice vectors using" << _peaks.size() << "peaks";
    FFTIndexing indexing(50.0);
    indexing.addVectors(qvects);
    qDebug() << "Running 7000 FFTs, keeping best 10 tvectors";
    std::vector<tVector> tvects=indexing.findOnSphere(50,20);
    qDebug() << "Refining solutions and diffractometers offsets";
    int soluce=0;
    for (int i=0;i<10;++i)
    {
    for (int j=i+1;j<10;++j)
    {
    for (int k=j+1;k<10;++k)
    {
        Eigen::Vector3d& v1=tvects[i]._vect;
        Eigen::Vector3d& v2=tvects[j]._vect;
        Eigen::Vector3d& v3=tvects[k]._vect;

        if (v1.dot(v2.cross(v3))>20.0)
        {
                UnitCell cell=UnitCell::fromDirectVectors(v1,v2,v3);
                std::shared_ptr<UnitCell> pcell(new UnitCell(cell));
                UBMinimizer minimizer;
                minimizer.setSample(dynamic_cast<SX::Instrument::Sample*>(_peaks[0].get().getSampleState()->getParent()));
                minimizer.setDetector(_peaks[0].get().getDetectorEvent()->getParent());
                minimizer.setFixedParameters(13);
                int success=0;
                for (auto& peak : _peaks)
                {
                    if (peak.get().hasIntegerHKL(pcell))
                    {
                        minimizer.addPeak(peak);
                        ++success;
                    }
                }

                if (success < 10)
                    continue;

                Eigen::Matrix3d M=cell.getReciprocalStandardM();
                minimizer.setStartingUBMatrix(M);

                int ret = minimizer.run(100);

                if (ret==1)
                {
                    qDebug() << "Refining solution... " << ++soluce << " ... convergence reached";
                    UBSolution solution=minimizer.getSolution();
                    SX::Crystal::UnitCell cc;
                    try
                    {
                        cc=SX::Crystal::UnitCell::fromReciprocalVectors(solution._ub.row(0),solution._ub.row(1),solution._ub.row(2));
                        cc.setReciprocalSigmas(solution._sigmaub.row(0),solution._sigmaub.row(1),solution._sigmaub.row(2));
                    }catch(...)
                    {
                        continue;
                    }
                    NiggliReduction niggli(cc.getMetricTensor(),0.03);
                    Eigen::Matrix3d newg,P;
                    niggli.reduce(newg,P);
                    cc.transform(P);
                    GruberReduction gruber(cc.getMetricTensor(),0.05);
                    LatticeCentring c;
                    BravaisType b;
                    gruber.reduce(P,c,b);
                    cc.setLatticeCentring(c);
                    cc.setBravaisType(b);
                    cc.transform(P);
                    double ap,bp,cp,alpha,beta,gamma;
                    double as,bs,cs,alphas,betas,gammas;
                    cc.getParameters(ap,bp,cp,alpha,beta,gamma);
                    cc.getParametersSigmas(as,bs,cs,alphas,betas,gammas);
                    double score=0.0;
                    std::shared_ptr<UnitCell> pcc(new UnitCell(cc));
                    for (auto& peak : _peaks)
                    {
                        if (peak.get().setBasis(pcc))
                            score++;
                    }
                    score /= 0.01*_peaks.size();
                    _unitcells.push_back(std::make_tuple(cc,solution,score));
                }
                 minimizer.resetParameters();
        }
    }
    }
    }
    //Sort the Quality of the solutions decreasing
    std::sort(_unitcells.begin(),
              _unitcells.end(),
              [](const std::tuple<SX::Crystal::UnitCell,SX::Crystal::UBSolution,double>& cell1,const std::tuple<SX::Crystal::UnitCell,SX::Crystal::UBSolution,double>& cell2)->bool
                {
                    return (std::get<2>(cell1)>std::get<2>(cell2));
                });

    DialogUnitCellSolutions* ucs=new DialogUnitCellSolutions(this);
    ucs->setSolutions(_unitcells);
    ucs->show();
    connect(ucs,SIGNAL(selectSolution(int)),this,SLOT(acceptSolution(int)));
}

void DialogUnitCell::acceptSolution(int i)
{
    qWarning() << "Selection of Unit Cell and UB-matrix";
    _basis=std::get<0>(_unitcells[i]);
    const SX::Crystal::UBSolution& sol=std::get<1>(_unitcells[i]);
    setUpValues();
    SX::Instrument::Component* s=_peaks[0].get().getSampleState()->getParent();
    for (int i=0;i<s->numberOfAxes();++i)
    {
        s->getGonio()->getAxis(i)->setOffset(sol._sampleOffsets[i]);
    }
    SX::Instrument::Detector* d=_peaks[0].get().getDetectorEvent()->getParent();
    for (int i=0;i<d->numberOfAxes();++i)
    {
        d->getGonio()->getAxis(i)->setOffset(sol._detectorOffsets[i]);
    }

    int success=0;

    for (auto& peak : _peaks)
    {
        if (peak.get().setBasis(std::shared_ptr<UnitCell>(new UnitCell(_basis))))
            ++success;
    }

    QMessageBox::information(this,"Indexation","Successfully indexed"+QString::number(success)+" peaks out of "+QString::number(_peaks.size()));

    qDebug() << "" << _basis << sol;
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

