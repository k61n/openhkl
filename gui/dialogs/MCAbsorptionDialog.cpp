//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/MCAbsorptionDialog.cpp
//! @brief     Implements class MCAbsorptionDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/MCAbsorptionDialog.h"

#include "core/experiment/DataSet.h"
#include "core/monte-carlo/MCAbsorption.h"
#include "gui/models/Session.h"
#include <QCR/widgets/actions.h>
#include <QCR/widgets/controls.h>
#include <QFormLayout>

MCAbsorptionDialog::MCAbsorptionDialog() : QDialog {}
{
    setAttribute(Qt::WA_DeleteOnClose);
    QFormLayout* layout = new QFormLayout(this);
    selUnitCell = new QcrCell<int>(0);
    int numUnitCells = gSession->selectedExperiment()->getUnitCellNames().size();
    QStringList a;
    for (int i = 0; i < numUnitCells; i++)
        a.append("crystal" + QString::number(i));
    QcrComboBox* crystals = new QcrComboBox("adhoc_mccrystals", selUnitCell, a);
    layout->addRow("Crystal reference:", crystals);
    numRaysCell = new QcrCell<int>(1000);
    QcrSpinBox* numRays = new QcrSpinBox("aadhoc_MCRays", numRaysCell, 5);
    layout->addRow("Number of rays:", numRays);
    QcrTextTriggerButton* runMC = new QcrTextTriggerButton("adhoc_runMC", "Run Monte Carlo");
    runMC->trigger()->setTriggerHook([=]() { runMCAbsorption(); });
    layout->addRow(runMC);
    progressBar = new QProgressBar(this);
    progressBar->setValue(0);
    layout->addRow(progressBar);

    show();
}

void MCAbsorptionDialog::runMCAbsorption()
{
    // nsx::sptrExperiment experiment = gSession->selectedExperiment()->experiment();

    // nsx::Diffractometer* diffractometer = experiment->diffractometer();

    // nsx::sptrUnitCell cell = gSession->selectedExperiment()->getUnitCell(selUnitCell->val());

    // xsection::Material* material = cell->material();

    // if (!material) {
    //     qWarning("No material defined for this crystal");
    //     return;
    // }

    // const nsx::Source& source = diffractometer->source();
    // const nsx::Monochromator& mono = source.selectedMonochromator();

    // const nsx::Sample& sample = diffractometer->sample();
    // const nsx::ConvexHull& hull = sample.shape();
    // if (!hull.checkEulerConditions()) {
    //     qWarning("The sample shape (hull) is ill-defined");
    //     return;
    // }

    // nsx::MCAbsorption mca(hull, mono.width(), mono.height(), -1.0);

    // mca.setMuAbsorption(material->muAbsorption());
    // mca.setMuScattering(material->muIncoherent());

    // progressBar->setValue(0);
    // progressBar->setTextVisible(true);

    // int progress = 0;

    // const std::map<std::string, nsx::sptrDataSet>& data = experiment->data();

    // for (auto& d : data) {
    //     const nsx::PeakList& peaks = gSession->selectedExperiment()->getPeakList(d.second);
    //     progressBar->setMaximum(peaks.size());
    //     progressBar->setFormat(
    //         QString::fromStdString(d.second->filename()) + ": " + QString::number(progress) +
    //         "%");
    //     for (const nsx::sptrPeak3D& p : peaks) {
    //         Eigen::Vector3d coord = p->shape().center();
    //         nsx::InterpolatedState state = d.second->interpolatedState(coord[2]);
    //         nsx::DirectVector position =
    //             d.second->reader()->diffractometer()->detector()->pixelPosition(coord[0],
    //             coord[1]);
    //         nsx::ReciprocalVector kf = state.kfLab(position);
    //         // todo: check coordinate systems here, may not be consistent
    //         double transmission =
    //             mca.run(numRaysCell->val(), kf.rowVector(), state.sampleOrientationMatrix());
    //         p->setTransmission(transmission);
    //         progressBar->setValue(++progress);
    //     }
    // }
}
