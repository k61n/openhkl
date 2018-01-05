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
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Ellipsoid.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MergedData.h>
#include <nsxlib/MergedPeak.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/ResolutionShell.h>
#include <nsxlib/RFactor.h>
#include <nsxlib/Sample.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/XDS.h>

#include "AbsorptionDialog.h"
#include "DataItem.h"
#include "DetectorItem.h"
#include "DetectorScene.h"
#include "DialogCalculatedPeaks.h"
#include "DialogConvolve.h"
#include "DialogExperiment.h"
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
#include "ResolutionCutoffDialog.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "SpaceGroupDialog.h"
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
    if (auto p=dynamic_cast<UnitCellItem*>(item)) {
        // The first item of the Sample item branch is the SampleShapeItem, skip it
        int idx = p->index().row()- 1;
        auto expt = p->getExperiment();
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

void SessionModel::importData()
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
           Eigen::RowVector3d pos = peak->getQ().rowVector();
           sphere->setPos(pos[0]*100,pos[1]*100,pos[2]*100);
           sphere->setColor(0,1,0);
           scene.addActor(sphere);
       }
    }
    glw->show();
}

void SessionModel::findSpaceGroup()
{
    // NOT IMPLEMENTED
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

void SessionModel::findPeaks(const QModelIndex& index)
{
    MainWindow* main = dynamic_cast<MainWindow*>(QApplication::activeWindow());
    auto ui = main->getUI();
    QStandardItem* item = itemFromIndex(index);
    TreeItem* titem = dynamic_cast<TreeItem*>(item);

    if (!titem)
        return;

    auto expt = titem->getExperiment();

    if (!expt)
        return;

    QStandardItem* ditem = itemFromIndex(index);
    nsx::DataList selectedNumors;
    int nTotalNumors = rowCount(ditem->index());
    selectedNumors.reserve(size_t(nTotalNumors));

    for (int i = 0; i < nTotalNumors; ++i) {
        if (ditem->child(i)->checkState() == Qt::Checked) {
            if (auto ptr = dynamic_cast<NumorItem*>(ditem->child(i)))
                selectedNumors.push_back(ptr->getData());
        }
    }

    if (selectedNumors.empty()) {
        nsx::error()<<"No numors selected for finding peaks";
        return;
    }

    // run peak find
    auto frame = ui->_dview->getScene()->getCurrentFrame();

    // check if now frame was loaded, or is otherwise invalid
    if ( frame.rows() == 0 || frame.cols() == 0) {
        // attempt to read first frame of first numor by default
        try {
            selectedNumors[0]->open();
            frame = selectedNumors[0]->getFrame(0);
        }
        catch(std::exception& e) {
            nsx::debug() << "Peak search failed: cannot load frame: " << e.what();
            return;
        }
    }

    // reset progress handler
    _progressHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);

    // set up peak finder
    if ( !_peakFinder)
        _peakFinder = nsx::sptrPeakFinder(new nsx::PeakFinder);
    _peakFinder->setHandler(_progressHandler);

    DialogConvolve* dialog = new DialogConvolve(frame, _peakFinder, nullptr);
    dialog->setColorMap(_colormap);

    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogConvolve> dialog_ptr(dialog);

    if (!dialog->exec())
        return;

    ui->_dview->getScene()->clearPeaks();

    size_t max = selectedNumors.size();
    nsx::info() << "Peak find algorithm: Searching peaks in " << max << " files";

    // create a pop-up window that will show the progress
    ProgressView* progressView = new ProgressView(nullptr);
    progressView->watch(_progressHandler);

    // execute in a try-block because the progress handler may throw if it is aborted by GUI
    try {
        _peaks = _peakFinder->find(selectedNumors);
    }
    catch(std::exception& e) {
        nsx::debug() << "Caught exception during peak find: " << e.what();
        return;
    }

    // delete the progressView
    delete progressView;

    updatePeaks();
    nsx::debug() << "Peak search complete., found " << _peaks.size() << " peaks.";
}


void SessionModel::incorporateCalculatedPeaks()
{
    nsx::debug() << "Incorporating missing peaks into current data set...";

    DialogCalculatedPeaks dialog;

    if (!dialog.exec()) {
        return;
    }

    nsx::DataList numors = getSelectedNumors();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    int current_numor = 0;

    int observed_peaks = 0;

    for(auto numor: numors) {
        nsx::debug() << "Finding missing peaks for numor " << ++current_numor << " of " << numors.size();

        auto reference_peaks = peaks(numor.get());

        auto predictor = nsx::PeakPredictor(numor);
        predictor._dmin = dialog.dMin();
        predictor._dmax = dialog.dMax();       
        predictor._peakScale = dialog.peakScale();
        predictor._bkgScale = dialog.bkgScale();     
        predictor._Isigma = dialog.Isigma();
        predictor._handler = handler;
        // debugging
        nsx::PeakSet predicted = predictor.predictPeaks(true, reference_peaks);
        //nsx::PeakSet predicted = predictor.predictPeaks(false, reference_peaks);
        numor->integratePeaks(predicted, predictor._peakScale, predictor._bkgScale, false, handler);
        observed_peaks += peaks(numor.get()).size();

        for (auto peak: predicted) {
            addPeak(peak);
        }
    }
    updatePeaks();
    nsx::debug() << "Done incorporating missing peaks.";
}

void SessionModel::applyResolutionCutoff(double dmin, double dmax)
{
    int num_removed = 0;
    double avg_d = 0;
    int num_peaks = 0;

    nsx::debug() << "Applying resolution cutoff...";

    nsx::DataList numors = getSelectedNumors();

    for(auto numor: numors) {
        nsx::PeakList bad_peaks;
        auto sample = numor->getDiffractometer()->getSample();

        for (auto peak: peaks(numor.get())) {

            if (!peak->isSelected()) {
                continue;
            }

            double d = 1.0 / peak->getQ().rowVector().norm();
            avg_d += d;
            ++num_peaks;

            if ( dmin > d && d > dmax)
                bad_peaks.push_back(peak);
        }

        // erase the bad peaks from the list
        for (auto peak: bad_peaks) {
            removePeak(peak);
            ++num_removed;
        }
    }

    avg_d /= num_peaks;

    nsx::debug() << "Done applying resolution cutoff. Removed " << num_removed << " peaks.";
    nsx::debug() << "Average value of d for peaks is " << avg_d;
}


void SessionModel::writeLog()
{
    LogFileDialog dialog;
    auto numors = this->getSelectedNumors();
    nsx::PeakList peaks;

    for (auto peak: _peaks) {
        if (peak->isSelected()) {
            peaks.push_back(peak);
        }
    }

    if (!peaks.size()) {
        nsx::error() << "No peaks in the table";
        return;
    }

    // return if user cancels dialog
    if (!dialog.exec())
        return;

    bool friedel = dialog.friedel();

    if (dialog.writeUnmerged()) {
        if (!writeXDS(dialog.unmergedFilename(), peaks, false, friedel))
            nsx::error() << "Could not write unmerged data to " << dialog.unmergedFilename().c_str();
    }

    if (dialog.writeMerged()) {
        if (!writeXDS(dialog.mergedFilename(), peaks, true, friedel))
            nsx::error() << "Could not write unmerged data to " << dialog.mergedFilename().c_str();
    }

    if (dialog.writeStatistics()) {
        if (!writeStatistics(dialog.statisticsFilename(),
                             peaks,
                             dialog.dmin(), dialog.dmax(), dialog.numShells(), friedel))
            nsx::error() << "Could not write statistics log to " << dialog.statisticsFilename().c_str();
    }
}

bool SessionModel::writeNewShellX(std::string filename, const nsx::PeakList& peaks)
{
    std::fstream file(filename, std::ios::out);
    std::vector<char> buf(1024, 0); // buffer for snprintf

    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return false;
    }

    auto basis = peaks[0]->activeUnitCell();

    if (!basis) {
        nsx::error() << "No unit cell defined the peaks. No index can be defined.";
        return false;
    }

    for (auto peak: peaks) {
        if (!peak->isSelected()) {
            continue;
        }

        Eigen::RowVector3d hkl;
        auto currentBasis = peak->activeUnitCell();

        if (currentBasis != basis) {
            nsx::error() << "Not all the peaks have the same unit cell. Multi crystal not implement yet";
            return false;
        }

        if (!(currentBasis->getMillerIndices(peak->getQ(), hkl, true)))
            continue;

        const long h = std::lround(hkl[0]);
        const long k = std::lround(hkl[1]);
        const long l = std::lround(hkl[2]);

        auto center = peak->getShape().center();
        auto pos = peak->data()->getDiffractometer()->getDetector()->getPos(center[0], center[1]);

        double intensity = peak->getCorrectedIntensity().value();
        double sigma = peak->getCorrectedIntensity().sigma();

        std::snprintf(&buf[0], buf.size(), "  %4ld %4ld %4ld %15.2f %10.2f", h, k, l, intensity, sigma);
        file << &buf[0] << std::endl;
    }

    if (file.is_open())
        file.close();

    return true;
}

bool SessionModel::writeStatistics(std::string filename,
                                    const nsx::PeakList &peaks,
                                    double dmin, double dmax, unsigned int num_shells, bool friedel)
{
    std::fstream file(filename, std::ios::out);
    nsx::ResolutionShell res = {dmin, dmax, num_shells};
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

    auto cell = peaks[0]->activeUnitCell();
    auto grp = nsx::SpaceGroup(cell->spaceGroup());

    nsx::MergedData merged(grp, friedel);

    for (auto&& peak: peaks) {
        // skip bad/masked peaks
        if (!peak->isSelected()) {
            continue;
        }

        if (cell != peak->activeUnitCell()) {
            nsx::error() << "Only one unit cell is supported at this time!!";
            return false;
        }
        // skip misindexed peaks
        if (!cell->getMillerIndices(peak->getQ(), HKL, true)) {
            continue;
        }
        res.addPeak(peak);
    }

    auto&& ds = res.getD();
    auto&& shells = res.getShells();

    std::vector<nsx::PeakList> all_equivs;

    file << "          dmax       dmin       nobs redundancy     r_meas    r_merge      r_pim    CChalf    CC*" << std::endl;

    // note: we print the shells in reverse order
    for (int i = num_shells-1; i >= 0; --i) {
        const double d_lower = ds[i];
        const double d_upper = ds[i+1];

        nsx::MergedData merged_shell(grp, friedel);

        auto peak_equivs = grp.findEquivalences(shells[i], friedel);
        
        for (auto&& equiv: peak_equivs)
            all_equivs.push_back(equiv);

        double redundancy = double(shells[i].size()) / double(peak_equivs.size());

        for (auto equiv: peak_equivs) {

            for (auto peak: equiv) {
                merged.addPeak(peak);
                merged_shell.addPeak(peak);
            }
        }

        nsx::CC cc;
        cc.calculate(merged_shell);
        nsx::RFactor rfactor;
        rfactor.calculate(merged_shell);

        std::snprintf(&buf[0], buf.size(),
                "    %10.2f %10.2f %10d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",
                d_upper, d_lower, int(shells[i].size()), redundancy,
                rfactor.Rmeas(), rfactor.Rmerge(), rfactor.Rpim(), cc.CChalf(), cc.CCstar());

        file << &buf[0] << std::endl;

        nsx::debug() << "Finished logging shell " << i+1;
    }

    file << "--------------------------------------------------------------------------------" << std::endl;

    

    int num_peaks = 0;

    for (auto& equiv: all_equivs)
        num_peaks += equiv.size();

    double redundancy = double(num_peaks) / double(all_equivs.size());

    nsx::CC cc;
    cc.calculate(merged);

    nsx::RFactor rfactor;    
    rfactor.calculate(merged);

    std::snprintf(&buf[0], buf.size(),
            "    %10.2f %10.2f %10d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",
            dmin, dmax, num_peaks, redundancy,
            rfactor.Rmeas(), rfactor.Rmerge(), rfactor.Rpim(), cc.CChalf(), cc.CCstar());

    file << &buf[0] << std::endl << std::endl;

    file << "   h    k    l            I        sigma    nobs       chi2             p  "
         << std::endl;

    unsigned int total_peaks = 0;
    unsigned int bad_peaks = 0;

    for (auto&& peak: merged.getPeaks()) {

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
    }


    file << std::endl;
    file << "total peaks: " << total_peaks << std::endl;
    file << "  bad peaks: " << bad_peaks << std::endl;

    nsx::debug() << "Done writing log file.";

    file.close();
    return true;
}


bool SessionModel::writeXDS(std::string filename, const nsx::PeakList& peaks, bool merge, bool friedel)
{
    #pragma warning "todo: fix this method"
    #if 0
    const std::string date = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    nsx::XDS xds(peaks, merge, friedel, filename, date);
    std::fstream file(filename, std::ios::out);

    if (!file.is_open()) {
        nsx::error() << "Could not open " << filename << " for writing.";
        return false;
    }
    bool result = xds.write(file);
    nsx::debug() << "Done writing log file.";
    return result;
    #endif
    return false;
}


void SessionModel::autoAssignUnitCell()
{
    auto numors = getSelectedNumors();

    for (auto&& numor: numors) {
        auto sample = numor->getDiffractometer()->getSample();
        nsx::PeakSet numor_peaks = peaks(numor.get());

        for (auto&& peak: numor_peaks) {
            Eigen::RowVector3d hkl;
            bool assigned = false;

            for (size_t i = 0; i < sample->getNCrystals(); ++i) {
                auto cell = sample->unitCell(i);

                if (cell->getMillerIndices(peak->getQ(), hkl, true)) {
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

nsx::PeakSet SessionModel::peaks(const nsx::DataSet* data) const
{  
    if (data == nullptr) {
        return _peaks;
    }

    nsx::PeakSet data_peaks;

    for (auto peak: _peaks) {
        if (peak->data().get() == data) {
            data_peaks.insert(peak);
        }
    }
    return data_peaks;
}

void SessionModel::addPeak(nsx::sptrPeak3D peak)
{
    _peaks.insert(peak);
}

void SessionModel::removePeak(nsx::sptrPeak3D peak)
{
    auto it = _peaks.find(peak);
    if (it != _peaks.end()) {
        _peaks.erase(it);
    }
}
