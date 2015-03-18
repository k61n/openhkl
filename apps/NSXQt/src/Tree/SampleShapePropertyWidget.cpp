#include <map>
#include <set>

#include <QtDebug>
#include <QMessageBox>

#include "Diffractometer.h"
#include "Gonio.h"
#include "IData.h"
#include "IMaterial.h"
#include "Logger.h"
#include "IMaterial.h"
#include "MaterialManager.h"
#include "MCAbsorption.h"
#include "Peak3D.h"
#include "Sample.h"
#include "Source.h"

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
    // When editing the Z value is finished, set it to the corresponding unit cell
    connect(ui->spinBox_Z,SIGNAL(editingFinished()),this,SLOT(setZ()));
    connect(ui->comboBox_CellChoices,SIGNAL(currentIndexChanged(int)),this,SLOT(changeUnitCell(int)));
    setHullProperties();
    int ncrystals=_caller->getExperiment()->getDiffractometer()->getSample()->getNCrystals();
    for (int i=0;i<ncrystals;++i)
        ui->comboBox_CellChoices->addItem("Crystal "+QString::number(i+1));

    changeUnitCell(ui->comboBox_CellChoices->currentIndex());
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
        unsigned int cellIndex=static_cast<unsigned int>(ui->comboBox_CellChoices->currentIndex());
        _caller->getExperiment()->getDiffractometer()->getSample()->setMaterial(material,cellIndex);
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
    SX::Instrument::Sample* sample=_caller->getExperiment()->getDiffractometer()->getSample();
    auto& hull=sample->getShape();
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

    // HERE FOR TESTING PURPOSES

    // Get the source
    SX::Instrument::Source* source=_caller->getExperiment()->getDiffractometer()->getSource();
    if (!source)
        return true;

    // Get the material
    unsigned int cellIndex=static_cast<unsigned int>(ui->comboBox_CellChoices->currentIndex());
    SX::Chemistry::sptrMaterial material=sample->getMaterial(cellIndex);

    SX::Geometry::MCAbsorption mca(source->getWidth(),source->getHeight(),source->getRestPosition()[1]);
    mca.setSample(hull,material->getMuScattering(),material->getMuAbsorption(source->getWavelength()));

    std::map<std::string,IData*> data=_caller->getExperiment()->getData();

    for (auto& d: data)
    {
        const auto& peaks=d.second->getPeaks();
        for (auto& p: peaks)
        {
            auto hommat=sample->getGonio()->getHomMatrix(p->getSampleState()->getValues());
            mca.run(1000,p->getKf(),hommat.rotation());
        }
    }

    return true;
}

void SampleShapePropertyWidget::setZ()
{
    unsigned int cellIndex=static_cast<unsigned int>(ui->comboBox_CellChoices->currentIndex());
    unsigned int Z=static_cast<unsigned int>(ui->spinBox_Z->value());
    _caller->getExperiment()->getDiffractometer()->getSample()->setZ(Z,cellIndex);
}

void SampleShapePropertyWidget::changeUnitCell(int idx)
{
    auto material=_caller->getExperiment()->getDiffractometer()->getSample()->getMaterial(idx);
    if (material)
        ui->lineEdit_ChemicalFormula->setText(QString::fromStdString(material->getName()));
    unsigned int Z=_caller->getExperiment()->getDiffractometer()->getSample()->getZ(idx);
    ui->spinBox_Z->setValue(Z);
}
