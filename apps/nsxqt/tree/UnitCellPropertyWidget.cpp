#include <QCompleter>
#include <QMessageBox>
#include <QSortFilterProxyModel>

#include <nsxlib/ChemistryTypes.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/Material.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "SessionModel.h"
#include "UnitCellItem.h"
#include "UnitCellPropertyWidget.h"

#include "ui_UnitCellPropertyWidget.h"

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

    SessionModel* sessionModel = dynamic_cast<SessionModel*>(_unitCellItem->model());

    connect(sessionModel,SIGNAL(updateCellParameters(nsx::sptrUnitCell)),this,SLOT(updateCellParameters(nsx::sptrUnitCell)));

    auto cell = _unitCellItem->unitCell();

    ui->spinBox_Z->setValue(cell->z());
    auto material=cell->material();
    if (material)
        ui->lineEdit_ChemicalFormula->setText(material->formula().c_str());

    updateCellParameters(cell);

    auto&& symbols = nsx::SpaceGroup::symbols();

    for (const auto& symbol : symbols)
        ui->comboBox->addItem(QString::fromStdString(symbol));

    ui->comboBox->setCurrentText(cell->spaceGroup().symbol().c_str());

    ui->indexingTolerance->setValue(cell->indexingTolerance());

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(ui->comboBox->model());

    QCompleter* completer = new QCompleter(proxyModel,ui->comboBox);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    ui->comboBox->setCompleter(completer);

    connect(ui->comboBox->lineEdit(), SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));
    connect(completer, SIGNAL(activated(const QString &)), this, SLOT(onCompleterActivated(const QString &)));

    connect(ui->indexingTolerance,SIGNAL(valueChanged(double)),this,SLOT(setIndexingTolerance(double)));

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
        _unitCellItem->unitCell()->setParams(a,b,c,alpha*nsx::deg,beta*nsx::deg,gamma*nsx::deg);
    }catch(...)
    {

    }

    setMassDensity();

}

void UnitCellPropertyWidget::setMassDensity() const
{
    auto material=_unitCellItem->unitCell()->material();
    if (material)
    {
        double mm=material->molarMass();
        mm*=ui->spinBox_Z->value()/nsx::avogadro;
        double volume=_unitCellItem->unitCell()->volume()*nsx::ang3;
        material->setMassDensity(mm/volume);
    }
}


void UnitCellPropertyWidget::updateCellParameters(nsx::sptrUnitCell cell)
{
    nsx::CellCharacter ch = cell->character();
    ui->doubleSpinBoxa->setValue(ch.a);
    ui->doubleSpinBoxb->setValue(ch.b);
    ui->doubleSpinBoxc->setValue(ch.c);
    ui->doubleSpinBoxalpha->setValue(ch.alpha/nsx::deg);
    ui->doubleSpinBoxbeta->setValue(ch.beta/nsx::deg);
    ui->doubleSpinBoxgamma->setValue(ch.gamma/nsx::deg);
}

void UnitCellPropertyWidget::getLatticeParams()
{
}

void UnitCellPropertyWidget::setChemicalFormula(const QString &formula)
{
    try
    {
        nsx::sptrMaterial material(new nsx::Material(formula.toStdString()));
        _unitCellItem->unitCell()->setMaterial(material);
    }
    catch(std::exception& e)
    {
       QMessageBox::critical(this, tr("NSXTool"),tr(e.what()));
        return;
   }
}

void UnitCellPropertyWidget::on_spinBox_Z_valueChanged(int arg1)
{
        _unitCellItem->unitCell()->setZ(arg1);
        setMassDensity();
}

void UnitCellPropertyWidget::on_lineEdit_ChemicalFormula_editingFinished()
{
    setChemicalFormula(ui->lineEdit_ChemicalFormula->text());
    setMassDensity();
}

void UnitCellPropertyWidget::onCompleterActivated(const QString& text)
{
    if (text.isEmpty())
        return;

    ui->comboBox->setCurrentIndex(ui->comboBox->findText(text));
    ui->comboBox->activated(ui->comboBox->currentIndex());
}

void UnitCellPropertyWidget::setIndexingTolerance(double tolerance)
{
    _unitCellItem->unitCell()->setIndexingTolerance(tolerance);
}
