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

#include "ui_MainWindow.h"
#include "dialogs/DialogConvolve.h"
#include <nsxlib/utils/ProgressHandler.h>
#include "views/ProgressView.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <map>
#include <array>
#include <tuple>
#include <vector>
#include <fstream>

#include <QAbstractItemView>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>
#include <QtDebug>
#include <QDate>

//#include "BlobFinder.h"
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/instrument/Detector.h>
#include "dialogs/DialogExperiment.h"
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/utils/gcd.h>

#include "dialogs/DialogCalculatedPeaks.h"
#include "dialogs/ResolutionCutoffDialog.h"

#include "views/PeakTableView.h"
#include "absorption/AbsorptionDialog.h"
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
#include "absorption/MCAbsorptionDialog.h"
#include "OpenGL/GLWidget.h"
#include "OpenGL/GLSphere.h"
#include "Logger.h"
#include "views/ReciprocalSpaceViewer.h"
#include "DetectorScene.h"

#include <nsxlib/crystal/SpaceGroupSymbols.h>
#include <nsxlib/crystal/SpaceGroup.h>

#include "dialogs/SpaceGroupDialog.h"
#include "dialogs/LogFileDialog.h"

#include "UnitCellItem.h"

#include <QVector>
#include "Externals/qcustomplot.h"
#include "ui_ScaleDialog.h"

#include "dialogs/ScaleDialog.h"
#include "dialogs/FriedelDialog.h"

#include <nsxlib/crystal/RFactor.h>
#include <hdf5.h>
#include <H5Exception.h>

#include "dialogs/PeakFitDialog.h"
#include "dialogs/DialogConvolve.h"
#include <nsxlib/crystal/ResolutionShell.h>
#include <nsxlib/crystal/MergedPeak.h>
#include <nsxlib/data/XDS.h>

#include <nsxlib/geometry/NDTree.h>
#include <nsxlib/geometry/Ellipsoid.h>
#include <nsxlib/crystal/PeakPredictor.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

using std::vector;
using SX::Data::DataSet;
using std::shared_ptr;
using SX::Utils::ProgressHandler;
using SX::Crystal::UnitCell;
using SX::Crystal::SpaceGroup;
using SX::Crystal::RFactor;
using SX::Data::PeakFinder;
using SX::Crystal::PeakCalc;
using SX::Crystal::Peak3D;

using Octree = SX::Geometry::NDTree<double, 3>;
using Ellipsoid3D = SX::Geometry::Ellipsoid<double, 3>;

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
        auto sptrUnitCell = expt->getDiffractometer()->getSample()->getUnitCell(idx);
        sptrUnitCell->setName(p->text().toStdString());
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
            qWarning() << "Empty experiment name";
            return;
        }
    }
    catch(std::exception& e) {
        qDebug() << "Runtime error: " << e.what();
        return;
    }

    // Add the experiment
    try {
        addExperiment(dlg->getExperimentName().toStdString(),dlg->getInstrumentName().toStdString());
    }
    catch(const std::runtime_error& e) {
        qWarning() << e.what();
        return;
    }
}

std::shared_ptr<SX::Instrument::Experiment> SessionModel::addExperiment(const std::string& experimentName, const std::string& instrumentName)
{
    // Create an experiment
    std::shared_ptr<SX::Instrument::Experiment> expPtr(new SX::Instrument::Experiment(experimentName,instrumentName));

    // Create an experiment item
    ExperimentItem* expt = new ExperimentItem(expPtr);

    appendRow(expt);

    return expPtr;
}

vector<shared_ptr<DataSet>> SessionModel::getSelectedNumors(ExperimentItem* item) const
{
    vector<shared_ptr<DataSet>> numors;

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
        std::shared_ptr<Experiment> ptr = addExperiment(name, instrument);

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

vector<shared_ptr<DataSet>> SessionModel::getSelectedNumors() const
{
    vector<shared_ptr<DataSet>> numors;

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
    qDebug() << "Finding peak equivalences...";

    std::vector<std::shared_ptr<DataSet>> numors = getSelectedNumors();
    std::vector<std::vector<sptrPeak3D>> peak_equivs;
    std::vector<sptrPeak3D> peak_list;

    std::shared_ptr<UnitCell> unit_cell;

    for (std::shared_ptr<DataSet> numor: numors)
    {
        std::set<sptrPeak3D> peaks = numor->getPeaks();
        for (sptrPeak3D peak: peaks)
        {
            if ( peak && peak->isSelected() && !peak->isMasked() ) {
                peak_list.push_back(peak);
            }
        }
    }

    if ( peak_list.size() == 0) {
        qDebug() << "No peaks -- cannot search for equivalences!";
        return;
    }

    for (sptrPeak3D peak: peak_list)
    {
        // what do we do if there is more than one sample/unit cell??
        unit_cell = peak->getActiveUnitCell();

        if (unit_cell) {
            break;
        }
    }

    if (!unit_cell) {
        qDebug() << "No unit cell selected! Cannot compute R factors.";
        return;
    }

    SpaceGroup grp("P 1");

    // spacegroup construct can throw
    try {
        grp = SpaceGroup(unit_cell->getSpaceGroup());
    }
    catch(std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << endl;
        return;
    }

    peak_equivs = grp.findEquivalences(peak_list, true);

    qDebug() << "Found " << peak_equivs.size() << " equivalence classes of peaks:";

    std::map<size_t, int> size_counts;

    for (auto& peaks: peak_equivs) {
        ++size_counts[peaks.size()];
    }

    for (auto& it: size_counts) {
        qDebug() << "Found " << it.second << " classes of size " << it.first;
    }

    qDebug() << "Computing R factors:";

    RFactor rfactor(peak_equivs);

    qDebug() << "    Rmerge = " << rfactor.Rmerge();
    qDebug() << "    Rmeas  = " << rfactor.Rmeas();
    qDebug() << "    Rpim   = " << rfactor.Rpim();

    //ScaleDialog* scaleDialog = new ScaleDialog(peak_equivs, this);
    //scaleDialog->exec();
}

void SessionModel::findFriedelPairs()
{
    qDebug() << "findFriedelParis() is not yet implemented!";
    return;
}

void SessionModel::peakFitDialog()
{
    qDebug() << "peakFitDialog() triggered";
    PeakFitDialog* dialog = new PeakFitDialog(this, nullptr);
    dialog->exec();
}

void SessionModel::findPeaks(const QModelIndex& index)
{
    MainWindow* main = dynamic_cast<MainWindow*>(QApplication::activeWindow());
    auto ui = main->getUI();
    QStandardItem* item = itemFromIndex(index);
    TreeItem* titem = dynamic_cast<TreeItem*>(item);

    if (!titem)
        return;

    std::shared_ptr<SX::Instrument::Experiment> expt(titem->getExperiment());

    if (!expt)
        return;

    QStandardItem* ditem = itemFromIndex(index);
    std::vector<std::shared_ptr<SX::Data::DataSet>> selectedNumors;
    int nTotalNumors = rowCount(ditem->index());
    selectedNumors.reserve(size_t(nTotalNumors));

    for (int i = 0; i < nTotalNumors; ++i) {
        if (ditem->child(i)->checkState() == Qt::Checked) {
            if (auto ptr = dynamic_cast<NumorItem*>(ditem->child(i)))
                selectedNumors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
        }
    }

    if (selectedNumors.empty()) {
        qWarning()<<"No numors selected for finding peaks";
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
            qDebug() << "Peak search failed: cannot load frame: " << e.what();
            return;
        }
    }

    // qDebug() << "Preview frame has dimensions" << frame.rows() << " " << frame.cols();

    // reset progress handler
    _progressHandler = std::shared_ptr<ProgressHandler>(new ProgressHandler);

    // set up peak finder
    if ( !_peakFinder)
        _peakFinder = std::shared_ptr<PeakFinder>(new PeakFinder);
    _peakFinder->setHandler(_progressHandler);

    DialogConvolve* dialog = new DialogConvolve(frame, _peakFinder, nullptr);
    dialog->setColorMap(_colormap);

    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogConvolve> dialog_ptr(dialog);

    if (!dialog->exec())
        return;

    ui->_dview->getScene()->clearPeaks();

    size_t max = selectedNumors.size();
    qWarning() << "Peak find algorithm: Searching peaks in " << max << " files";

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
            qDebug() << "Caught exception during peak find: " << e.what();
            qDebug() <<" Peak search aborted.";
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

            qDebug() << "Peak search complete., found "
                     << num_peaks
                     << " peaks.";
        }
        else {
            qDebug() << "Peak search failed!";
        }
    };

    auto job = new Job(this, task, onFinished, true);
    //connect(progressView, SIGNAL(canceled()), job, SLOT(terminate()));

    job->exec();
}


void SessionModel::incorporateCalculatedPeaks()
{
    qDebug() << "Incorporating missing peaks into current data set...";

    DialogCalculatedPeaks dialog;

    if (!dialog.exec()) {
        return;
    }

    const double dmax = dialog.dMax();
    const double dmin = dialog.dMin();
    const double search_radius = dialog.searchRadius();

    std::vector<std::shared_ptr<DataSet>> numors = getSelectedNumors();

    std::shared_ptr<SX::Utils::ProgressHandler> handler(new SX::Utils::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    int current_numor = 0;


    int last_done = 0;
//    int predicted_peaks = 0;
    int observed_peaks = 0;

    for(std::shared_ptr<DataSet> numor: numors) {
        qDebug() << "Finding missing peaks for numor " << ++current_numor << " of " << numors.size();

//        auto& mono = numor->getDiffractometer()->getSource()->getSelectedMonochromator();
//        const double wavelength = mono.getWavelength();
//        std::vector<sptrPeak3D> calculated_peaks;

//        shared_ptr<Sample> sample = numor->getDiffractometer()->getSample();
//        unsigned int ncrystals = static_cast<unsigned int>(sample->getNCrystals());

//        for (unsigned int i = 0; i < ncrystals; ++i) {
//            SX::Crystal::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
//            auto cell = sample->getUnitCell(i);
//            auto UB = cell->getReciprocalStandardM();

//            handler->setStatus("Calculating peak locations...");

//            //auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
//            auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInShell(dmin, dmax, wavelength);

//            predicted_peaks += predicted_hkls.size();

//            std::vector<SX::Crystal::PeakCalc> peaks = numor->hasPeaks(predicted_hkls, UB);
//            calculated_peaks.reserve(peaks.size());

//            int current_peak = 0;

//            handler->setStatus("Building set of previously found peaks...");

//            std::set<sptrPeak3D> found_peaks = numor->getPeaks();
//            std::set<Eigen::RowVector3i, compare_fn> found_hkls;


//            Eigen::Vector3d lb = {0.0, 0.0, 0.0};
//            Eigen::Vector3d ub = {double(numor->getNCols()), double(numor->getNRows()), double(numor->getNFrames())};
//            auto&& octree = Octree(lb, ub);

//            octree.setMaxDepth(4);
//            octree.setMaxStorage(50);

//            handler->log("Building peak octree...");

//            for (sptrPeak3D p: found_peaks) {
//                found_hkls.insert(p->getIntegerMillerIndices());

//                if (!p->isSelected() || p->isMasked()) {
//                    continue;
//                }
//                octree.addData(&p->getShape());
//            }

//            handler->log("Done building octree; number of chambers is " + std::to_string(octree.numChambers()));

//            handler->setStatus("Adding calculated peaks...");

//            int done_peaks = 0;

//            #pragma omp parallel for
//            for (size_t peak_id = 0; peak_id < peaks.size(); ++peak_id) {
//                PeakCalc& p = peaks[peak_id];
//                ++current_peak;

//                Eigen::RowVector3i hkl(int(std::lround(p._h)), int(std::lround(p._k)), int(std::lround(p._l)));

//                // try to find this reflection in the list of peaks, skip if found
//                if (std::find(found_hkls.begin(), found_hkls.end(), hkl) != found_hkls.end() ) {
//                    continue;
//                }

//                // now we must add it, calculating shape from nearest peaks
//                 // K is outside the ellipsoid at PsptrPeak3D
//                sptrPeak3D new_peak = p.averagePeaks(octree, search_radius);
//                //sptrPeak3D new_peak = p.averagePeaks(numor);

//                if (!new_peak) {
//                    continue;
//                }

//                new_peak->linkData(numor);
//                new_peak->setSelected(true);
//                new_peak->addUnitCell(cell, true);
//                new_peak->setObserved(false);

//                #pragma omp critical
//                calculated_peaks.push_back(new_peak);

//                #pragma omp atomic
//                ++done_peaks;
//                int done = int(std::lround(done_peaks * 100.0 / peaks.size()));

//                if ( done != last_done) {
//                    handler->setProgress(done);
//                    last_done = done;
//                }
//            }
//        }
//        for (sptrPeak3D peak: calculated_peaks) {
//            numor->addPeak(peak);
//        }
//        qDebug() << "Integrating calculated peaks.";
//        numor->integratePeaks(_peakScale, _bkgScale, false, handler);

        auto predictor = SX::Crystal::PeakPredictor();

        predictor._dmin = dmin;
        predictor._dmax = dmax;
        predictor._searchRadius = search_radius; // todo
        predictor._peakScale = 1.0; // todo
        predictor._bkgScale = 3.0 ; // todo
        predictor._handler = handler;

        predictor.addPredictedPeaks(numor);
        // numor->addPredictedPeaks(dmin, dmax, handler);
        observed_peaks += numor->getPeaks().size();
    }
    updatePeaks();
    qDebug() << "Done incorporating missing peaks.";
    // qDebug() << "Q coverage = " << double(observed_peaks) / double(predicted_peaks) * 100.0 << "%";
}

void SessionModel::applyResolutionCutoff(double dmin, double dmax)
{
    int num_removed = 0;
    double avg_d = 0;
    int num_peaks = 0;

    qDebug() << "Applying resolution cutoff...";

    std::vector<std::shared_ptr<DataSet>> numors = getSelectedNumors();

    for(std::shared_ptr<DataSet> numor: numors) {
        std::vector<std::shared_ptr<Peak3D>> bad_peaks;
        shared_ptr<Sample> sample = numor->getDiffractometer()->getSample();

        for (std::shared_ptr<Peak3D> peak: numor->getPeaks()) {
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

    qDebug() << "Done applying resolution cutoff. Removed " << num_removed << " peaks.";
    qDebug() << "Average value of d for peaks is " << avg_d;
}


void SessionModel::writeLog()
{
    LogFileDialog dialog;
    std::vector<sptrPeak3D> peaks;

    auto numors = this->getSelectedNumors();

    for (auto numor: numors) {
        for (auto peak: numor->getPeaks()) {
            if (peak->isSelected() && !peak->isMasked()) {
                peaks.push_back(peak);
            }
        }
    }

    if (!peaks.size()) {
        qCritical() << "No peaks in the table";
        return;
    }

    // return if user cancels dialog
    if (!dialog.exec())
        return;

    bool friedel = dialog.friedel();

    if (dialog.writeUnmerged()) {
        if (!writeXDS(dialog.unmergedFilename(), peaks, false, friedel))
            qCritical() << "Could not write unmerged data to " << dialog.unmergedFilename().c_str();
    }

    if (dialog.writeMerged()) {
        if (!writeXDS(dialog.mergedFilename(), peaks, true, friedel))
            qCritical() << "Could not write unmerged data to " << dialog.mergedFilename().c_str();
    }

    if (dialog.writeStatistics()) {
        if (!writeStatistics(dialog.statisticsFilename(),
                             peaks,
                             dialog.dmin(), dialog.dmax(), dialog.numShells(), friedel))
            qCritical() << "Could not write statistics log to " << dialog.statisticsFilename().c_str();
    }
}

bool SessionModel::writeNewShellX(std::string filename, const std::vector<sptrPeak3D>& peaks)
{
    std::fstream file(filename, std::ios::out);
    std::vector<char> buf(1024, 0); // buffer for snprintf

    if (!file.is_open()) {
        qCritical() << "Error writing to this file, please check write permisions";
        return false;
    }

    auto sptrBasis = peaks[0]->getActiveUnitCell();

    if (!sptrBasis) {
        qCritical() << "No unit cell defined the peaks. No index can be defined.";
        return false;
    }

    for (sptrPeak3D peak: peaks) {
        if (peak->isMasked() || !peak->isSelected())
            continue;

        Eigen::RowVector3d hkl;
        auto sptrCurrentBasis = peak->getActiveUnitCell();

        if (sptrCurrentBasis != sptrBasis) {
            qCritical() << "Not all the peaks have the same unit cell. Multi crystal not implement yet";
            return false;
        }

        if (!(peak->getMillerIndices(*sptrCurrentBasis, hkl, true)))
            continue;

        const long h = std::lround(hkl[0]);
        const long k = std::lround(hkl[1]);
        const long l = std::lround(hkl[2]);

        double lorentz = peak->getLorentzFactor();
        double trans = peak->getTransmission();

        double intensity = peak->getCorrectedIntensity().getValue();
        double sigma = peak->getCorrectedIntensity().getSigma();

        std::snprintf(&buf[0], buf.size(), "  %4ld %4ld %4ld %15.2f %10.2f", h, k, l, intensity, sigma);
        file << &buf[0] << std::endl;
    }

    if (file.is_open())
        file.close();

    return true;
}

bool SessionModel::writeStatistics(std::string filename,
                                    const std::vector<SX::Crystal::sptrPeak3D> &peaks,
                                    double dmin, double dmax, unsigned int num_shells, bool friedel)
{
    std::fstream file(filename, std::ios::out);
    SX::Crystal::ResolutionShell res = {dmin, dmax, num_shells};
    std::vector<char> buf(1024, 0); // buffer for snprintf
    std::vector<SX::Crystal::MergedPeak> merged_peaks;
    Eigen::RowVector3d HKL(0.0, 0.0, 0.0);

    if (!file.is_open()) {
        qCritical() << "Error writing to this file, please check write permisions";
        return false;
    }

    if (peaks.size() == 0) {
        qCritical() << "No peaks to write to log!";
        return false;
    }

    auto cell = peaks[0]->getActiveUnitCell();
    auto grp = SX::Crystal::SpaceGroup(cell->getSpaceGroup());

    for (auto&& peak: peaks) {
        if (cell != peak->getActiveUnitCell()) {
            qCritical() << "Only one unit cell is supported at this time!!";
            return false;
        }
        res.addPeak(peak);
    }

    auto&& ds = res.getD();
    auto&& shells = res.getShells();

    std::vector<std::vector<sptrPeak3D>> all_equivs;

    file << "          dmin       dmax       nobs redundancy     r_meas    r_merge      r_pim" << std::endl;

    for (size_t i = 0; i < size_t(num_shells); ++i) {
        const double d_lower = ds[i];
        const double d_upper = ds[i+1];

        auto peak_equivs = grp.findEquivalences(shells[i], friedel);
        RFactor rfactor(peak_equivs);

        for (auto&& equiv: peak_equivs)
            all_equivs.push_back(equiv);

        double redundancy = double(shells[i].size()) / double(peak_equivs.size());

        std::snprintf(&buf[0], buf.size(),
                "    %10.2f %10.2f %10d %10.3f %10.3f %10.3f %10.3f",
                d_lower, d_upper, int(shells[i].size()), redundancy,
                rfactor.Rmeas(), rfactor.Rmerge(), rfactor.Rpim());

        file << &buf[0] << std::endl;

        for (auto equiv: peak_equivs) {
            SX::Crystal::MergedPeak new_peak(grp, friedel);

            for (auto peak: equiv) {
                // skip bad/masked peaks
                if (peak->isMasked() || !peak->isSelected())
                    continue;

                // skip misindexed peaks
                if (!peak->getMillerIndices(*cell, HKL, true)) {
                    continue;
                }
                // peak was not equivalent to any of the merged peaks
                new_peak.addPeak(peak);
            }

            if (new_peak.redundancy() > 0)
                merged_peaks.push_back(new_peak);
        }

         qDebug() << "Finished logging shell " << i+1;
    }

    file << "--------------------------------------------------------------------------------" << std::endl;

    RFactor rfactor(all_equivs);

    int num_peaks = 0;

    for (auto& equiv: all_equivs)
        num_peaks += equiv.size();

    double redundancy = double(num_peaks) / double(all_equivs.size());

    std::snprintf(&buf[0], buf.size(),
            "    %10.2f %10.2f %10d %10.3f %10.3f %10.3f %10.3f",
            dmin, dmax, num_peaks, redundancy,
            rfactor.Rmeas(), rfactor.Rmerge(), rfactor.Rpim());

    file << &buf[0] << std::endl << std::endl;

    auto compare_fn = [](const SX::Crystal::MergedPeak& p, const SX::Crystal::MergedPeak& q) -> bool
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

    std::sort(merged_peaks.begin(), merged_peaks.end(), compare_fn);

    file << "   h    k    l            I        sigma   nobs       chi2             std            std/I  "
         << std::endl;

    unsigned int total_peaks = 0;
    unsigned int bad_peaks = 0;

    for (auto&& peak: merged_peaks) {

        const auto hkl = peak.getIndex();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        const double intensity = peak.getIntensity().getValue();
        const double sigma = peak.getIntensity().getSigma();
        const double chi2 = peak.chiSquared();
        const int nobs = peak.redundancy();
        const double std = peak.std();
        const double rel_std = std / intensity;

        std::snprintf(&buf[0], buf.size(), "  %4d %4d %4d %15.2f %10.2f %3d %15.5f %15.5f %15.5f",
                h, k, l, intensity, sigma, nobs, chi2, std, rel_std);

        file << &buf[0];

        if (rel_std > 1.0) {
            file << " ***** ";
            ++bad_peaks;
        }

        file << std::endl;
        ++total_peaks;
    }


    file << std::endl;
    file << "total peaks: " << total_peaks << std::endl;
    file << "  bad peaks: " << bad_peaks << std::endl;

    qDebug() << "Done writing log file.";

    file.close();
    return true;
}

bool SessionModel::writeXDS(std::string filename, const std::vector<sptrPeak3D>& peaks, bool merge, bool friedel)
{
    const std::string date = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    SX::Data::XDS xds(peaks, merge, friedel, filename, date);
    std::fstream file(filename, std::ios::out);

    if (!file.is_open()) {
        qCritical() << "Could not open " << filename << " for writing.";
        return false;
    }
    bool result = xds.write(file);
    qDebug() << "Done writing log file.";
    return result;
}
