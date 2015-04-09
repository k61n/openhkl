#include <set>

#include <QMessageBox>
#include <QtDebug>

#include "Gonio.h"
#include "IData.h"
#include "IMaterial.h"
#include "MaterialManager.h"
#include "UnitCell.h"
#include "Units.h"

#include "ui_UnitCellPropertyWidget.h"
#include "DialogFindUnitCell.h"
#include "DialogRefineUnitCell.h"
#include "DialogTransformationMatrix.h"
#include "Logger.h"
#include "Tree/UnitCellPropertyWidget.h"
#include "Tree/UnitCellItem.h"
#include "Peak3D.h"

UnitCellPropertyWidget::UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent) :
    _unitCellItem(caller),
    QWidget(parent),
    ui(new Ui::UnitCellPropertyWidget)
{
    ui->setupUi(this);
    // Special character
    ui->labelalpha->setText(QString((QChar) 0x03B1));
    ui->labelbeta->setText(QString((QChar) 0x03B2));
    ui->labelgamma->setText(QString((QChar) 0x03B3));

    connect(ui->doubleSpinBoxa,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxb,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxc,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxalpha,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxbeta,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->doubleSpinBoxgamma,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));

    ui->spinBox_Z->setValue(_unitCellItem->getCell()->getZ());
    auto material=_unitCellItem->getCell()->getMaterial();
    if (material)
        ui->lineEdit_ChemicalFormula->setText(material->getName().c_str());

    getLatticeParams();
}



UnitCellPropertyWidget::~UnitCellPropertyWidget()
{
    delete ui;
}

void UnitCellPropertyWidget::setLatticeParams()
{
    double a=ui->doubleSpinBoxa->value();
    double b=ui->doubleSpinBoxb->value();
    double c=ui->doubleSpinBoxc->value();
    double alpha=ui->doubleSpinBoxalpha->value();
    double beta=ui->doubleSpinBoxbeta->value();
    double gamma=ui->doubleSpinBoxgamma->value();

    try
    {
    _unitCellItem->getCell()->setParams(a,b,c,alpha*SX::Units::deg,beta*SX::Units::deg,gamma*SX::Units::deg);
    }catch(...)
    {

    }

    setMassDensity();

}

void UnitCellPropertyWidget::setMassDensity() const
{
    auto material=_unitCellItem->getCell()->getMaterial();
    if (material)
    {
        double mm=material->getMolarMass();
        mm*=ui->spinBox_Z->value()/SX::Units::avogadro;
        double volume=_unitCellItem->getCell()->getVolume()*SX::Units::ang3;
        material->setMassDensity(mm/volume);
    }
}

void UnitCellPropertyWidget::on_pushButton_Info_clicked()
{
   qDebug() << "" << *(_unitCellItem->getCell());
}

void UnitCellPropertyWidget::on_pushButton_Index_clicked()
{
    emit activateIndexingMode(_unitCellItem->getCell());
}

void UnitCellPropertyWidget::on_pushButton_AutoIndexing_clicked()
{
    DialogFindUnitCell* dialog=new DialogFindUnitCell(_unitCellItem->getExperiment(),this);
    // Ensure that lattice parameters are updated if a solution is accepted
    connect(dialog,SIGNAL(solutionAccepted(SX::Crystal::UnitCell)),this,SLOT(setCell(SX::Crystal::UnitCell)));
    dialog->exec();
}

void UnitCellPropertyWidget::setCell(const SX::Crystal::UnitCell& cell)
{
    _unitCellItem->getCell()->copyMatrices(cell);
    getLatticeParams();
    auto datamap=_unitCellItem->getExperiment()->getData();
    for (auto data: datamap)
    {
        auto& peaks=data.second->getPeaks();
        for (auto p: peaks)
        {
            p->setBasis(_unitCellItem->getCell());
        }
    }
    emit activateIndexingMode(_unitCellItem->getCell());
    emit cellUpdated();
}

void UnitCellPropertyWidget::getLatticeParams()
{
    auto sample=_unitCellItem->getCell();
    ui->doubleSpinBoxa->setValue(sample->getA());
    ui->doubleSpinBoxb->setValue(sample->getB());
    ui->doubleSpinBoxc->setValue(sample->getC());
    ui->doubleSpinBoxalpha->setValue(sample->getAlpha()/SX::Units::deg);
    ui->doubleSpinBoxbeta->setValue(sample->getBeta()/SX::Units::deg);
    ui->doubleSpinBoxgamma->setValue(sample->getGamma()/SX::Units::deg);
}

void UnitCellPropertyWidget::on_pushButton_Refine_clicked()
{
    DialogRefineUnitCell* dialog=new DialogRefineUnitCell(_unitCellItem->getExperiment(),_unitCellItem->getCell(),this);
    dialog->exec();
}

void UnitCellPropertyWidget::setChemicalFormula(const QString &formula)
{
    auto mgr=SX::Chemistry::MaterialManager::Instance();
    try
    {
        auto material=mgr->buildMaterialFromChemicalFormula(formula.toStdString(),SX::Chemistry::ChemicalState::Solid);
        _unitCellItem->getCell()->setMaterial(material);
    }
    catch(std::exception& e)
    {
       QMessageBox::critical(this, tr("NSXTool"),tr(e.what()));
        return;
   }
}

void UnitCellPropertyWidget::on_spinBox_Z_valueChanged(int arg1)
{
        _unitCellItem->getCell()->setZ(arg1);
        setMassDensity();
}

void UnitCellPropertyWidget::on_lineEdit_ChemicalFormula_editingFinished()
{
    setChemicalFormula(ui->lineEdit_ChemicalFormula->text());
    setMassDensity();
}

void UnitCellPropertyWidget::on_pushButton_Refine_2_clicked()
{
    DialogTransformationmatrix* dialog=new DialogTransformationmatrix(this);
    connect(dialog,SIGNAL(getMatrix(Eigen::Matrix3d)),this,SLOT(transform(Eigen::Matrix3d)));
    dialog->exec();
}

void UnitCellPropertyWidget::transform(const Eigen::Matrix3d &P)
{
   _unitCellItem->getCell()->transform(P);
   getLatticeParams();
}

void UnitCellPropertyWidget::on_pushButton_WeakPeaks_pressed()
{
//    auto peakmap=_unitCellItem->getCell()->generateReflectionsInSphere(2.0);
//    qDebug() << "reflections in sphere generated";
//    auto UB=_unitCellItem->getCell()->getReciprocalReferenceM();
//    auto& map=_unitCellItem->getExperiment()->getData();

//    int npeaks=0;
//    for (auto& d: map)
//    {
//        auto& data=d.second;
//        for (auto p : peakmap)
//        {
//            auto& hkl=p.second;
//            SX::Crystal::PeakCalc* pe=data->hasPeak(hkl[0],hkl[1],hkl[2],UB);
//            if (pe)
//            {
//               //qDebug()<< "Adding peak" << hkl[0] << " " << hkl[1] << " " << hkl[2];
//               //data->addPeak(pe);
//               npeaks++;
//            }
//        }

//    }
//    std::cout << "npeaks" << npeaks << std::endl;
//    emit cellUpdated();
}

void UnitCellPropertyWidget::on_pushButton_UB_clicked()
{

}
