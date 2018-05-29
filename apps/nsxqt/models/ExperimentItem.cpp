#include <fstream>

#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>

#include <nsxlib/CC.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MergedData.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/ResolutionShell.h>
#include <nsxlib/RFactor.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>

#include "DataItem.h"
#include "DetectorItem.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "LibraryItem.h"
#include "LogFileDialog.h"
#include "PeaksItem.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "TreeItem.h"

ExperimentItem::ExperimentItem(nsx::sptrExperiment experiment): TreeItem(), _experiment(experiment)
{
    setText(QString::fromStdString(experiment->getName()));
    setForeground(QBrush(QColor("blue")));
    QIcon icon(":/resources/experimentIcon.png");
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
    setEditable(true);

    auto diff = experiment->getDiffractometer();
    _instr = new InstrumentItem(diff->getName().c_str(), diff->getSource()->getName().c_str());

    // Add the instrument item to the experiment item
    appendRow(_instr);

    // Create a data item and add it to the experiment item
    _data = new DataItem;
    appendRow(_data);

    // Create a peaks item and add it to the experiment item
    _peaks = new PeaksItem;
    appendRow(_peaks);

    // Create the reference peak library
    _library = new LibraryItem;
    appendRow(_library);
}

QJsonObject ExperimentItem::toJson()
{
    auto exp_ptr = experiment();
    QJsonObject experiment;

    experiment["name"] = QString(exp_ptr->getName().c_str());
    experiment["instrument"] = _instr->toJson();
    experiment["data"] = _data->toJson();

    return experiment;
}

void ExperimentItem::fromJson(const QJsonObject &obj)
{
    _instr->fromJson(obj["instrument"].toObject());
    _data->fromJson(obj["data"].toObject());
}

InstrumentItem* ExperimentItem::getInstrumentItem()
{
    return _instr;
}

void ExperimentItem::writeLogFile()
{
    LogFileDialog dlg;

    if (!dlg.exec()) {
        return;
    }

    auto&& peaks = _peaks->selectedPeaks();
    writeStatistics(dlg.statisticsFilename(), peaks, dlg.dmin(), dlg.dmax(), dlg.numShells(), dlg.friedel());
}


bool ExperimentItem::writeStatistics(std::string filename,
                                    const nsx::PeakList& peaks,
                                    double dmin, double dmax, unsigned int num_shells, bool friedel)
{
    std::fstream file(filename, std::ios::out);
    std::vector<char> buf(1024, 0); // buffer for snprintf

    Eigen::RowVector3d HKL(0.0, 0.0, 0.0);

    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return false;
    }

    if (peaks.size() == 0) {
        nsx::error() << "No peaks to write to log!";
        return false;
    }

    nsx::PeakFilter peak_filter;
    nsx::PeakList filtered_peaks;
    filtered_peaks = peak_filter.selected(peaks,true);
    filtered_peaks = peak_filter.hasUnitCell(filtered_peaks);

    if (filtered_peaks.empty()) {
        return false;
    }

    auto cell = filtered_peaks[0]->activeUnitCell();

    filtered_peaks = peak_filter.unitCell(filtered_peaks,cell);
    filtered_peaks = peak_filter.indexed(filtered_peaks,cell,cell->indexingTolerance());

    auto grp = nsx::SpaceGroup(cell->spaceGroup());

    nsx::MergedData merged_data(grp, friedel);

    nsx::ResolutionShell resolution_shells = {dmin, dmax, num_shells};
    for (auto peak : filtered_peaks) {
        resolution_shells.addPeak(peak);
    }

    //const auto& shells = resolution_shells.shells();
 
    file << "          dmax       dmin       nobs nmerge   redundancy     r_meas    r_merge      r_pim    CChalf    CC*" << std::endl;

    // note: we print the shells in reverse order
    for (int i = num_shells-1; i >= 0; --i) {
        const double d_lower = resolution_shells.shell(i).dmin;
        const double d_upper = resolution_shells.shell(i).dmax;

        nsx::MergedData merged_shell(grp, friedel);

        for (auto&& peak: resolution_shells.shell(i).peaks) {
            merged_shell.addPeak(peak);
            merged_data.addPeak(peak);
        }

        nsx::CC cc;
        cc.calculate(merged_shell);
        nsx::RFactor rfactor;
        rfactor.calculate(merged_shell);

        std::snprintf(&buf[0], buf.size(),
                " %10.2f %10.2f" // dmax, dmin
                " %10zd %10zd %10.3f" // nobs, nmerge, redundancy
                " %10.3f %10.3f" // Rmeas, expected Rmeas
                " %10.3f %10.3f" // Rmerge, expected Rmerge
                " %10.3f %10.3f" // Rpim, expected Rpim
                " %10.3f %10.3f", // CC half, CC*
                d_upper, d_lower, 
                merged_shell.totalSize(), merged_shell.getPeaks().size(), merged_shell.redundancy(),
                rfactor.Rmeas(), rfactor.expectedRmeas(),
                rfactor.Rmerge(), rfactor.expectedRmerge(),
                rfactor.Rpim(), rfactor.expectedRpim(),
                cc.CChalf(), cc.CCstar());

        file << &buf[0] << std::endl;

        nsx::debug() << "Finished logging shell [" << d_lower << "," << d_upper << "]";
    }

    file << "--------------------------------------------------------------------------------" << std::endl;
 
    nsx::CC cc;
    cc.calculate(merged_data);

    nsx::RFactor rfactor;    
    rfactor.calculate(merged_data);

    std::snprintf(&buf[0], buf.size(),
            "    %10.2f %10.2f %10d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",
            dmin, dmax, merged_data.totalSize(), merged_data.redundancy(),
            rfactor.Rmeas(), rfactor.Rmerge(), rfactor.Rpim(), cc.CChalf(), cc.CCstar());

    file << &buf[0] << std::endl << std::endl;

    file << "   h    k    l            I        sigma    nobs       chi2             p  "
         << std::endl;

    unsigned int total_peaks = 0;
    unsigned int bad_peaks = 0;

    // for debugging
    bool write_unmerged = true;

    for (auto&& peak : merged_data.getPeaks()) {

        const auto hkl = peak.getIndex();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        const double intensity = peak.getIntensity().value();
        const double sigma = peak.getIntensity().sigma();
        const int nobs = peak.redundancy();

        const double chi2 = peak.chi2();
        const double p = peak.pValue();

        std::snprintf(&buf[0], buf.size(), "  %4d %4d %4d %15.2f %10.2f %3d %15.5f %15.5f",
                      h, k, l, intensity, sigma, nobs, chi2, p);

        file << &buf[0];
        file << std::endl;
        
        ++total_peaks;

        if (!write_unmerged) {
            continue;
        }

        for (auto unmerged: peak.getPeaks()) {
            auto c = unmerged->getShape().center();
            auto numor = unmerged->data()->filename();
            auto I = unmerged->correctedIntensity();

            std::snprintf(&buf[0], buf.size(), 
                "    "
                " %10.3f %10.3f %10.3f " // x, y, frame
                " %10.3f %10.3f" // I, sigma
                " %s", // filename
                c[0], c[1], c[2], 
                I.value(), I.sigma(),
                numor.c_str());

            file << &buf[0];
            file << std::endl;
        }
    }

    file << std::endl;
    file << "total peaks: " << total_peaks << std::endl;
    file << "  bad peaks: " << bad_peaks << std::endl;

    nsx::debug() << "Done writing log file.";

    file.close();
    return true;
}
