#include "include/SampleShapePropertyWidget.h"
#include "ui_SampleShapePropertyWidget.h"
#include "Absorption/AbsorptionDialog.h"
#include "SampleShapeItem.h"
#include "MaterialManager.h"
#include "IMaterial.h"
#include <QMessageBox>
#include "Logger.h"
#include <QtDebug>

SampleShapePropertyWidget::SampleShapePropertyWidget(SampleShapeItem* caller,QWidget *parent) :
    _caller(caller),
    QWidget(parent),
    ui(new Ui::SampleShapePropertyWidget)
{
    ui->setupUi(this);
    // When editing of text is finished, try to parse this as a chemical formula
    connect(ui->lineEdit_ChemicalFormula,&QLineEdit::returnPressed,[=](){setChemicalFormula(ui->lineEdit_ChemicalFormula->text());});
}

SampleShapePropertyWidget::~SampleShapePropertyWidget()
{
    delete ui;
}

void SampleShapePropertyWidget::on_pushButton_Info_clicked()
{
    AbsorptionDialog* dialog=new AbsorptionDialog(_caller->getExperiment(),nullptr);
    dialog->exec();
}

void SampleShapePropertyWidget::setChemicalFormula(const QString &formula)
{
    auto mgr=SX::Chemistry::MaterialManager::Instance();
    SX::Chemistry::sptrMaterial material;
    try
    {
    material=mgr->buildMaterialFromChemicalFormula(formula.toStdString(),SX::Chemistry::IMaterial::State::Solid);
    }catch(std::exception& e)
    {
        QMessageBox::critical(this, tr("NSXTool"),
                              tr(e.what()));
        return;
    }

    qDebug() << "" << *material;
}
