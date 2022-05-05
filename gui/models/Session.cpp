//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/Session.cpp
//! @brief     Implements class Session
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/Session.h"

#include "base/utils/Logger.h"
#include "base/utils/Path.h" // fileBasename
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/DataKeys.h"
#include "core/raw/MetaData.h"
#include "gui/MainWin.h"
#include "gui/connect/Sentinel.h"
#include "gui/dialogs/DataNameDialog.h"
#include "gui/dialogs/RawDataDialog.h"
#include "gui/models/Project.h"
#include "gui/subframe_merge/SubframeMergedPeaks.h"
#include "gui/subframe_filter/SubframeFilterPeaks.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_integrate/SubframeIntegrate.h"
#include "gui/subframe_predict/SubframePredictPeaks.h"
#include "gui/subframe_refiner/SubframeRefiner.h"
#include "gui/utility/CellComboBox.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/FoundPeakComboBox.h"
#include "gui/utility/IntegratedPeakComboBox.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PredictedPeakComboBox.h"

#include <QCollator>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <QMessageBox>


Session* gSession;

namespace {

// open a dialog to choose a name for a dataset; warn against name clashes with previous names
std::string askDataName(const std::string dataname0, const QStringList* const datanames_pre)
{
    DataNameDialog dataname_dialog(QString::fromStdString(dataname0), datanames_pre);
    dataname_dialog.exec();
    if (dataname_dialog.result())
        return dataname_dialog.dataName().toStdString();

    return dataname0;
}

} // namespace

Session::Session()
{
    gSession = this;

    //! For refreshing combos in other widgets
    _cell_combo = new CellComboBox();
    _data_combo = new DataComboBox();
    _peak_combo = new PeakComboBox();
    _found_peak_combo = new FoundPeakComboBox();
    _predicted_peak_combo = new PredictedPeakComboBox();
    _integrated_peak_combo = new IntegratedPeakComboBox();
}

Project* Session::currentProject()
{
    if (!hasProject())
        return nullptr;
    return _projects.at(_currentProject).get();
}

const Project* Session::currentProject() const
{
    if (!hasProject())
        throw std::runtime_error("Session::currentProject(): no projects available");
    return _projects.at(_currentProject).get();
}

bool Session::hasProject() const
{
    return !_projects.empty();
}

Project* Session::experimentAt(int i)
{
    if (!hasProject())
        throw std::runtime_error("Session::experimentAt(): no projects available");
    return _projects.at(i).get();
}
const Project* Session::experimentAt(int i) const
{
    if (_projects.size() == 0 || _projects.size() < i)
        return nullptr;

    return _projects.at(i).get();
}

int Session::currentProjectNum() const
{
    return _currentProject;
}
int Session::numExperiments() const
{
    return _projects.size();
}

Project* Session::createProject(QString experimentName, QString instrumentName)
{
    for (const QString& name : experimentNames()) { // check name
        if (name == experimentName) {
            return nullptr;
        }
    }
    return new Project(experimentName, instrumentName);
}

bool Session::addProject(std::unique_ptr<Project> project_ptr)
{
    _projects.push_back(std::move(project_ptr));
    _currentProject = _projects.size() - 1;
    onExperimentChanged();

    return true;
}

void Session::removeExperiment(unsigned int id)
{
    if (_projects.empty()) {
        return;
    } else {
        for (decltype(_projects)::const_iterator it = _projects.begin(); it != _projects.end();) {
            const Project& prj{**it};
            if (id == prj.id())
                it = _projects.erase(it);
            else
                ++it;
        }
    }
    _currentProject = _projects.empty() ? -1 : 0;
    onExperimentChanged();
}

std::vector<QString> Session::experimentNames() const
{
    std::vector<QString> ret;
    for (const auto& project : _projects)
        ret.push_back(QString::fromStdString(project->experiment()->name()));
    return ret;
}

void Session::selectProject(int select)
{
    if (select < _projects.size() && select >= 0)
        _currentProject = select;
    onExperimentChanged();
}

void Session::loadData(nsx::DataFormat format)
{
    // Loading data requires an existing Experiment
    if (_currentProject < 0) {
        QMessageBox::critical(nullptr, "Error", "Please create an experiment before loading data.");
        return;
    }

    QSettings qset;
    qset.beginGroup("RecentDirectories");
    QString loadDirectory = qset.value("data", QDir::homePath()).toString();

    QString format_string;
    switch (format) {
        case nsx::DataFormat::NSX: {
            format_string = QString("Data files(*.nsx);;all files (*.* *)");
            break;
        }
        case nsx::DataFormat::NEXUS: {
            format_string = QString("Data files(*.nxs);;all files (*.* *)");
            break;
        }
        default: {
            throw std::runtime_error(
                "Session::LoadData can only load NSX(HDF5) or Nexus data files");
            break;
        }
    }

    QStringList filenames =
        QFileDialog::getOpenFileNames(gGui, "import data", loadDirectory, format_string);

    if (filenames.empty())
        return;

    QFileInfo info(filenames.at(0));
    loadDirectory = info.absolutePath();
    qset.setValue("data", loadDirectory);
    std::string dataset1_name; // name of the first dataset (to be set by the user)

    for (const QString& filename : filenames) {
        QFileInfo fileinfo(filename);
        nsx::Experiment* exp = currentProject()->experiment();

        try {
            // For all data-readers, a valid diffractometer instrument is needed;
            // `_diffractometer` must not be null; otherwise undefined behaviour might occur
            if (!exp->getDiffractometer()) {
                throw std::runtime_error("Please set a valid instrument first");
            }

            // choose a name for the dataset
            // default dataset name: basename of the first data-file
            const QStringList& datanames_pre{currentProject()->getDataNames()};
            const std::string dataset_nm{
                askDataName(nsx::fileBasename(filename.toStdString()), &datanames_pre)};
            const nsx::sptrDataSet dataset_ptr{
                std::make_shared<nsx::DataSet>(dataset_nm, exp->getDiffractometer())};

            dataset_ptr->addDataFile(filename.toStdString(), "nsx");

            // store the name of the first dataset
            if (dataset1_name.empty())
                dataset1_name = dataset_ptr->name();

            dataset_ptr->finishRead();
            exp->addData(dataset_ptr);
        } catch (const std::exception& ex) {
            QString msg = QString("Loading file(s) '") + filename + QString("' failed with error: ")
                + QString(ex.what()) + QString(".");

            QMessageBox::critical(nullptr, "Error", msg);
            return;
        }
    }

    // select the first dataset
    currentProject()->selectData(currentProject()->getIndex(QString::fromStdString(dataset1_name)));
    onDataChanged();
    auto data_list = currentProject()->getDataNames();
    gGui->sentinel->setLinkedComboList(ComboType::DataSet, data_list);
}

void Session::removeData()
{
    if (_currentProject == -1)
        return;
    if (_selectedData == -1)
        return;

    std::string numorname = currentProject()->getData(_selectedData)->name();
    currentProject()->experiment()->removeData(numorname);
    onDataChanged();
}

bool Session::loadRawData()
{
    // Loading data requires an existing Experiment
    if (_currentProject < 0) {
        return false;
    }

    try {
        QSettings qset;
        qset.beginGroup("RecentDirectories");
        QString loadDirectory = qset.value("data_raw", QDir::homePath()).toString();

        QStringList qfilenames =
            QFileDialog::getOpenFileNames(gGui, "import raw data", loadDirectory);
        if (qfilenames.empty())
            return false;

        // Don't leave sorting the files to the OS. Use QCollator + std::sort to sort naturally
        // (numerically)
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(
            qfilenames.begin(), qfilenames.end(),
            [&collator](const QString& file1, const QString& file2) {
                return collator.compare(file1, file2) < 0;
            });

        QFileInfo info(qfilenames.at(0));
        loadDirectory = info.absolutePath();
        qset.setValue("data_raw", loadDirectory);

        std::vector<std::string> filenames;
        for (const QString& filename : qfilenames)
            filenames.push_back(filename.toStdString());

        nsx::RawDataReaderParameters parameters;
        parameters.dataset_name = nsx::fileBasename(filenames[0]);
        parameters.LoadDataFromFile(filenames.at(0));
        const QStringList& datanames_pre{currentProject()->getDataNames()};
        RawDataDialog dialog(parameters, datanames_pre);
        if (!dialog.exec()) {
            return false;
        }
        nsx::Experiment* exp = currentProject()->experiment();

        // update the parameters by those from the dialog
        parameters = dialog.parameters();

        nsx::Diffractometer* diff = exp->getDiffractometer();
        const std::shared_ptr<nsx::DataSet> dataset_ptr{
            std::make_shared<nsx::DataSet>(parameters.dataset_name, diff)};
        dataset_ptr->setRawReaderParameters(parameters);
        for (const auto& filenm : filenames)
            dataset_ptr->addRawFrame(filenm);

        dataset_ptr->finishRead();
        if (!exp->addData(dataset_ptr)) {
        }
        onDataChanged();
        auto data_list = currentProject()->getDataNames();
        gGui->sentinel->setLinkedComboList(ComboType::DataSet, data_list);
    } catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Error", QString(e.what()));
    } catch (...) {
        return false;
    }
    return true;
}

void Session::onDataChanged()
{
    DataList data = currentProject()->experiment()->getAllData();
    gGui->onDataChanged();
    _data_combo->clearAll();
    _data_combo->addDataSets(data);
    _data_combo->refreshAll();
    onPeaksChanged();
}

void Session::onExperimentChanged()
{
    if (!gSession->hasProject())
        return;
    if (currentProject()->experiment()->getDiffractometer()) {
        gGui->onExperimentChanged();
    }
    gGui->finder->grabFinderParameters();
    gGui->finder->grabIntegrationParameters();
    gGui->filter->grabFilterParameters();
    gGui->indexer->grabIndexerParameters();
    gGui->predictor->grabPredictorParameters();
    gGui->predictor->grabRefinerParameters();
    gGui->predictor->grabShapeCollectionParameters();
    gGui->refiner->grabRefinerParameters();
    gGui->integrator->grabIntegrationParameters();
    gGui->merger->grabMergeParameters();
    onDataChanged();
    onUnitCellChanged();
}

void Session::onPeaksChanged()
{
    PeakList peaks = currentProject()->experiment()->getPeakCollections();
    gGui->onPeaksChanged();
    _peak_combo->clearAll();
    _peak_combo->addPeakCollections(peaks);
    _peak_combo->refreshAll();
    _found_peak_combo->clearAll();
    _found_peak_combo->addPeakCollections(peaks);
    _found_peak_combo->refreshAll();
    _predicted_peak_combo->clearAll();
    _predicted_peak_combo->addPeakCollections(peaks);
    _predicted_peak_combo->refreshAll();
    _integrated_peak_combo->clearAll();
    _integrated_peak_combo->addPeakCollections(peaks);
    _integrated_peak_combo->refreshAll();
}

void Session::onUnitCellChanged()
{
    CellList cells = currentProject()->experiment()->getSptrUnitCells();
    gGui->onUnitCellChanged();
    _cell_combo->clearAll();
    _cell_combo->addCells(cells);
    _cell_combo->refreshAll();
}

void Session::loadExperimentFromFile(QString filename)
{
    std::unique_ptr<Project> project_ptr{createProject(
        QString::fromStdString(nsx::kw_experimentDefaultName),
        QString::fromStdString(nsx::kw_diffractometerDefaultName))};

    if (!project_ptr)
        return;

    nsx::nsxlog(
        nsx::Level::Debug, "Session: Created Project for file '", filename.toStdString(), "'");

    try {
        project_ptr->experiment()->loadFromFile(filename.toStdString());
        nsx::nsxlog(
            nsx::Level::Debug, "Session: Loaded data for Project created from file '",
            filename.toStdString(), "'");

        project_ptr->generatePeakModels();

        nsx::nsxlog(
            nsx::Level::Debug, "Session: Generated PeakModels for Project created from file '",
            filename.toStdString(), "'");

    } catch (const std::exception& ex) {
        const std::string msg{
            "Loading experiment from '" + filename.toStdString()
            + "' failed with error: " + ex.what() + "."};
        throw std::runtime_error(msg);
        return;
    }

    addProject(std::move(project_ptr));
    onExperimentChanged();

    nsx::nsxlog(
        nsx::Level::Debug, "Session: Finished creating Project for file '", filename.toStdString(),
        "'");
}

bool Session::UpdateExperimentData(unsigned int idx, QString name, QString instrument)
{
    if (idx >= _projects.size())
        return false;

    for (const auto& e : _projects) { // excluding duplicate project names
        if (e->experiment()->name() == name.toStdString()) {
            // the selected item is allowed to have an identical name!
            // This allows to change instument name and keeo the same experiment name.
            // therefore ->
            if (_projects.at(idx) != e)
                return false;
        }
    }
    _projects.at(idx)->experiment()->setName(name.toStdString());
    _projects.at(idx)->experiment()->setDiffractometer(instrument.toStdString());
    return true;
}

std::string Session::generateExperimentName()
{
    int n = 3;
    std::string str = std::to_string(_projects.size() + 1);
    if (str.size() > n) { //
        return "New Experiment";
    }
    return std::string("Experiment") + std::string(n - str.size(), '0').append(str);
}
