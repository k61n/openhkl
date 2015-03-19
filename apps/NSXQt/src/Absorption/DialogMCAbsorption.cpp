#include "include/Absorption/DialogMCAbsorption.h"
#include "ui_DialogMCAbsorption.h"
#include "Experiment.h"
#include "Diffractometer.h"
#include "Sample.h"
#include "MCAbsorption.h"
#include "Source.h"
#include <QMessageBox>
#include "IData.h"
#include "Peak3D.h"
#include <Eigen/Dense>
#include <QtDebug>
#include "Gonio.h"
#include "Units.h"

DialogMCAbsorption::DialogMCAbsorption(SX::Instrument::Experiment *experiment, QWidget *parent):
    _experiment(experiment),
    QDialog(parent),
    ui(new Ui::DialogMCAbsorption)
{
    ui->setupUi(this);
    auto ncrystals=_experiment->getDiffractometer()->getSample()->getNCrystals();
    if (ncrystals>0)
    {
        ui->comboBox->setEnabled(true);
        for (unsigned int i=0;i<ncrystals;++i)
        {
            ui->comboBox->addItem("Crystal"+QString::number(i+1));
        }
    }
}

DialogMCAbsorption::~DialogMCAbsorption()
{
    delete ui;
}

void DialogMCAbsorption::on_pushButton_run_pressed()
{

    if (!ui->comboBox->isEnabled())
        return;

    // Get the source
    SX::Instrument::Source* source=_experiment->getDiffractometer()->getSource();
    SX::Instrument::Sample* sample=_experiment->getDiffractometer()->getSample();

    // Get the material
    unsigned int cellIndex=static_cast<unsigned int>(ui->comboBox->currentIndex());
    SX::Chemistry::sptrMaterial material=sample->getMaterial(cellIndex);

    SX::Geometry::MCAbsorption mca(source->getWidth(),source->getHeight(),-1.0);
    auto& hull=sample->getShape();
    if (!hull.checkEulerConditions())
    {
            QMessageBox::critical(this,"NSXTOOL","The sample shape (hull) is ill-defined");
            return;
    }

    mca.setSample(&hull,material->getMuScattering(),material->getMuAbsorption(source->getWavelength()*SX::Units::ang));

    const auto& data=_experiment->getData();

    for (auto& d: data)
    {
        const auto& peaks=d.second->getPeaks();
        for (auto& p: peaks)
        {
            Eigen::Transform<double,3,2> hommat=sample->getGonio()->getHomMatrix(p->getSampleState()->getValues());
            Eigen::Matrix3d rot=hommat.rotation();
            qDebug() << mca.run(ui->spinBox->value(),p->getKf(),rot);
        }
    }
}
