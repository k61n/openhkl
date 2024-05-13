//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/Session.cpp
//! @brief     Implements class Session
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/Session.h"

#include "base/utils/Logger.h"
#include "base/utils/Path.h" // fileBasename
#include "core/data/DataSet.h"
#include "core/data/SingleFrame.h"
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/ExperimentYAML.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/loader/TiffDataReader.h"
#include "core/raw/MetaData.h"
#include "gui/MainWin.h"
#include "gui/dialogs/DataNameDialog.h"
#include "gui/dialogs/ImageReaderDialog.h"
#include "gui/models/Project.h"
#include "gui/subframe_filter/SubframeFilter.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_integrate/SubframeIntegrate.h"
#include "gui/subframe_merge/SubframeMerge.h"
#include "gui/subframe_predict/SubframePredict.h"
#include "gui/subframe_refiner/SubframeRefiner.h"
#include "gui/utility/CellComboBox.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/FoundPeakComboBox.h"
#include "gui/utility/IntegratedPeakComboBox.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PredictedPeakComboBox.h"
#include "gui/utility/ShapeComboBox.h"
#include "gui/utility/SideBar.h"
#include "gui/widgets/DirectBeamWidget.h"

#include <QCollator>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <stdexcept>

Session* gSession;

namespace {

//! Opens a dialog to choose a name for a dataset; warn against name clashes with previous names
std::string askDataName(const std::string dataname0, const QStringList* const datanames_pre)
{
    DataNameDialog dataname_dialog(QString::fromStdString(dataname0), datanames_pre);
    dataname_dialog.exec();
    if (dataname_dialog.result())
        return dataname_dialog.dataName().toStdString();

    return dataname0;
}

//! Opens a dialog to choose a list of raw files
std::pair<QString, QStringList> askFileNames(ohkl::DataFormat fmt)
{
    QSettings qset = gGui->qSettings();
    qset.beginGroup("RecentDirectories");

    QStringList qfilenames;
    QString loadDirectory;
    switch (fmt) {
        case ohkl::DataFormat::RAW: {
            loadDirectory = qset.value("data_raw", QDir::homePath()).toString();
            qfilenames = QFileDialog::getOpenFileNames(
                gGui, "Import raw data", loadDirectory, "Image files (*.raw);; All files (*.* *)");
            break;
        }
        case ohkl::DataFormat::TIFF: {
            loadDirectory = qset.value("data_tiff", QDir::homePath()).toString();
            qfilenames = QFileDialog::getOpenFileNames(
                gGui, "Import tiff data", loadDirectory,
                "Image files (*.tif *.tiff);; All files (*.* *)");
            break;
        }
        case ohkl::DataFormat::PLAINTEXT: {
            loadDirectory = qset.value("data_text", QDir::homePath()).toString();
            qfilenames = QFileDialog::getOpenFileNames(
                gGui, "Import plain text data", loadDirectory,
                "Plain text files (*.dat *.txt);; All files (*.* *)");

            break;
        }
        default: throw std::runtime_error("askFileNames: Invalid DataFormat");
    }

    if (qfilenames.empty())
        return {};

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
    switch (fmt) {
        case ohkl::DataFormat::RAW: {
            qset.setValue("data_raw", loadDirectory);
            break;
        }
        case ohkl::DataFormat::TIFF: {
            qset.setValue("data_tiff", loadDirectory);
            break;
        }
        case ohkl::DataFormat::PLAINTEXT: {
            qset.setValue("data_text", loadDirectory);
            break;
        }
        default: throw std::runtime_error("askFileNames: Invalid DataFormat");
    }
    return {loadDirectory, qfilenames};
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
    _shape_combo = new ShapeComboBox();

    _beam_setter_widget = new DirectBeamWidget();
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

Project* Session::createProject(QString experimentName, QString instrumentName, bool strategy)
{
    for (const QString& name : experimentNames()) { // check name
        if (name == experimentName) {
            return nullptr;
        }
    }
    return new Project(experimentName, instrumentName, strategy);
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

void Session::loadData(ohkl::DataFormat format)
{
    // Loading data requires an existing Experiment
    if (_currentProject < 0) {
        QMessageBox::critical(nullptr, "Error", "Please create an experiment before loading data.");
        return;
    }

    QSettings qset = gGui->qSettings();
    qset.beginGroup("RecentDirectories");
    QString loadDirectory = qset.value("data", QDir::homePath()).toString();

    QString format_string;
    switch (format) {
        case ohkl::DataFormat::OHKL: {
            format_string = QString("Data files(*.ohkl);;all files (*.* *)");
            break;
        }
        case ohkl::DataFormat::NEXUS: {
            format_string = QString("Data files(*.nxs);;all files (*.* *)");
            break;
        }
        default: {
            throw std::runtime_error("Session::LoadData can only load OHKL or Nexus data files");
            break;
        }
    }

    QStringList filenames =
        QFileDialog::getOpenFileNames(gGui, "import data", loadDirectory, format_string);

    if (filenames.empty())
        return;

    QFileInfo info(filenames.at(0));
    loadDirectory = info.absolutePath();
    currentProject()->setDirectory(loadDirectory);
    currentProject()->readYaml();
    qset.setValue("data", loadDirectory);
    std::string dataset1_name; // name of the first dataset (to be set by the user)

    for (const QString& filename : filenames) {
        QFileInfo fileinfo(filename);
        ohkl::Experiment* exp = currentProject()->experiment();

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
                askDataName(ohkl::fileBasename(filename.toStdString()), &datanames_pre)};
            const ohkl::sptrDataSet dataset_ptr{
                std::make_shared<ohkl::DataSet>(dataset_nm, exp->getDiffractometer())};

            dataset_ptr->addDataFile(filename.toStdString(), format);

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
}

void Session::removeData()
{
    if (_currentProject == -1)
        return;
    if (_selectedData == -1)
        return;

    std::string dataname = currentProject()->getData(_selectedData)->name();
    currentProject()->experiment()->removeData(dataname);
    onDataChanged();
}

void Session::writeYaml()
{
    if (hasProject()) {
        for (auto& project : _projects)
            project->writeYaml();
    }
}

bool Session::loadRawData(bool single_file /* = false */)
{
    if (_currentProject < 0)
        return false;

    try {
        // Get input filenames from dialog.
        QStringList filenames;
        QString path;
        std::tie(path, filenames) = askFileNames(ohkl::DataFormat::RAW);
        if (filenames.empty())
            return false;
        if (single_file && filenames.size() > 1)
            throw std::runtime_error("Session::loadRawData expected exactly one file");

        // Get metadata from readme file, then edit them in dialog.
        const QStringList& extant_dataset_names = currentProject()->getDataNames();

        ohkl::DataReaderParameters parameters;
        QString yml_file = QString::fromStdString(currentProject()->experiment()->name() + ".yml");
        QString yml_path = QDir(path).filePath(yml_file);
        QFileInfo info(yml_path);
        if (info.exists() && info.isFile())
            parameters.loadFromYAML(yml_path.toStdString());
        ImageReaderDialog dialog(filenames, &parameters, ohkl::DataFormat::RAW);
        dialog.setWindowTitle("Raw data parameters");
        if (single_file)
            dialog.setSingleImageMode();

        if (!dialog.exec())
            return false;

        currentProject()->setDirectory(path);
        ohkl::Experiment* exp = currentProject()->experiment();
        parameters = dialog.dataReaderParameters();

        // Transfer metadata to diffractometer.
        ohkl::Detector* detector = exp->getDiffractometer()->detector();
        detector->setBaseline(parameters.baseline);
        detector->setGain(parameters.gain);

        // Transfer metadata to dataset, and load the raw data.
        if (single_file) {
            const std::shared_ptr<ohkl::DataSet> dataset{std::make_shared<ohkl::SingleFrame>(
                parameters.dataset_name, exp->getDiffractometer())};
            dataset->setImageReaderParameters(parameters);
            dataset->addRawFrame(filenames[0].toStdString());
            dataset->finishRead();
            parameters.cols = dataset->nCols();
            parameters.rows = dataset->nRows();
            exp->addData(dataset);

        } else {
            const std::shared_ptr<ohkl::DataSet> dataset{
                std::make_shared<ohkl::DataSet>(parameters.dataset_name, exp->getDiffractometer())};
            dataset->setImageReaderParameters(parameters);
            for (const auto& filename : filenames)
                dataset->addRawFrame(filename.toStdString());
            dataset->finishRead();
            parameters.cols = dataset->nCols();
            parameters.rows = dataset->nRows();
            exp->addData(dataset);
        }

        ohkl::ExperimentYAML yaml(yml_path.toStdString());
        yaml.setDataReaderParameters(&parameters);
        yaml.writeFile(yml_path.toStdString());
        onDataChanged();
    } catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Error", QString(e.what()));
    } catch (...) {
        return false;
    }
    return true;
}

bool Session::loadTiffData(bool single_file /* = false */)
{
    if (_currentProject < 0)
        return false;

    ohkl::Experiment* exp = currentProject()->experiment();
    ohkl::Detector* detector = exp->getDiffractometer()->detector();
    ohkl::DataReaderParameters params;

    try {
        // Get input filenames from dialog.
        QStringList filenames;
        QString path;
        std::tie(path, filenames) = askFileNames(ohkl::DataFormat::TIFF);
        if (filenames.empty())
            return false;
        if (single_file && filenames.size() > 1)
            throw std::runtime_error("Session::loadTiffData expected exactly one file");

        std::string ext = ""; // let's store the used file extension for later

        const QStringList& extant_dataset_names = currentProject()->getDataNames();
        QString yml_file = QString::fromStdString(currentProject()->experiment()->name() + ".yml");
        QString yml_path = QDir(path).filePath(yml_file);
        QFileInfo info(yml_path);
        if (info.exists() && info.isFile())
            params.loadFromYAML(yml_path.toStdString());
        ImageReaderDialog dialog(
            filenames, static_cast<ohkl::DataReaderParameters*>(&params), ohkl::DataFormat::TIFF);

        dialog.setWindowTitle("Tiff data parameters");
        if (single_file)
            dialog.setSingleImageMode();

        if (!dialog.exec())
            return false;

        currentProject()->setDirectory(path);
        params = dialog.dataReaderParameters();
        detector->setBaseline(params.baseline);
        detector->setGain(params.gain);

        if (single_file) {
            const std::shared_ptr<ohkl::DataSet> dataset{
                std::make_shared<ohkl::SingleFrame>(params.dataset_name, exp->getDiffractometer())};
            dataset->setImageReaderParameters(params);
            dataset->addTiffFrame(filenames[0].toStdString());
            dataset->finishRead();
            exp->addData(dataset);
        } else {
            const std::shared_ptr<ohkl::DataSet> dataset{
                std::make_shared<ohkl::DataSet>(params.dataset_name, exp->getDiffractometer())};
            dataset->setImageReaderParameters(params);
            for (const auto& filename : filenames)
                dataset->addTiffFrame(filename.toStdString());
            dataset->finishRead();
            exp->addData(dataset);
        }

        onDataChanged();
    } catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Error", QString(e.what()));
    } catch (...) {
        return false;
    }
    return true;
}

bool Session::loadPlainTextData(bool single_file /* = false */)
{
    if (_currentProject < 0)
        return false;

    ohkl::Experiment* exp = currentProject()->experiment();
    ohkl::Detector* detector = exp->getDiffractometer()->detector();
    ohkl::DataReaderParameters params;

    try {
        // Get input filenames from dialog.
        QStringList filenames;
        QString path;
        std::tie(path, filenames) = askFileNames(ohkl::DataFormat::PLAINTEXT);
        if (filenames.empty())
            return false;
        if (single_file && filenames.size() > 1)
            throw std::runtime_error("Session::loadPlainTextData expected exactly one file");

        std::string ext = ""; // let's store the used file extension for later

        const QStringList& extant_dataset_names = currentProject()->getDataNames();
        QString yml_file = QString::fromStdString(currentProject()->experiment()->name() + ".yml");
        QString yml_path = QDir(path).filePath(yml_file);
        QFileInfo info(yml_path);
        if (info.exists() && info.isFile())
            params.loadFromYAML(yml_path.toStdString());
        ImageReaderDialog dialog(
            filenames, static_cast<ohkl::DataReaderParameters*>(&params), ohkl::DataFormat::PLAINTEXT);

        dialog.setWindowTitle("Plain text data parameters");
        if (single_file)
            dialog.setSingleImageMode();

        if (!dialog.exec())
            return false;

        currentProject()->setDirectory(path);
        params = dialog.dataReaderParameters();
        detector->setBaseline(params.baseline);
        detector->setGain(params.gain);

        if (single_file) {
            const std::shared_ptr<ohkl::DataSet> dataset{
                std::make_shared<ohkl::SingleFrame>(params.dataset_name, exp->getDiffractometer())};
            dataset->setImageReaderParameters(params);
            dataset->addPlainTextFrame(filenames[0].toStdString());
            dataset->finishRead();
            exp->addData(dataset);
        } else {
            const std::shared_ptr<ohkl::DataSet> dataset{
                std::make_shared<ohkl::DataSet>(params.dataset_name, exp->getDiffractometer())};
            dataset->setImageReaderParameters(params);
            for (const auto& filename : filenames)
                dataset->addPlainTextFrame(filename.toStdString());
            dataset->finishRead();
            exp->addData(dataset);
        }

        onDataChanged();
    } catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Error", QString(e.what()));
    } catch (...) {
        return false;
    }
    return true;
}

void Session::onDataChanged()
{
    QSignalBlocker blocker(_data_combo);
    DataList data = currentProject()->experiment()->getAllData();
    _data_combo->clearAll();
    _data_combo->addDataSets(data);
    _data_combo->refreshAll();

    if (!gSession->currentProject()->hasDataSet())
        return;

    double x_offset = _data_combo->currentData()
                            ->diffractometer()
                            ->source()
                            .selectedMonochromator()
                            .xOffset();
    double y_offset = _data_combo->currentData()
                            ->diffractometer()
                            ->source()
                            .selectedMonochromator()
                            .yOffset();
    _beam_setter_widget->onBeamPosChanged({x_offset, y_offset});
    onPeaksChanged();
    onUnitCellChanged();
    onShapesChanged();
}

void Session::onExperimentChanged()
{
    if (!gSession->hasProject())
        return;
    gGui->sideBar()->setStrategyMode(gSession->currentProject()->strategyMode());
    gGui->finder->grabFinderParameters();
    gGui->finder->grabIntegrationParameters();
    gGui->filter->grabFilterParameters();
    gGui->indexer->grabIndexerParameters();
    gGui->predictor->grabPredictorParameters();
    gGui->predictor->grabRefinerParameters();
    gGui->predictor->grabShapeModelParameters();
    gGui->refiner->grabRefinerParameters();
    gGui->integrator->grabIntegrationParameters();
    gGui->merger->grabMergeParameters();
    onDataChanged();
    onUnitCellChanged();
}

void Session::onPeaksChanged()
{
    PeakList peaks =
        currentProject()->experiment()->getPeakCollections(currentProject()->currentData());

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

    gGui->sideBar()->refreshCurrent();
}

void Session::onUnitCellChanged()
{
    CellList cells = currentProject()->experiment()->getSptrUnitCells(currentProject()->currentData());

    _cell_combo->clearAll();
    _cell_combo->addCells(cells);
    _cell_combo->refreshAll();
}

void Session::onShapesChanged()
{
    ShapesList shape_list =
        currentProject()->experiment()->getShapeModels(currentProject()->currentData());
    _shape_combo->clearAll();
    _shape_combo->addShapeModels(shape_list);
    _shape_combo->refreshAll();
}

void Session::loadExperimentFromFile(QString filename)
{
    std::unique_ptr<Project> project_ptr{createProject(
        QString::fromStdString(ohkl::kw_experimentDefaultName),
        QString::fromStdString(ohkl::kw_diffractometerDefaultName))};

    if (!project_ptr)
        return;

    ohkl::ohklLog(
        ohkl::Level::Debug, "Session: Created Project for file '", filename.toStdString(), "'");

    try {
        project_ptr->experiment()->loadFromFile(filename.toStdString());
        project_ptr->setStrategyMode(project_ptr->experiment()->strategy());
        ohkl::ohklLog(
            ohkl::Level::Debug, "Session: Loaded data for Project created from file '",
            filename.toStdString(), "'");

        project_ptr->generatePeakModels();

        ohkl::ohklLog(
            ohkl::Level::Debug, "Session: Generated PeakModels for Project created from file '",
            filename.toStdString(), "'");

        QFileInfo info(filename);
        QString path = info.absolutePath();
        project_ptr->setDirectory(path);
        project_ptr->readYaml();

    } catch (const std::exception& ex) {
        const std::string msg{
            "Loading experiment from '" + filename.toStdString()
            + "' failed with error: " + ex.what() + "."};
        throw std::runtime_error(msg);
        return;
    }

    addProject(std::move(project_ptr));
    onExperimentChanged();

    ohkl::ohklLog(
        ohkl::Level::Debug, "Session: Finished creating Project for file '", filename.toStdString(),
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
