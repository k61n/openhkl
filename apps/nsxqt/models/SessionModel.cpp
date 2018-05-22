/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include <hdf5.h>
#include <H5Exception.h>

#include <QAbstractItemView>
#include <QDate>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>

#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>
#include <QVector>

#include <nsxlib/CC.h>
#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Ellipsoid.h>
#include <nsxlib/FitProfile.h>
#include <nsxlib/GeometryTypes.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MergedData.h>
#include <nsxlib/MergedPeak.h>
#include <nsxlib/MillerIndex.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/ResolutionShell.h>
#include <nsxlib/RFactor.h>
#include <nsxlib/Sample.h>
#include <nsxlib/ShapeLibrary.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/StrongPeakIntegrator.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>

#include "AbsorptionDialog.h"
#include "DataItem.h"
#include "DetectorItem.h"
#include "DetectorScene.h"
#include "DialogCalculatedPeaks.h"
#include "DialogExperiment.h"
#include "DialogPeakFind.h"
#include "ExperimentItem.h"
#include "FriedelDialog.h"
#include "GLSphere.h"
#include "GLWidget.h"
#include "InstrumentItem.h"
#include "LogFileDialog.h"
#include "MCAbsorptionDialog.h"
#include "NumorItem.h"
#include "PeakListItem.h"
#include "PeakTableView.h"
#include "ProgressView.h"
#include "QCustomPlot.h"
#include "PeaksItem.h"
#include "ResolutionCutoffDialog.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "TreeItem.h"
#include "UnitCellItem.h"

#include "ui_MainWindow.h"
#include "ui_ScaleDialog.h"

SessionModel::SessionModel()
{
    connect(this,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(onItemChanged(QStandardItem*)));
}

SessionModel::~SessionModel()
{
    // _model should be deleted automatically during destructor by QT
    //delete _model;
}

void SessionModel::onItemChanged(QStandardItem* item)
{
    if (auto p = dynamic_cast<UnitCellItem*>(item)) {
        // The first item of the Sample item branch is the SampleShapeItem, skip it
        int idx = p->index().row()- 1;
        auto expt = p->experiment();
        auto uc = expt->getDiffractometer()->getSample()->unitCell(idx);
        uc->setName(p->text().toStdString());
    }
}

nsx::DataList SessionModel::getSelectedNumors(ExperimentItem* item) const
{
    nsx::DataList numors;

    QList<QStandardItem*> dataItems = findItems(QString("Data"),Qt::MatchCaseSensitive|Qt::MatchRecursive);

    for (const auto& it : dataItems) {
        for (auto i=0;i < rowCount(it->index());++i) {
            if (it->child(i)->checkState() == Qt::Checked) {
                if (auto ptr = dynamic_cast<NumorItem*>(it->child(i))) {
                    if (it->parent() == item)
                        numors.push_back(ptr->getData());
                }
            }
        }
    }

    return numors;
}

void SessionModel::setFilename(QString name)
{
    _filename = name;
}

QString SessionModel::getFilename()
{
    return _filename;
}

void SessionModel::setColorMap(const std::string &name)
{
    _colormap = name;
}

std::string SessionModel::getColorMap() const
{
    return _colormap;
}

nsx::DataList SessionModel::getSelectedNumors() const
{
    nsx::DataList numors;

    QList<QStandardItem*> dataItems = findItems(QString("Data"),Qt::MatchCaseSensitive|Qt::MatchRecursive);

    for (const auto& it : dataItems) {
        for (auto i=0;i < rowCount(it->index());++i) {
            if (it->child(i)->checkState() == Qt::Checked) {
                if (auto ptr = dynamic_cast<NumorItem*>(it->child(i)))
                    numors.push_back(ptr->getData());
            }
        }
    }

    return numors;
}


void SessionModel::absorptionCorrection()
{
    // NOT IMPLEMENTED
}

void SessionModel::showPeaksOpenGL()
{
    GLWidget* glw=new GLWidget();
    auto& scene=glw->getScene();
    auto datav=getSelectedNumors();
    for (auto idata : datav) {
       auto numor_peaks = peaks(idata.get());
       for (auto peak: numor_peaks) {
           GLSphere* sphere=new GLSphere("");
           Eigen::RowVector3d pos = peak->q().rowVector();
           sphere->setPos(pos[0]*100,pos[1]*100,pos[2]*100);
           sphere->setColor(0,1,0);
           scene.addActor(sphere);
       }
    }
    glw->show();
}

void SessionModel::computeRFactors()
{
    // todo: reimplement this method
    nsx::debug() << "not currently implemented!";
}

void SessionModel::findFriedelPairs()
{
    nsx::debug() << "findFriedelParis() is not yet implemented!";
    return;
}

void SessionModel::peakFitDialog()
{
    nsx::debug() << "peakFitDialog() triggered";
    nsx::error() << "this feature has been deprecated";
}


void SessionModel::writeLog()
{
    // todo: fix this
    #if 0
    LogFileDialog dialog;
    auto numors = this->getSelectedNumors();

    nsx::PeakFilter peak_filter;
    nsx::PeakList selected_peaks;
    selected_peaks = peak_filter.selected(_peaks,true);

    if (!selected_peaks.size()) {
        nsx::error() << "No peaks in the table";
        return;
    }

    // return if user cancels dialog
    if (!dialog.exec())
        return;

    bool friedel = dialog.friedel();

    if (dialog.writeUnmerged()) {
        if (!writeXDS(dialog.unmergedFilename(), selected_peaks, false, friedel))
            nsx::error() << "Could not write unmerged data to " << dialog.unmergedFilename().c_str();
    }

    if (dialog.writeMerged()) {
        if (!writeXDS(dialog.mergedFilename(), selected_peaks, true, friedel))
            nsx::error() << "Could not write unmerged data to " << dialog.mergedFilename().c_str();
    }

    if (dialog.writeStatistics()) {
        if (!writeStatistics(dialog.statisticsFilename(),
                selected_peaks,
                             dialog.dmin(), dialog.dmax(), dialog.numShells(), friedel))
            nsx::error() << "Could not write statistics log to " << dialog.statisticsFilename().c_str();
    }
    #endif
}

bool SessionModel::writeXDS(std::string filename, const nsx::PeakList& peaks, bool merge, bool friedel)
{
    nsx::error() << "writeXDS method not implemented";
    return false;
}

bool SessionModel::writeNewShellX(std::string filename, const nsx::PeakList& peaks)
{
    std::fstream file(filename, std::ios::out);
    std::vector<char> buf(1024, 0); // buffer for snprintf

    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
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

    for (auto peak : filtered_peaks) {

        nsx::MillerIndex hkl(peak->q(), *cell);

        auto center = peak->getShape().center();
        auto pos = peak->data()->diffractometer()->getDetector()->pixelPosition(center[0], center[1]);

        auto corrected_intensity = peak->correctedIntensity();
        double intensity = corrected_intensity.value();
        double sigma = corrected_intensity.sigma();

        std::snprintf(&buf[0], buf.size(), "  %4d %4d %4d %15.2f %10.2f", hkl[0], hkl[1], hkl[2], intensity, sigma);
        file << &buf[0] << std::endl;
    }

    if (file.is_open())
        file.close();

    return true;
}

bool SessionModel::writeStatistics(std::string filename,
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

    const auto& shells = resolution_shells.shells();
 
    file << "          dmax       dmin       nobs nmerge   redundancy     r_meas    r_merge      r_pim    CChalf    CC*" << std::endl;

    // note: we print the shells in reverse order
    for (int i = num_shells-1; i >= 0; --i) {
        const double d_lower = shells[i].dmin;
        const double d_upper = shells[i].dmax;

        nsx::MergedData merged_shell(grp, friedel);

        for (auto&& peak: shells[i].peaks) {
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

void SessionModel::autoAssignUnitCell()
{
    auto numors = getSelectedNumors();

    for (auto&& numor: numors) {
        auto sample = numor->diffractometer()->getSample();
        nsx::PeakList numor_peaks = peaks(numor.get());

        nsx::PeakFilter peak_filter;
        nsx::PeakList filtered_peaks;
        filtered_peaks = peak_filter.selected(numor_peaks,true);

        for (auto peak : filtered_peaks) {
            Eigen::RowVector3d hkl;
            bool assigned = false;

            for (size_t i = 0; i < sample->getNCrystals(); ++i) {
                auto cell = sample->unitCell(i);
                nsx::MillerIndex hkl(peak->q(), *cell);
                if (hkl.indexed(cell->indexingTolerance())) {
                    peak->addUnitCell(cell, true);
                    assigned = true;
                    break;
                }
            }

            // could not assign unit cell
            if (assigned == false) {
                peak->setSelected(false);
            }
        }
    }
    nsx::debug() << "Done auto assigning unit cells";
}

#if 1
nsx::PeakList SessionModel::peaks(const nsx::DataSet* data) const
{  
    nsx::PeakList list;

    for (auto i = 0; i < rowCount(); ++i) {
        auto& exp_item = dynamic_cast<ExperimentItem&>(*item(i));
        auto&& peaks = exp_item.peaks()->selectedPeaks();

        for (auto peak: peaks) {
            if (data == nullptr || peak->data().get() == data) {
                list.push_back(peak);
            }
        }
    }
    return list;
}
#endif

void SessionModel::createNewExperiment()
{
    std::unique_ptr<DialogExperiment> dlg;

    // DialogExperiment could throw an exception if it fails to read the resource files
    try {
        dlg = std::unique_ptr<DialogExperiment>(new DialogExperiment());

        // The user pressed cancel, return
        if (!dlg->exec())
            return;

        // If no experiment name is provided, pop up a warning
        if (dlg->getExperimentName().isEmpty()) {
            nsx::error() << "Empty experiment name";
            return;
        }
    }
    catch(std::exception& e) {
        nsx::error() << e.what();
        return;
    }

    // Add the experiment
    try {
        // Create an experiment
        auto experimentName = dlg->getExperimentName().toStdString();
        auto instrumentName = dlg->getInstrumentName().toStdString();
        nsx::sptrExperiment expPtr(new nsx::Experiment(experimentName,instrumentName));
        // Create an experiment item
        ExperimentItem* expt = new ExperimentItem(expPtr);    
        appendRow(expt);
    }
    catch(const std::runtime_error& e) {
        nsx::error() << e.what();
        return;
    }
}