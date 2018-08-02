#include <stdexcept>

#include <QCompleter>
#include <QStandardItem>

#include <nsxlib/ChemistryTypes.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Material.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "SessionModel.h"
#include "UnitCellItem.h"
#include "UnitCellPropertyWidget.h"

#include "ui_UnitCellPropertyWidget.h"

UnitCellPropertyWidget::UnitCellPropertyWidget(UnitCellItem* unit_cell_item, QWidget *parent) :
    QWidget(parent),
    _unitCellItem(unit_cell_item),
    ui(new Ui::UnitCellPropertyWidget)
{
    ui->setupUi(this);

    // Set the title of the cell parameters group box to normal font
    ui->cellParameters->setStyleSheet("font-weight: normal;");

    // Special character
    ui->labelalpha->setText(QString((QChar) 0x03B1));
    ui->labelbeta->setText(QString((QChar) 0x03B2));
    ui->labelgamma->setText(QString((QChar) 0x03B3));

    for (auto&& sg : nsx::SpaceGroup::symbols()) {
        ui->spaceGroup->addItem(QString::fromStdString(sg));
    }

    QCompleter* completer = new QCompleter(ui->spaceGroup->model(),ui->spaceGroup);
    ui->spaceGroup->setCompleter(completer);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);

    update();

    connect(_unitCellItem->model(),SIGNAL(itemChanged(QStandardItem*)),this,SLOT(update(QStandardItem*)));

    connect(ui->name,SIGNAL(editingFinished()),this, SLOT(setUnitCellName()));
    connect(ui->a,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->b,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->c,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->alpha,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->beta,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->gamma,SIGNAL(editingFinished()),this,SLOT(setLatticeParams()));
    connect(ui->spaceGroup,SIGNAL(currentIndexChanged(QString)),this,SLOT(setSpaceGroup(QString)));
    connect(ui->spaceGroup->completer(), SIGNAL(activated(const QString &)), this, SLOT(activateSpaceGroupCompletion(const QString &)));
    connect(ui->indexingTolerance,SIGNAL(valueChanged(double)),this,SLOT(setIndexingTolerance(double)));
    connect(ui->z,SIGNAL(valueChanged(int)),this,SLOT(setZValue(int)));
    connect(ui->chemicalFormula,SIGNAL(editingFinished()),this,SLOT(setChemicalFormula()));
}

UnitCellPropertyWidget::~UnitCellPropertyWidget()
{
    delete ui;
}

void UnitCellPropertyWidget::setZValue(int z)
{
    _unitCellItem->unitCell()->setZ(z);
    setMassDensity();
}

void UnitCellPropertyWidget::setUnitCellName()
{
    _unitCellItem->setData(ui->name->text(),Qt::DisplayRole);
}

void UnitCellPropertyWidget::setLatticeParams()
{
    double a = ui->a->value();
    double b = ui->b->value();
    double c = ui->c->value();
    double alpha = ui->alpha->value();
    double beta = ui->beta->value();
    double gamma = ui->gamma->value();

    try
    {
        _unitCellItem->unitCell()->setParams(a,b,c,alpha*nsx::deg,beta*nsx::deg,gamma*nsx::deg);
    } catch(const std::exception& e) {
        nsx::error() << e.what();
    }

    setMassDensity();

}

void UnitCellPropertyWidget::setMassDensity() const
{
    auto material=_unitCellItem->unitCell()->material();
    if (!material) {
        return;
    }

    double mm=material->molarMass();
    mm*=ui->z->value()/nsx::avogadro;
    double volume=_unitCellItem->unitCell()->volume()*nsx::ang3;
    material->setMassDensity(mm/volume);
}


void UnitCellPropertyWidget::updateCellParameters()
{
    auto unit_cell = _unitCellItem->unitCell();

    auto cell_params = unit_cell->character();

    ui->a->setValue(cell_params.a);
    ui->b->setValue(cell_params.b);
    ui->c->setValue(cell_params.c);
    ui->alpha->setValue(cell_params.alpha/nsx::deg);
    ui->beta->setValue(cell_params.beta/nsx::deg);
    ui->gamma->setValue(cell_params.gamma/nsx::deg);
}

void UnitCellPropertyWidget::getLatticeParams()
{
}

void UnitCellPropertyWidget::setChemicalFormula()
{
    auto formula = ui->chemicalFormula->text();

    try {
        nsx::sptrMaterial material(new nsx::Material(formula.toStdString()));
        _unitCellItem->unitCell()->setMaterial(material);
    } catch(std::exception& e) {
       nsx::error() << e.what();
   }

    setMassDensity();
}

void UnitCellPropertyWidget::setSpaceGroup(QString sg)
{
    std::string space_group = sg.toStdString();

    auto&& symbols = nsx::SpaceGroup::symbols();

    auto it = std::find(symbols.begin(),symbols.end(),space_group);

    // The space group does not exist, reset to the current value
    if (it == symbols.end()) {
        space_group = _unitCellItem->unitCell()->spaceGroup().symbol();
        ui->spaceGroup->setCurrentText(QString::fromStdString(space_group));
    }

    _unitCellItem->unitCell()->setSpaceGroup(space_group);
}

void UnitCellPropertyWidget::activateSpaceGroupCompletion(QString sg)
{
    ui->spaceGroup->setCurrentText(sg);
}

void UnitCellPropertyWidget::setIndexingTolerance(double tolerance)
{
    _unitCellItem->unitCell()->setIndexingTolerance(tolerance);
}

void UnitCellPropertyWidget::update(QStandardItem* item)
{
    Q_UNUSED(item)

    auto unit_cell = _unitCellItem->unitCell();

    ui->name->setText(QString::fromStdString(unit_cell->name()));

    ui->indexingTolerance->setValue(unit_cell->indexingTolerance());

    ui->z->setValue(unit_cell->z());

    auto material=_unitCellItem->unitCell()->material();
    if (material) {
        ui->chemicalFormula->setText(QString::fromStdString(unit_cell->material()->formula()));
    }

    ui->spaceGroup->setCurrentText(QString::fromStdString(unit_cell->spaceGroup().symbol()));

    updateCellParameters();
}
