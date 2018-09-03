#include <memory>

#include <QFileInfo>
#include <QIcon>
#include <QMessageBox>
#include <QStandardItem>
#include <QString>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/GaussianIntegrator.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/ISigmaIntegrator.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MeanBackgroundIntegrator.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/Profile1DIntegrator.h>
#include <nsxlib/Profile3DIntegrator.h>
#include <nsxlib/RawDataReader.h>
#include <nsxlib/Sample.h>
#include <nsxlib/PixelSumIntegrator.h>
#include <nsxlib/UnitCell.h>

#include "DataItem.h"
#include "DialogAutoIndexing.h"
#include "DialogIntegrate.h"
#include "DialogMCAbsorption.h"
#include "DialogPeakFilter.h"
#include "DialogShapeLibrary.h"
#include "DialogRefineUnitCell.h"
#include "DialogSpaceGroup.h"
#include "ExperimentItem.h"
#include "GLSphere.h"
#include "GLWidget.h"
#include "InspectableTreeItem.h"
#include "InstrumentItem.h"
#include "LibraryItem.h"
#include "MetaTypes.h"
#include "NumorItem.h"
#include "PeakListItem.h"
#include "PeaksItem.h"
#include "PeaksPropertyWidget.h"
#include "ProgressView.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "UnitCellItem.h"
#include "UnitCellsItem.h"

PeaksItem::PeaksItem(): InspectableTreeItem()
{
    setText("Peaks");
    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
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

        for (auto&& peak: list.peaks()) {
            peaks.push_back(peak);
        }
    }
    return peaks;
}

nsx::PeakList PeaksItem::allPeaks()
{
    nsx::PeakList peaks;
    for (auto i = 0; i < rowCount(); ++i) {
        auto& list = dynamic_cast<PeakListItem&>(*child(i));

        for (auto&& peak: list.peaks()) {
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
    
    integrator_map["Pixel sum integrator"] = [&]() {return new nsx::PixelSumIntegrator(dialog->fitCenter(), dialog->fitCov());};
    integrator_map["3d profile integrator"] = [&]() {return new nsx::Profile3DIntegrator(library, dialog->radius(), dialog->nframes(), false);};
    integrator_map["I/Sigma integrator"] = [&]() {return new nsx::ISigmaIntegrator(library, dialog->radius(), dialog->nframes());};
    integrator_map["1d Profile integrator"] = [&]() {return new nsx::Profile1DIntegrator(library, dialog->radius(), dialog->nframes());};
    integrator_map["Gaussian integrator"] = [&]() {return new nsx::GaussianIntegrator(dialog->fitCenter(), dialog->fitCov());};

    for (const auto& pair: integrator_map) {
        integrator_names.push_back(pair.first);
    }

    dialog->setIntegrators(integrator_names);

    if (!dialog->exec()) {
        nsx::info() << "Peak integration canceled.";
        return;
    }

    const double dmin = dialog->dMin();
    const double dmax = dialog->dMax();

    //nsx::DataList numors = _session->getSelectedNumors();
    auto&& numors = exp_item->dataItem()->selectedData();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(nullptr);
    view.watch(handler);

    nsx::PeakFilter peak_filter;
    // todo: bkg_begin and bkg_end
    auto peaks = peak_filter.dRange(selected_peaks, dmin, dmax);

    for (auto&& numor: numors) {
        nsx::info() << "Integrating " << peaks.size() << " peaks";
        std::unique_ptr<nsx::IPeakIntegrator> integrator(integrator_map[dialog->integrator()]());
        integrator->setHandler(handler);
        integrator->integrate(peaks, numor, library->peakScale(), library->bkgBegin(), library->bkgEnd());
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

    for (auto peak: peaks) {
        GLSphere* sphere=new GLSphere("");
        Eigen::RowVector3d pos = peak->q().rowVector();
        sphere->setPos(pos[0]*100,pos[1]*100,pos[2]*100);
        sphere->setColor(0,1,0);
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
        QMessageBox::warning(nullptr, "NSXTool","Need to selected peaks to fit profile!");
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
        QMessageBox::warning(nullptr, "NSXTool", "The selected peaks must have the same active unit cell for profile fitting");
        return;
    }

    std::unique_ptr<DialogShapeLibrary> dialog(new DialogShapeLibrary(experimentItem(), unit_cell, peaks));

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

void PeaksItem::filterPeaks()
{
    auto&& selected_peaks = selectedPeaks();
    std::unique_ptr<DialogPeakFilter> dialog(new DialogPeakFilter(selected_peaks));

    if (!dialog->exec()) {
        return;
    }    

    auto&& filtered_peaks = dialog->filteredPeaks();

    if (filtered_peaks.empty()) {
        return;
    }

    auto peak_list = new PeakListItem(filtered_peaks);

    nsx::info()<<"Applied peak filters on selected peaks. Remains "<<filtered_peaks.size()<<" out of "<<selected_peaks.size()<<" peaks";

    peak_list->setText("Filtered peaks");

    appendRow(peak_list);

    emit model()->itemChanged(this);
}

void PeaksItem::autoindex()
{
    nsx::PeakList peaks = selectedPeaks();

    std::unique_ptr<DialogAutoIndexing> dialog(new DialogAutoIndexing(experimentItem(), peaks));

    if (!dialog->exec()) {
        return;
    }

    emit model()->itemChanged(this);
}

void PeaksItem::refine()
{
    nsx::PeakList peaks = selectedPeaks();
    int nPeaks = peaks.size();
    // Check that a minimum number of peaks have been selected for indexing
    if (nPeaks < 10) {
        QMessageBox::warning(nullptr, "NSXTool", "Need at least 10 peaks for refining");
        return;
    }

    nsx::sptrUnitCell uc(peaks[0]->unitCell());
    for (auto&& peak : peaks) {
        if (peak->unitCell() != uc) {
            uc = nullptr;
            break;
        }
    }

    if (uc == nullptr) {
        QMessageBox::warning(nullptr, "NSXTool", "The selected peaks must have the same active unit cell for refining");
        return;
    }

    std::unique_ptr<DialogRefineUnitCell> dialog(new DialogRefineUnitCell(experiment(),uc,peaks,nullptr));
    if (!dialog->exec()) {
        return;
    }

    emit model()->itemChanged(this);
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

    for (auto peak: peaks) {
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
