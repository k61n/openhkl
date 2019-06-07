#include <memory>

#include <QMessageBox>

#include <Eigen/Dense>

#include <core/DataSet.h>
#include <core/Detector.h>
#include <core/Diffractometer.h>
#include <core/Experiment.h>
#include <core/Gonio.h>
#include <core/IDataReader.h>
#include <core/InstrumentState.h>
#include <core/MCAbsorption.h>
#include <core/Material.h>
#include <core/Monochromator.h>
#include <core/Peak3D.h>
#include <core/ReciprocalVector.h>
#include <core/Sample.h>
#include <core/Source.h>
#include <core/UnitCell.h>
#include <core/Units.h>

#include "DialogMCAbsorption.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "UnitCellsItem.h"

#include "ui_DialogMCAbsorption.h"

DialogMCAbsorption::DialogMCAbsorption(ExperimentItem* experiment_item, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogMCAbsorption), _experiment_item(experiment_item)
{
    ui->setupUi(this);

    auto unit_cells_item = _experiment_item->unitCellsItem();

    auto&& cells = unit_cells_item->unitCells();

    if (cells.size() > 0) {
        ui->unitCells->setEnabled(true);
        for (unsigned int i = 0; i < cells.size(); ++i) {
            ui->unitCells->addItem("Crystal" + QString::number(i + 1));
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
    if (!ui->unitCells->isEnabled()) {
        return;
    }

    auto experiment = _experiment_item->experiment();

    auto diffractometer = experiment->diffractometer();

    auto unit_cells_item = _experiment_item->unitCellsItem();
    auto&& cells = unit_cells_item->unitCells();
    auto cell = cells[ui->unitCells->currentIndex()];

    auto material = cell->material();

    if (material == nullptr) {
        QMessageBox::critical(this, "NSXTOOL", "No material defined for this crystal");
        return;
    }

    const auto& source = diffractometer->source();
    const auto& mono = source.selectedMonochromator();

    const auto& sample = diffractometer->sample();
    const auto& hull = sample.shape();
    if (!hull.checkEulerConditions()) {
        QMessageBox::critical(this, "NSXTOOL", "The sample shape (hull) is ill-defined");
        return;
    }

    nsx::MCAbsorption mca(hull, mono.width(), mono.height(), -1.0);

    mca.setMuAbsorption(material->muAbsorption());
    mca.setMuScattering(material->muIncoherent());

    ui->progressBar_MCStatus->setValue(0);
    ui->progressBar_MCStatus->setTextVisible(true);

    int progress = 0;

    auto session = dynamic_cast<SessionModel*>(_experiment_item->model());

    const auto& data = experiment->data();

    for (auto& d : data) {
        const auto& peaks = session->peaks(d.second);
        ui->progressBar_MCStatus->setMaximum(peaks.size());
        ui->progressBar_MCStatus->setFormat(
            QString::fromStdString(d.second->filename()) + ": " + QString::number(progress) + "%");
        for (auto& p : peaks) {
            auto data = p->data();
            auto coord = p->shape().center();
            auto state = data->interpolatedState(coord[2]);
            auto position =
                data->reader()->diffractometer()->detector()->pixelPosition(coord[0], coord[1]);
            auto kf = state.kfLab(position);
            // todo: check coordinate systems here, may not be consistent
            double transmission =
                mca.run(ui->spinBox->value(), kf.rowVector(), state.sampleOrientationMatrix());
            p->setTransmission(transmission);
            ui->progressBar_MCStatus->setValue(++progress);
        }
    }
}
