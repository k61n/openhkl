#include <QtDebug>
#include <QMessageBox>

#include <memory>

#include <Eigen/Dense>

#include "MCAbsorptionDialog.h"
#include "ui_MCAbsorptionDialog.h"
#include "Experiment.h"
#include "Diffractometer.h"
#include "Sample.h"
#include "MCAbsorption.h"
#include "Source.h"
#include "IData.h"
#include <nsxlib/crystal/Peak3D.h>
#include "Gonio.h"
#include "Units.h"
#include "Monochromator.h"

MCAbsorptionDialog::MCAbsorptionDialog(std::shared_ptr<SX::Instrument::Experiment> experiment, QWidget *parent):
    QDialog(parent),
     ui(new Ui::MCAbsorptionDialog),
    _experiment(experiment)
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
    ui->progressBar_MCStatus->setValue(0);
}

MCAbsorptionDialog::~MCAbsorptionDialog()
{
    delete ui;
}

void MCAbsorptionDialog::on_pushButton_run_pressed()
{

    if (!ui->comboBox->isEnabled())
        return;

    // Get the source
    std::shared_ptr<SX::Instrument::Source> source=_experiment->getDiffractometer()->getSource();
    std::shared_ptr<SX::Instrument::Sample> sample=_experiment->getDiffractometer()->getSample();

    // Get the material
    unsigned int cellIndex=static_cast<unsigned int>(ui->comboBox->currentIndex());
    SX::Chemistry::sptrMaterial material=sample->getMaterial(cellIndex);
    if (material==nullptr)
    {
            QMessageBox::critical(this,"NSXTOOL","No material defined for this crystal");
            return;
    }

    SX::Geometry::MCAbsorption mca(source->getSelectedMonochromator()->getWidth(),source->getSelectedMonochromator()->getHeight(),-1.0);
    auto& hull=sample->getShape();
    if (!hull.checkEulerConditions())
    {
            QMessageBox::critical(this,"NSXTOOL","The sample shape (hull) is ill-defined");
            return;
    }

    mca.setSample(&hull,material->getMuScattering(),material->getMuAbsorption(source->getWavelength()*SX::Units::ang));

    const auto& data=_experiment->getData();

    ui->progressBar_MCStatus->setValue(0);
    ui->progressBar_MCStatus->setTextVisible(true);

    int progress=0;
    for (auto& d: data)
    {
        const auto& peaks=d.second->getPeaks();
        ui->progressBar_MCStatus->setMaximum(peaks.size());
        ui->progressBar_MCStatus->setFormat(QString::fromStdString(d.second->getBasename()) + ": "+QString::number(progress)+"%");
        for (auto& p: peaks)
        {
            Eigen::Transform<double,3,2> hommat=sample->getGonio()->getHomMatrix(p->getSampleState()->getValues());
            Eigen::Matrix3d rot=hommat.rotation();
            double transmission=mca.run(ui->spinBox->value(),p->getKf(),rot);
            p->setTransmission(transmission);
            ui->progressBar_MCStatus->setValue(++progress);
        }
    }
}
