#include "DialogUnitCell.h"

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
    _peaks=peaks;
    ui->spinBoxNumberPeaks->setValue(peaks.size());
}

DialogUnitCell::~DialogUnitCell()
{}

void DialogUnitCell::getUnitCell()
{
    SX::Geometry::LatticeFinder finder(0.02,0.01);
    for (SX::Geometry::Peak3D& peak : _peaks)
    {
        Eigen::Vector3d realQ=peak.getQ();
        finder.addPoint(realQ[0],realQ[1],realQ[2]);
    }
    finder.run(3.0);
    Eigen::Vector3d as,bs,cs;
    if (!finder.determineLattice(as,bs,cs,30))
        return;

    SX::Geometry::Basis b=SX::Geometry::Basis::fromReciprocalVectors(as,bs,cs);
    SX::Crystal::NiggliReduction n(b.getMetricTensor(),1e-3);
    Eigen::Matrix3d newg,P;
    n.reduce(newg,P);
    b.transform(P);
    SX::Geometry::Basis niggli=b;
    SX::Crystal::GruberReduction gr(b.getMetricTensor(),4.0);
    Eigen::Matrix3d Pprime;
    SX::Crystal::UnitCell::Centring type;
    gr.reduce(Pprime,type);
    b.transform(Pprime);
    std::shared_ptr<SX::Geometry::Basis> conventional(new SX::Geometry::Basis(b));
    _basis=conventional;
    setUpValues();
}
void DialogUnitCell::setUpValues()
{
    ui->doubleSpinBoxa->setValue(_basis->gete1Norm());
    ui->doubleSpinBoxb->setValue(_basis->gete2Norm());
    ui->doubleSpinBoxc->setValue(_basis->gete3Norm());
    ui->doubleSpinBoxalpha->setValue(_basis->gete2e3Angle()/SX::Units::deg);
    ui->doubleSpinBoxbeta->setValue(_basis->gete1e3Angle()/SX::Units::deg);
    ui->doubleSpinBoxgamma->setValue(_basis->gete1e2Angle()/SX::Units::deg);
    Eigen::Matrix3d M=_basis->getStandardM().inverse();
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
    for (SX::Geometry::Peak3D& peak : _peaks)
    {
        if (peak.setBasis(_basis))
            success++;
    }
    QMessageBox::information(this,"Indexation","Successfully indexed"+QString::number(success)+" peaks out of "+QString::number(_peaks.size()));
}
