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
    int ncrystals=_caller->getExperiment()->getDiffractometer()->getSample()->getNCrystals();
    for (int i=0;i<ncrystals;++i)
        ui->comboBox_CellChoices->addItem("Crystal "+QString::number(i+1));

    auto material=_caller->getExperiment()->getDiffractometer()->getSample()->getMaterial();
    if (material)
    {
        ui->lineEdit_ChemicalFormula->setText(QString::fromStdString(material->getName()));
    }

}

SampleShapePropertyWidget::~SampleShapePropertyWidget()
{
    delete ui;
}

void SampleShapePropertyWidget::setChemicalFormula(const QString &formula)
{
    auto mgr=SX::Chemistry::MaterialManager::Instance();
    try
    {
        auto material=mgr->buildMaterialFromChemicalFormula(formula.toStdString(),SX::Chemistry::ChemicalState::Solid);
       _caller->getExperiment()->getDiffractometer()->getSample()->setMaterial(material);
        qDebug() << "" <<*material;
    }
    catch(std::exception& e)
    {
       QMessageBox::critical(this, tr("NSXTool"),tr(e.what()));
        return;
   }
}

void SampleShapePropertyWidget::on_pushButton_LoadMovie_clicked()
{
    AbsorptionDialog* dialog=new AbsorptionDialog(_caller->getExperiment(),nullptr);
    if (!dialog->exec())
        if (setHullProperties())
            ui->lineEdit_MovieFilename->setText(QString::fromStdString(dialog->getMovieFilename()));
}

bool SampleShapePropertyWidget::setHullProperties()
{
    auto& hull=_caller->getExperiment()->getDiffractometer()->getSample()->getShape();
    if (!hull.checkEulerConditions())
        return false;

    // The hull is translated to its center
    hull.translateToCenter();
    // The hull is rotated of -90 along chi axis (Y axis)
    Eigen::Matrix3d mat;
    mat << 0, 0, 1,
           0, 1, 0,
          -1, 0, 0;

    hull.rotate(mat);
    qDebug() << "Coordinates of the Hull at rest:" << hull;

    ui->lineEdit_Volume->setText(QString::number(hull.getVolume())+" mm^3");
    ui->lineEdit_Faces->setText(QString::number(hull.getNFaces()));
    ui->lineEdit_Edges->setText(QString::number(hull.getNEdges()));
    ui->lineEdit_Vertices->setText(QString::number(hull.getNVertices()));


    return true;
}
