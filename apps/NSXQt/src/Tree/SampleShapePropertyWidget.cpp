#include <QtDebug>
#include <QMessageBox>

#include "IMaterial.h"
#include "Logger.h"
#include "MaterialManager.h"
#include "Sample.h"
#include "Diffractometer.h"

#include "ui_SampleShapePropertyWidget.h"
#include "Absorption/AbsorptionDialog.h"
#include "Tree/SampleShapeItem.h"
#include "Tree/SampleShapePropertyWidget.h"

SampleShapePropertyWidget::SampleShapePropertyWidget(SampleShapeItem* caller,QWidget *parent) :
    _caller(caller),
    QWidget(parent),
    ui(new Ui::SampleShapePropertyWidget)
{
    ui->setupUi(this);
    // When editing of text is finished, try to parse this as a chemical formula
    connect(ui->lineEdit_ChemicalFormula,&QLineEdit::returnPressed,[=](){setChemicalFormula(ui->lineEdit_ChemicalFormula->text());});
    setHullProperties();
}

SampleShapePropertyWidget::~SampleShapePropertyWidget()
{
    delete ui;
}

void SampleShapePropertyWidget::setChemicalFormula(const QString &formula)
{
    auto mgr=SX::Chemistry::MaterialManager::Instance();
    SX::Chemistry::sptrMaterial material;
    try
    {
    material=mgr->buildMaterialFromChemicalFormula(formula.toStdString(),SX::Chemistry::ChemicalState::Solid);
    }catch(std::exception& e)
    {
        QMessageBox::critical(this, tr("NSXTool"),tr(e.what()));
        return;
    }

    qDebug() << "" << *material;
}

void SampleShapePropertyWidget::on_pushButton_LoadMovie_clicked()
{
    AbsorptionDialog* dialog=new AbsorptionDialog(_caller->getExperiment(),nullptr);
    if (!dialog->exec())
      setHullProperties();
}

void SampleShapePropertyWidget::setHullProperties()
{
    auto& hull=_caller->getExperiment()->getDiffractometer()->getSample()->getShape();
    if (!hull.checkEulerConditions())
        return;
    hull.translateToCenter();
    ui->lineEdit_Volume->setText(QString::number(hull.getVolume())+" mm^3");
    ui->lineEdit_Faces->setText(QString::number(hull.getNFaces()));
    ui->lineEdit_Edges->setText(QString::number(hull.getNEdges()));
    ui->lineEdit_Vertices->setText(QString::number(hull.getNVertices()));
}
