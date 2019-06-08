//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/PeaksItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <memory>

#include <QFileInfo>
#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QString>

#include "core/data/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Experiment.h"
#include "core/integration/GaussianIntegrator.h"
#include "core/data/IDataReader.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/logger/Logger.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/data/MetaData.h"
#include "core/crystal/PeakFilter.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/data/RawDataReader.h"
#include "core/instrument/Sample.h"
#include "core/crystal/UnitCell.h"

#include "apps/models/DataItem.h"
#include "apps/dialogs/DialogIntegrate.h"
#include "apps/dialogs/DialogMCAbsorption.h"
#include "apps/dialogs/DialogPeakFilter.h"
#include "apps/dialogs/DialogShapeLibrary.h"
#include "apps/dialogs/DialogSpaceGroup.h"
#include "apps/models/ExperimentItem.h"
#include "apps/frames/FrameAutoIndexer.h"
#include "apps/frames/FrameRefiner.h"
#include "apps/frames/FrameUserDefinedUnitCellIndexer.h"
#include "apps/opengl/GLSphere.h"
#include "apps/opengl/GLWidget.h"
#include "apps/models/InspectableTreeItem.h"
#include "apps/models/InstrumentItem.h"
#include "apps/models/LibraryItem.h"
#include "apps/MainWindow.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/NumorItem.h"
#include "apps/models/PeakListItem.h"
#include "apps/models/PeaksItem.h"
#include "apps/tree/PeaksPropertyWidget.h"
#include "apps/views/ProgressView.h"
#include "apps/models/SampleItem.h"
#include "apps/models/SessionModel.h"
#include "apps/models/UnitCellItem.h"
#include "apps/models/UnitCellsItem.h"

PeaksItem::PeaksItem() : InspectableTreeItem()
{
    setText("Peaks");

    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);

    setDragEnabled(false);
    setDropEnabled(false);

    setEditable(false);

    setSelectable(false);

    setCheckable(false);
}

QWidget* PeaksItem::inspectItem()
{
    return new PeaksPropertyWidget(this);
}

void PeaksItem::removeUnitCell(nsx::sptrUnitCell unit_cell)
{
    auto all_peaks = allPeaks();

    for (auto peak : all_peaks) {
        if (peak->unitCell() != unit_cell) {
            continue;
        }
        peak->setUnitCell(nullptr);
    }

    emit model()->signalUnitCellRemoved(unit_cell);
    emit model()->itemChanged(this);
}

nsx::PeakList PeaksItem::selectedPeaks()
{
    nsx::PeakList peaks;
    for (auto i = 0; i < rowCount(); ++i) {
        auto& list = dynamic_cast<PeakListItem&>(*child(i));

        if (list.checkState() != Qt::Checked) {
            continue;
        }

        for (auto&& peak : list.peaks()) {
            peaks.push_back(peak);
        }
    }
    return peaks;
}

void PeaksItem::normalizeToMonitor()
{
    bool ok;
    double factor = QInputDialog::getDouble(
        nullptr, "Enter normalization factor", "", 1.0e4, 1.0e-9, 1.0e9, 3, &ok);

    if (!ok) {
        return;
    }

    auto selected_peaks = selectedPeaks();

    for (auto peak : selected_peaks) {
        auto data = peak->data();
        if (!data) {
            continue;
        }

        double monitor = data->reader()->metadata().key<double>("monitor");

        peak->setScale(factor / monitor);
    }
    emit model()->itemChanged(this);
}

nsx::PeakList PeaksItem::allPeaks()
{
    nsx::PeakList peaks;
    for (auto i = 0; i < rowCount(); ++i) {
        auto& list = dynamic_cast<PeakListItem&>(*child(i));

        for (auto&& peak : list.peaks()) {
            peaks.push_back(peak);
        }
    }
    return peaks;
}

void PeaksItem::integratePeaks()
{
    ExperimentItem* exp_item = dynamic_cast<ExperimentItem*>(parent());
    auto&& selected_peaks = selectedPeaks();
    auto& library = exp_item->libraryItem()->library();

    if (!library) {
        throw std::runtime_error("Error: cannot integrate weak peaks without a shape library!");
    }

    nsx::info() << "Reintegrating peaks...";

    auto dialog = new DialogIntegrate(selected_peaks);

    std::map<std::string, std::function<nsx::IPeakIntegrator*()>> integrator_map;
    std::vector<std::string> integrator_names;

    integrator_map["Pixel sum integrator"] = [&]() {
        return new nsx::PixelSumIntegrator(dialog->fitCenter(), dialog->fitCov());
    };
    integrator_map["3d profile integrator"] = [&]() {
        return new nsx::Profile3DIntegrator(library, dialog->radius(), dialog->nframes(), false);
    };
    integrator_map["I/Sigma integrator"] = [&]() {
        return new nsx::ISigmaIntegrator(library, dialog->radius(), dialog->nframes());
    };
    integrator_map["1d Profile integrator"] = [&]() {
        return new nsx::Profile1DIntegrator(library, dialog->radius(), dialog->nframes());
    };
    integrator_map["Gaussian integrator"] = [&]() {
        return new nsx::GaussianIntegrator(dialog->fitCenter(), dialog->fitCov());
    };

    for (const auto& pair : integrator_map) {
        integrator_names.push_back(pair.first);
    }

    dialog->setIntegrators(integrator_names);

    if (!dialog->exec()) {
        nsx::info() << "Peak integration canceled.";
        return;
    }

    const double dmin = dialog->dMin();
    const double dmax = dialog->dMax();

    // nsx::DataList numors = _session->getSelectedNumors();
    auto&& numors = exp_item->dataItem()->selectedData();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(nullptr);
    view.watch(handler);

    nsx::PeakFilter peak_filter;
    // todo: bkg_begin and bkg_end
    auto peaks = peak_filter.dRange(selected_peaks, dmin, dmax);

    for (auto&& numor : numors) {
        nsx::info() << "Integrating " << peaks.size() << " peaks";
        std::unique_ptr<nsx::IPeakIntegrator> integrator(integrator_map[dialog->integrator()]());
        integrator->setHandler(handler);
        integrator->integrate(
            peaks, numor, library->peakScale(), library->bkgBegin(), library->bkgEnd());
    }

    nsx::info() << "Done reintegrating peaks";

    emit model()->itemChanged(this);
}

void PeaksItem::findSpaceGroup()
{
    std::unique_ptr<DialogSpaceGroup> dialog(new DialogSpaceGroup(selectedPeaks()));
    if (!dialog->exec()) {
        return;
    }
    emit model()->itemChanged(this);
}

void PeaksItem::showPeaksOpenGL()
{
    GLWidget* glw = new GLWidget();
    auto& scene = glw->getScene();
    auto peaks = selectedPeaks();

    for (auto peak : peaks) {
        GLSphere* sphere = new GLSphere("");
        Eigen::RowVector3d pos = peak->q().rowVector();
        sphere->setPos(pos[0] * 100, pos[1] * 100, pos[2] * 100);
        sphere->setColor(0, 1, 0);
        scene.addActor(sphere);
    }

    glw->show();
}

void PeaksItem::absorptionCorrection()
{
    // todo: check that this is correct!
    std::unique_ptr<DialogMCAbsorption> dialog(new DialogMCAbsorption(experimentItem()));
    if (!dialog->exec()) {
        return;
    }
}

void PeaksItem::buildShapeLibrary()
{
    nsx::PeakList peaks = selectedPeaks();
    ExperimentItem* exp_item = dynamic_cast<ExperimentItem*>(parent());

    // Check that a minimum number of peaks have been selected for indexing
    if (peaks.size() == 0) {
        QMessageBox::warning(nullptr, "NSXTool", "Need to selected peaks to fit profile!");
        return;
    }

    nsx::sptrUnitCell unit_cell(peaks[0]->unitCell());
    for (auto&& peak : peaks) {
        if (peak->unitCell() != unit_cell) {
            unit_cell = nullptr;
            break;
        }
    }

    if (unit_cell == nullptr) {
        QMessageBox::warning(
            nullptr, "NSXTool",
            "The selected peaks must have the same active unit "
            "cell for profile fitting");
        return;
    }

    std::unique_ptr<DialogShapeLibrary> dialog(
        new DialogShapeLibrary(experimentItem(), unit_cell, peaks));

    // rejected
    if (!dialog->exec()) {
        return;
    }

    auto new_library = dialog->library();

    if (!new_library) {
        return;
    }

    exp_item->libraryItem()->library() = new_library;
    nsx::info() << "Update profiles of " << peaks.size() << " peaks";

    emit model()->itemChanged(this);
}

void PeaksItem::openPeakFilterDialog()
{
    auto&& selected_peaks = selectedPeaks();

    DialogPeakFilter* dialog = DialogPeakFilter::create(experimentItem(), selected_peaks);

    dialog->show();

    dialog->raise();
}

void PeaksItem::openAutoIndexingFrame()
{
    auto&& selected_peaks = selectedPeaks();

    FrameAutoIndexer* frame_autoindexer =
        FrameAutoIndexer::create(experimentItem(), selected_peaks);

    frame_autoindexer->show();

    frame_autoindexer->raise();
}

void PeaksItem::openUserDefinedUnitCellIndexerFrame()
{
    auto&& selected_peaks = selectedPeaks();

    FrameUserDefinedUnitCellIndexer* frame =
        FrameUserDefinedUnitCellIndexer::create(experimentItem(), selected_peaks);

    frame->show();

    frame->raise();
}

void PeaksItem::refine()
{
    nsx::PeakList selected_peaks = selectedPeaks();

    FrameRefiner* frame_refiner = FrameRefiner::create(experimentItem(), selected_peaks);

    frame_refiner->show();

    frame_refiner->raise();
}

void PeaksItem::autoAssignUnitCell()
{
    auto&& peaks = selectedPeaks();

    auto unit_cells_item = experimentItem()->unitCellsItem();

    auto&& cells = unit_cells_item->unitCells();

    if (cells.size() < 1) {
        nsx::info() << "There are no unit cells to assign";
        return;
    }

    for (auto peak : peaks) {
        if (!peak->enabled()) {
            continue;
        }

        Eigen::RowVector3d hkl;
        bool assigned = false;

        for (auto cell : cells) {
            nsx::MillerIndex hkl(peak->q(), *cell);
            if (hkl.indexed(cell->indexingTolerance())) {
                peak->setUnitCell(cell);
                assigned = true;
                break;
            }
        }

        // could not assign unit cell
        if (assigned == false) {
            peak->setSelected(false);
        }
    }
    nsx::debug() << "Done auto assigning unit cells";

    emit model()->itemChanged(this);
}
