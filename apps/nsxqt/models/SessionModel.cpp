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
#include <memory>
#include <stdexcept>
#include <map>
#include <vector>

#include <hdf5.h>
#include <H5Exception.h>

#include <QAbstractItemView>
#include <QDate>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>
#include <QVector>

#include <nsxlib/crystal/MergedPeak.h>
#include <nsxlib/crystal/PeakPredictor.h>
#include <nsxlib/crystal/ResolutionShell.h>
#include <nsxlib/statistics/RFactor.h>
#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/MergedData.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/data/XDS.h>
#include <nsxlib/geometry/Ellipsoid.h>
#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/logger/Logger.h>
#include <nsxlib/utils/ProgressHandler.h>
#include <nsxlib/statistics/CC.h>

#include "absorption/AbsorptionDialog.h"
#include "absorption/MCAbsorptionDialog.h"
#include "DetectorScene.h"
#include "dialogs/DialogCalculatedPeaks.h"
#include "dialogs/DialogConvolve.h"
#include "dialogs/DialogExperiment.h"
#include "dialogs/FriedelDialog.h"
#include "dialogs/LogFileDialog.h"
#include "dialogs/ResolutionCutoffDialog.h"
#include "dialogs/SpaceGroupDialog.h"
#include "externals/qcustomplot.h"
#include "models/DataItem.h"
#include "models/DetectorItem.h"
#include "models/ExperimentItem.h"
#include "models/SessionModel.h"
#include "models/TreeItem.h"
#include "models/InstrumentItem.h"
#include "models/NumorItem.h"
#include "models/PeakListItem.h"
#include "models/SampleItem.h"
#include "models/SourceItem.h"
#include "opengl/GLSphere.h"
#include "opengl/GLWidget.h"
#include "UnitCellItem.h"
#include "views/PeakTableView.h"
#include "views/ProgressView.h"

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
        auto uc = expt->getDiffractometer()->getSample()->getUnitCell(idx);
        uc->setName(p->text().toStdString());
    }
}

void SessionModel::createNewExperiment()
{
    DialogExperiment* dlg;

    // DialogExperiment could throw an exception if it fails to read the resource files
    try {
        dlg = new DialogExperiment();

        // The user pressed cancel, return
        if (!dlg->exec()) {
            return;
        }

        // If no experiment name is provided, pop up a warning
        if (dlg->getExperimentName().isEmpty()) {
            nsx::error() << "Empty experiment name";
            return;
        }
    }
    catch(std::exception& e) {
        nsx::debug() << "Runtime error: " << e.what();
        return;
    }

    // Add the experiment
    try {
        addExperiment(dlg->getExperimentName().toStdString(),dlg->getInstrumentName().toStdString());
    }
    catch(const std::runtime_error& e) {
        nsx::error() << e.what();
        return;
    }
}

nsx::sptrExperiment SessionModel::addExperiment(const std::string& experimentName, const std::string& instrumentName)
{
    // Create an experiment
    nsx::sptrExperiment expPtr(new nsx::Experiment(experimentName,instrumentName));

    // Create an experiment item
    ExperimentItem* expt = new ExperimentItem(expPtr);

    appendRow(expt);

    return expPtr;
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
                        numors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
                }
            }
        }
    }

    return numors;
}

QJsonObject SessionModel::toJsonObject()
{
    QJsonObject obj;
    QJsonArray experiments;

    for(int i = 0; i < rowCount(); ++i) {
        ExperimentItem* expItem = dynamic_cast<ExperimentItem*>(item(i,0));
        assert(expItem != nullptr);
        experiments.push_back(expItem->toJson());
    }

    obj["experiments"] = experiments;

    return obj;
}

void SessionModel::fromJsonObject(const QJsonObject &obj)
{

    QJsonArray experiments = obj["experiments"].toArray();

    for (auto expr: experiments) {
        QJsonObject exp_obj = expr.toObject();
        std::string name = exp_obj["name"].toString().toStdString();
        std::string instrument = exp_obj["instrument"].toObject()["name"].toString().toStdString();
        nsx::sptrExperiment ptr = addExperiment(name, instrument);

        ExperimentItem* expItem = dynamic_cast<ExperimentItem*>(item(rowCount()-1,0));
        assert(expItem != nullptr);
        expItem->fromJson(exp_obj);
    }
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
                    numors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
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
    for (auto idata : datav)
    {
       auto peaks=idata->getPeaks();
       for (auto peak: peaks)
       {
           GLSphere* sphere=new GLSphere("");
           Eigen::Vector3d pos=peak->getQ();
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
                selectedNumors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
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

    // lambda function to execute peak search in a separate thread
    auto task = [=] () -> bool
    {
        bool result = false;

        // execute in a try-block because the progress handler may throw if it is aborted by GUI
        try {
            result = _peakFinder->find(selectedNumors);
        }
        catch(std::exception& e) {
            nsx::debug() << "Caught exception during peak find: " << e.what();
            nsx::debug() <<" Peak search aborted.";
            return false;
        }
        return result;
    };

    auto onFinished = [=] (bool succeeded) -> void
    {
        // delete the progressView
        delete progressView;

        int num_peaks = 0;

        for (auto numor: selectedNumors) {
            num_peaks += numor->getPeaks().size();
        }

        if ( succeeded ) {
            //ui->_dview->getScene()->updatePeaks();
            updatePeaks();

            nsx::debug() << "Peak search complete., found "
                     << num_peaks
                     << " peaks.";
        }
        else {
            nsx::debug() << "Peak search failed!";
        }
    };

    auto job = new Job(this, task, onFinished, true);
    //connect(progressView, SIGNAL(canceled()), job, SLOT(terminate()));

    job->exec();
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


    int last_done = 0;
//    int predicted_peaks = 0;
    int observed_peaks = 0;

    for(auto numor: numors) {
        nsx::debug() << "Finding missing peaks for numor " << ++current_numor << " of " << numors.size();

        auto predictor = nsx::PeakPredictor();
        predictor._dmin = dialog.dMin();
        predictor._dmax = dialog.dMax();
        predictor._searchRadius = dialog.searchRadius();
        predictor._peakScale = dialog.peakScale();
        predictor._bkgScale = dialog.bkgScale();
        predictor._frameRadius = dialog.frameRadius();
        predictor._minimumRadius = dialog.minimumRadius();
        predictor._minimumPeakDuration = dialog.minimumPeakDuration();
        predictor._Isigma = dialog.Isigma();
        predictor._handler = handler;
        nsx::PeakSet predicted = predictor.predictPeaks(numor, false);
        numor->integratePeaks(predicted, predictor._peakScale, predictor._bkgScale, false, handler);
        observed_peaks += numor->getPeaks().size();

        for (auto peak: predicted) {
            numor->addPeak(peak);
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

        for (auto peak: numor->getPeaks()) {
            if (!peak->isSelected() || peak->isMasked())
                continue;

            double d = 1.0 / peak->getQ().norm();
            avg_d += d;
            ++num_peaks;

            if ( dmin > d && d > dmax)
                bad_peaks.push_back(peak);
        }

        // erase the bad peaks from the list
        for (auto peak: bad_peaks) {
            numor->removePeak(peak);
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
    nsx::PeakList peaks;

    auto numors = this->getSelectedNumors();

    for (auto numor: numors) {
        for (auto peak: numor->getPeaks()) {
            if (peak->isSelected() && !peak->isMasked()) {
                peaks.push_back(peak);
            }
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

    auto basis = peaks[0]->getActiveUnitCell();

    if (!basis) {
        nsx::error() << "No unit cell defined the peaks. No index can be defined.";
        return false;
    }

    for (auto peak: peaks) {
        if (peak->isMasked() || !peak->isSelected())
            continue;

        Eigen::RowVector3d hkl;
        auto currentBasis = peak->getActiveUnitCell();

        if (currentBasis != basis) {
            nsx::error() << "Not all the peaks have the same unit cell. Multi crystal not implement yet";
            return false;
        }

        if (!(peak->getMillerIndices(*currentBasis, hkl, true)))
            continue;

        const long h = std::lround(hkl[0]);
        const long k = std::lround(hkl[1]);
        const long l = std::lround(hkl[2]);

        double lorentz = peak->getLorentzFactor();
        double trans = peak->getTransmission();

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
    //std::vector<nsx::MergedPeak> merged_peaks;
    //std::vector<nsx::MergedPeak> merged_peaks_shell;

    Eigen::RowVector3d HKL(0.0, 0.0, 0.0);

    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return false;
    }

    if (peaks.size() == 0) {
        nsx::error() << "No peaks to write to log!";
        return false;
    }

    auto cell = peaks[0]->getActiveUnitCell();
    auto grp = nsx::SpaceGroup(cell->getSpaceGroup());

    nsx::MergedData merged(grp, friedel);

    for (auto&& peak: peaks) {
        if (cell != peak->getActiveUnitCell()) {
            nsx::error() << "Only one unit cell is supported at this time!!";
            return false;
        }
        // skip bad/masked peaks
        if (peak->isMasked() || !peak->isSelected()) {
            continue;
        }
        // skip misindexed peaks
        if (!peak->getMillerIndices(*cell, HKL, true)) {
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

            //if (new_peak.redundancy() > 0) {
            //    merged_peaks.push_back(new_peak);
            //    merged_peaks_shell.push_back(new_peak);
            //}
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

    auto compare_fn = [](const nsx::MergedPeak& p, const nsx::MergedPeak& q) -> bool
    {
        const auto a = p.getIndex();
        const auto b = q.getIndex();

        if (a(0) != b(0))
            return a(0) < b(0);
        else if (a(1) != b(1))
            return a(1) < b(1);
        else
            return a(2) < b(2);
    };

    //std::sort(merged_peaks.begin(), merged_peaks.end(), compare_fn);

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
        const double d = 0.0; //peak.d();
        const int nobs = peak.redundancy();
        //const double std = peak.std();
        //const double rel_std = std / intensity;

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
}


void SessionModel::autoAssignUnitCell()
{
    auto numors = getSelectedNumors();

    for (auto&& numor: numors) {
        auto sample = numor->getDiffractometer()->getSample();
        auto peaks = numor->getPeaks();

        for (auto&& peak: peaks) {
            Eigen::RowVector3d hkl;
            bool assigned = false;

            for (auto i = 0; i < sample->getNCrystals(); ++i) {
                auto cell = sample->getUnitCell(i);

                if (peak->getMillerIndices(*cell, hkl, true)) {
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
