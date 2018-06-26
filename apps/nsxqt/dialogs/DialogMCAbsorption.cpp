#include <memory>

#include <QMessageBox>

#include <Eigen/Dense>

#include <nsxlib/DataSet.h>

#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Material.h>
#include <nsxlib/MCAbsorption.h>
#include <nsxlib/Monochromator.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "DialogMCAbsorption.h"

#include "ui_DialogMCAbsorption.h"

DialogMCAbsorption::DialogMCAbsorption(SessionModel* session, nsx::sptrExperiment experiment, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogMCAbsorption),
    _experiment(experiment),
    _session(session)
{
    ui->setupUi(this);
    auto sample = _experiment->diffractometer()->sample();
    const auto& cells = sample->unitCells();
    
    if (cells.size() > 0) {
        ui->comboBox->setEnabled(true);
        for (unsigned int i = 0; i < cells.size(); ++i) {
            ui->comboBox->addItem("Crystal"+QString::number(i+1));
        }
    }
    ui->progressBar_MCStatus->setValue(0);
}

DialogMCAbsorption::~DialogMCAbsorption()
{
    delete ui;
}

void DialogMCAbsorption::on_pushButton_run_pressed()
{
    if (!ui->comboBox->isEnabled()) {
        return;
    }
    // Get the source
    auto source=_experiment->diffractometer()->source();
    auto sample=_experiment->diffractometer()->sample();

    // Get the material
    unsigned int cellIndex=static_cast<unsigned int>(ui->comboBox->currentIndex());
    auto cell = sample->unitCells()[cellIndex];
    auto material = cell->material();

    if (material == nullptr) {
        QMessageBox::critical(this,"NSXTOOL","No material defined for this crystal");
        return;
    }

    auto& mono = source->selectedMonochromator();

    nsx::MCAbsorption mca(mono.width(),mono.height(),-1.0);
    auto& hull=sample->shape();
    if (!hull.checkEulerConditions()) {
        QMessageBox::critical(this,"NSXTOOL","The sample shape (hull) is ill-defined");
        return;
    }

    mca.setSample(&hull,material->muIncoherent(),material->muAbsorption(mono.wavelength()*nsx::ang));
    const auto& data=_experiment->data();
    ui->progressBar_MCStatus->setValue(0);
    ui->progressBar_MCStatus->setTextVisible(true);
    int progress=0;

    for (auto& d: data) {
        const auto& peaks = _session->peaks(d.second.get());
        ui->progressBar_MCStatus->setMaximum(peaks.size());
        ui->progressBar_MCStatus->setFormat(QString::fromStdString(d.second->filename()) + ": "+QString::number(progress)+"%");
        for (auto& p: peaks) {
            auto data = p->data();
            auto coord = p->shape().center();
            auto state = data->interpolatedState(coord[2]);
            auto position = data->diffractometer()->detector()->pixelPosition(coord[0], coord[1]);
            auto kf = state.kfLab(position);
            // todo: check coordinate systems here, may not be consistent
            double transmission=mca.run(ui->spinBox->value(),kf.rowVector(),state.sampleOrientationMatrix());
            p->setTransmission(transmission);
            ui->progressBar_MCStatus->setValue(++progress);
        }
    }
}
