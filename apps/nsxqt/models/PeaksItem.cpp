#include <QIcon>
#include <QFileInfo>
#include <QJsonArray>
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
#include <nsxlib/RawDataReader.h>
#include <nsxlib/StrongPeakIntegrator.h>
#include <nsxlib/WeakPeakIntegrator.h>

#include "DataItem.h"
#include "DialogIntegrate.h"
#include "DialogSpaceGroup.h"
#include "ExperimentItem.h"
#include "PeaksItem.h"
#include "PeakListItem.h"
#include "ProgressView.h"
#include "NumorItem.h"

PeaksItem::PeaksItem(): TreeItem()
{
    setText("Peaks");
    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
}

PeakListItem* PeaksItem::createPeaksItem(const char* name)
{
    auto item = new PeakListItem;
    item->setText(name);
    appendRow(item);   void integratePeaks(); 
    return item;
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


void PeaksItem::integratePeaks()
{
    ExperimentItem& exp_item = dynamic_cast<ExperimentItem&>(*parent());
    auto selected_peaks = selectedPeaks();
    auto library = exp_item.library();

    if (!library) {
        throw std::runtime_error("Error: cannot integrate weak peaks without a shape library!");
    }

    nsx::info() << "Reintegrating peaks...";

    auto dialog = new DialogIntegrate();

    std::map<std::string, std::function<nsx::IPeakIntegrator*()>> integrator_map;
    std::vector<std::string> integrator_names;
    
    integrator_map["Strong peak integrator"] = [&]() {return new nsx::StrongPeakIntegrator(dialog->fitCenter(), dialog->fitCov());};
    integrator_map["Weak peak integrator"] = [&]() {return new nsx::WeakPeakIntegrator(library, dialog->radius(), dialog->nframes(), false);};
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

    const double peak_scale = dialog->peakScale();
    const double bkgBegin = dialog->bkgBegin();
    const double bkgEnd = dialog->bkgEnd();
    const double dmin = dialog->dMin();
    const double dmax = dialog->dMax();

    //nsx::DataList numors = _session->getSelectedNumors();
    auto numors = exp_item.dataItem()->selectedData();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(nullptr);
    view.watch(handler);

    for (auto&& numor: numors) {
        // todo: bkg_begin and bkg_end
        auto peaks = nsx::PeakFilter().dMin(selected_peaks, dmin);
        peaks = nsx::PeakFilter().dMax(peaks, dmax);
        nsx::info() << "Integrating " << peaks.size() << " peaks";
        std::unique_ptr<nsx::IPeakIntegrator> integrator(integrator_map[dialog->integrator()]());
        integrator->setHandler(handler);
        integrator->integrate(peaks, numor, peak_scale, bkgBegin, bkgEnd);
    }

    // todo: emit signal
    nsx::info() << "Done reintegrating peaks";
}

void PeaksItem::findSpaceGroup()
{
    DialogSpaceGroup dialog(selectedPeaks());
    dialog.exec();
}
