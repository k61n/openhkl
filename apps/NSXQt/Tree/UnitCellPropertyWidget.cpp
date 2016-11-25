#include <set>
#include <memory>

#include <QMessageBox>
#include <QtDebug>

#include "Gonio.h"
#include "IData.h"
#include "Material.h"
#include "MaterialManager.h"
#include "UnitCell.h"
#include "Units.h"

#include "ui_UnitCellPropertyWidget.h"
#include "DialogFindUnitCell.h"
#include "DialogRefineUnitCell.h"
#include "DialogTransformationMatrix.h"
#include "Logger.h"
#include "Tree/UnitCellPropertyWidget.h"
#include "models/UnitCellItem.h"
#include "Peak3D.h"
#include "SpaceGroupSymbols.h"
#include <QCompleter>
#include <QSortFilterProxyModel>
#include "LatticeIndexer.h"

#include "ProgressHandler.h"
#include "ProgressView.h"

UnitCellPropertyWidget::UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent) :
    QWidget(parent),
    _unitCellItem(caller),
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

    SX::Crystal::SpaceGroupSymbols* sgs=SX::Crystal::SpaceGroupSymbols::Instance();
    std::vector<std::string> symbols=sgs->getAllSymbols();

    for (const auto& symbol : symbols)
    {
        ui->comboBox->addItem(QString::fromStdString(symbol));
    }

    ui->comboBox->setCurrentText(_unitCellItem->getCell()->getSpaceGroup().c_str());

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(ui->comboBox->model());

    QCompleter* completer = new QCompleter(proxyModel,ui->comboBox);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    ui->comboBox->setCompleter(completer);

    QObject::connect(ui->comboBox->lineEdit(), SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));
    QObject::connect(completer, SIGNAL(activated(const QString &)), this, SLOT(onCompleterActivated(const QString &)));

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
    LatticeIndexer* indexer=new LatticeIndexer(_unitCellItem->getCell(),_unitCellItem->getExperiment());
    indexer->show();
}

void UnitCellPropertyWidget::on_pushButton_AutoIndexing_clicked()
{
    DialogFindUnitCell* dialog = new DialogFindUnitCell(_unitCellItem->getExperiment(), this);
    // Ensure that lattice parameters are updated if a solution is accepted
    connect(dialog, SIGNAL(solutionAccepted(const SX::Crystal::UnitCell&)),
            this, SLOT(setCell(const SX::Crystal::UnitCell&)));
    dialog->exec();
}

void UnitCellPropertyWidget::setCell(const SX::Crystal::UnitCell& cell)
{
    // set up progress handler and view
    std::shared_ptr<SX::Utils::ProgressHandler> progressHandler(new SX::Utils::ProgressHandler);
    ProgressView progressView(this);

    progressView.watch(progressHandler);
    progressHandler->setStatus("Setting unit cell...");
    progressHandler->setProgress(0);

    int i;

    //_unitCellItem->getCell()->copyMatrices(cell);
    *_unitCellItem->getCell() = cell;

    getLatticeParams();
    auto datamap=_unitCellItem->getExperiment()->getData();
    for (auto data: datamap)
    {
        i = 0;
        auto& peaks=data.second->getPeaks();
        for (auto p: peaks)
        {
            p->setUnitCell(_unitCellItem->getCell());
            progressHandler->setProgress(i * 100.0 / peaks.size());
            ++i;
        }
    }

    qDebug() << "Set unit cell: bravais type " << cell.getBravaisTypeSymbol();

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
    getLatticeParams();
    emit cellUpdated();
}

void UnitCellPropertyWidget::setChemicalFormula(const QString &formula)
{
    auto mgr=SX::Chemistry::MaterialManager::Instance();
    try
    {
        auto material=mgr->buildMaterialFromChemicalFormula(formula.toStdString());
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

void UnitCellPropertyWidget::on_pushButton_TransformationMatrix_clicked()
{
    DialogTransformationmatrix* dialog=new DialogTransformationmatrix(this);
    connect(dialog,SIGNAL(getMatrix(Eigen::Matrix3d)),this,SLOT(transform(Eigen::Matrix3d)));
    dialog->exec();
}

void UnitCellPropertyWidget::transform(const Eigen::Matrix3d &P)
{
   _unitCellItem->getCell()->transform(P);
   getLatticeParams();
   // Update peaks
   auto datamap=_unitCellItem->getExperiment()->getData();
   for (auto data: datamap)
   {
       auto& peaks=data.second->getPeaks();
       for (auto p: peaks)
       {
           p->setUnitCell(_unitCellItem->getCell());
       }
   }
   emit cellUpdated();
}

//void UnitCellPropertyWidget::on_comboBox_activated(const QString &arg1)
//{
//        auto uc=_unitCellItem->getCell();
//        try
//        {
//            uc->setSpaceGroup(arg1.toStdString());
//        }
//        catch(...)
//        {
//            qWarning() << " Space group non valid, reverting to P 1";
//            uc->setSpaceGroup("P 1");
//            ui->comboBox->lineEdit()->setText("P 1");
//        }
//}

void UnitCellPropertyWidget::onCompleterActivated(const QString& text)
{
    if (text.isEmpty())
        return;

    ui->comboBox->setCurrentIndex(ui->comboBox->findText(text));
    ui->comboBox->activated(ui->comboBox->currentIndex());
}
