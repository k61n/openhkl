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

 Forshungszentrum Juelich GmbH
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
#include "DialogConvolve.h"
#include "ProgressHandler.h"
#include "ProgressView.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <map>
#include <array>
#include <tuple>
#include <vector>

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

//#include "BlobFinder.h"
#include "DataReaderFactory.h"
#include "Detector.h"
#include "DialogExperiment.h"
#include "Diffractometer.h"
#include "Sample.h"
#include "Source.h"

#include "PeakTableView.h"
#include "AbsorptionDialog.h"
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
#include "MCAbsorptionDialog.h"
#include "OpenGL/GLWidget.h"
#include "OpenGL/GLSphere.h"
#include "Logger.h"
#include "ReciprocalSpaceViewer.h"
#include "DetectorScene.h"

#include "SpaceGroupSymbols.h"
#include "SpaceGroup.h"

#include "SpaceGroupDialog.h"

#include <QVector>
#include "Externals/qcustomplot.h"
#include "ui_ScaleDialog.h"

#include "ScaleDialog.h"
#include "FriedelDialog.h"

#include "RFactor.h"
#include <hdf5.h>
#include <H5Exception.h>

#include "PeakFitDialog.h"

#include "DialogConvolve.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>


using std::vector;
using SX::Data::IData;
using std::shared_ptr;
using SX::Utils::ProgressHandler;

SessionModel::SessionModel()
{
}

SessionModel::~SessionModel()
{
    // _model should be deleted automatically during destructor by QT
    //delete _model;
}

void SessionModel::createNewExperiment()
{
    DialogExperiment* dlg;

    // DialogExperiment could throw an exception if it fails to read the resource files
    try {
        dlg = new DialogExperiment();

        // The user pressed cancel, return
        if (!dlg->exec())
            return;

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

vector<shared_ptr<IData>> SessionModel::getSelectedNumors(ExperimentItem* item) const
{
    vector<shared_ptr<IData>> numors;

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

    for (auto&& expr: experiments) {
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

vector<shared_ptr<IData>> SessionModel::getSelectedNumors() const
{
    vector<shared_ptr<IData>> numors;

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

    std::vector<std::shared_ptr<IData>> numors = getSelectedNumors();
    std::vector<std::vector<sptrPeak3D>> peak_equivs;
    std::vector<sptrPeak3D> peak_list;

    std::shared_ptr<UnitCell> unit_cell;

    for (std::shared_ptr<IData> numor: numors)
    {
        std::set<sptrPeak3D> peaks = numor->getPeaks();
        for (sptrPeak3D peak: peaks)
        {
            if ( peak && peak->isSelected() && !peak->isMasked() )
                peak_list.push_back(peak);
        }
    }

    if ( peak_list.size() == 0) {
        qDebug() << "No peaks -- cannot search for equivalences!";
        return;
    }

    for (sptrPeak3D peak: peak_list) {
        // what do we do if there is more than one sample/unit cell??
        unit_cell = peak->getUnitCell();

        if (unit_cell)
            break;
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

    std::map<int, int> size_counts;

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

void SessionModel::integrateCalculatedPeaks()
{
    qDebug() << "Integrating calculated peaks...";

    int count = 0;
    Eigen::Vector3d peak_extent, bg_extent;
    peak_extent << 0.0, 0.0, 0.0;
    bg_extent << 0.0, 0.0, 0.0;

    std::shared_ptr<UnitCell> unit_cell;

    for (std::shared_ptr<IData> numor: getSelectedNumors())
    {
        for (sptrPeak3D peak: numor->getPeaks())
        {
            if ( peak && peak->isSelected() && !peak->isMasked() )
            {
                peak_extent += peak->getPeak()->getAABBExtents();
                bg_extent += peak->getBackground()->getAABBExtents();
                ++count;
            }
        }
    }

    if ( count == 0) {
        qDebug() << "No peaks -- cannot search for equivalences!";
        return;
    }

    peak_extent /= count;
    bg_extent /= count;

    qDebug() << "Done calculating average bounding box";

    qDebug() << peak_extent(0) << " " << peak_extent(1) << " " << peak_extent(2);
    qDebug() << bg_extent(0) << " " << bg_extent(1) << " " << bg_extent(2);

    for (std::shared_ptr<IData> numor: getSelectedNumors())
    {
        std::vector<sptrPeak3D> calculated_peaks;

        shared_ptr<Sample> sample = numor->getDiffractometer()->getSample();
        int ncrystals = sample->getNCrystals();

        if (ncrystals) {
            for (int i = 0; i < ncrystals; ++i) {
                SX::Crystal::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
                auto ub = sample->getUnitCell(i)->getReciprocalStandardM();

                qDebug() << "Calculating peak locations...";

                auto hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
                std::vector<SX::Crystal::PeakCalc> peaks = numor->hasPeaks(hkls, ub);
                calculated_peaks.reserve(calculated_peaks.size() + peaks.size());

                qDebug() << "Adding calculated peaks...";

                for(auto&& p: peaks) {
                    //calculated_peaks.push_back(p);
                }
            }
        }

        qDebug() << "Done.";
    }
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
    std::vector<std::shared_ptr<SX::Data::IData>> selectedNumors;
    int nTotalNumors(rowCount(ditem->index()));

    selectedNumors.reserve(nTotalNumors);

    for (auto i = 0; i < nTotalNumors; ++i) {
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

    int max=selectedNumors.size();
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
            numor->releaseMemory();
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

    std::vector<std::shared_ptr<IData>> numors = getSelectedNumors();

    std::shared_ptr<SX::Utils::ProgressHandler> handler(new SX::Utils::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    int current_numor = 0;

    class compare_fn {
    public:
        auto operator()(const Eigen::RowVector3i a, const Eigen::RowVector3i b) -> bool
        {
            if (a(0) != b(0))
                return a(0) < b(0);

            if (a(1) != b(1))
                return a(1) < b(1);

            return a(2) < b(2);
        }
    };

    int last_done = 0;

    for(std::shared_ptr<IData> numor: numors)
    {
        qDebug() << "Finding missing peaks for numor " << ++current_numor << " of " << numors.size();

        std::vector<sptrPeak3D> calculated_peaks;

        shared_ptr<Sample> sample = numor->getDiffractometer()->getSample();
        int ncrystals = sample->getNCrystals();

        for (int i = 0; i < ncrystals; ++i)
        {
            SX::Crystal::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
            auto ub = sample->getUnitCell(i)->getReciprocalStandardM();

            handler->setStatus("Calculating peak locations...");

            //auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
            auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.0);
            std::vector<SX::Crystal::PeakCalc> peaks = numor->hasPeaks(predicted_hkls, ub);
            calculated_peaks.reserve(peaks.size());

            int current_peak = 0;

            handler->setStatus("Building set of previously found peaks...");

            std::set<sptrPeak3D> found_peaks = numor->getPeaks();
            std::set<Eigen::RowVector3i, compare_fn> found_hkls;

            for (sptrPeak3D p: found_peaks)
                found_hkls.insert(p->getIntegerMillerIndices());

            handler->setStatus("Adding calculated peaks...");

            int done_peaks = 0;

            #pragma omp parallel for
            for (int peak_id = 0; peak_id < peaks.size(); ++peak_id) {
                PeakCalc& p = peaks[peak_id];
                ++current_peak;

                Eigen::RowVector3i hkl(std::round(p._h), std::round(p._k), std::round(p._l));

                // try to find this reflection in the list of peaks, skip if found
                if (std::find(found_hkls.begin(), found_hkls.end(), hkl) != found_hkls.end() )
                    continue;

                // now we must add it, calculating shape from nearest peaks
                sptrPeak3D new_peak = p.averagePeaks(numor, 200);

                if (!new_peak)
                    continue;

                new_peak->setSelected(true);
                #pragma omp critical
                calculated_peaks.push_back(new_peak);

                #pragma omp atomic
                ++done_peaks;
                int done = std::round(done_peaks * 100.0 / peaks.size());

                if ( done != last_done) {
                    handler->setProgress(done);
                    last_done = done;
                }
            }
        }

        for (sptrPeak3D peak: calculated_peaks)
            numor->addPeak(peak);

        numor->integratePeaks(handler);
    }

    qDebug() << "Done incorporating missing peaks.";
}
