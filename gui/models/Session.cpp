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
#include "base/utils/Path.h" // fileBasename
#include "base/utils/StringIO.h" // join
#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/DataKeys.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"
#include "gui/MainWin.h"
#include "gui/dialogs/DataNameDialog.h"
#include "gui/dialogs/RawDataDialog.h"
#include "gui/models/Project.h"

#include <QCollator>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>


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
}

Project* Session::currentProject()
{
    return _projects.at(_currentProject).get();
}
const Project* Session::currentProject() const
{
    return _projects.at(_currentProject).get();
}

Project* Session::experimentAt(int i)
{
    return _projects.at(i).get();
}
const Project* Session::experimentAt(int i) const
{
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

Project* Session::createProject
(QString experimentName, QString instrumentName)
{
    for (const QString& name : experimentNames()) {
        if (name == experimentName) {
            QMessageBox::critical(nullptr, "Error",
               "Experiment name, '" + experimentName + "' already exists");
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

void Session::removeExperiment(const QString& name)
{
    if (_projects.size() == 0) {
        return;
    } else {
        const std::string name_str {name.toStdString()};
        for (decltype(_projects)::const_iterator it = _projects.begin();
             it != _projects.end(); ) {
            const Project& prj {**it};
            if (name_str == prj.experiment()->name())
                it = _projects.erase(it);
            else
                ++it;
        }

    }
    _currentProject = _projects.size() > 0 ? 0 : -1;
    onExperimentChanged();
}

std::vector<QString> Session::experimentNames() const
{
    std::vector<QString> ret;
    for (const auto& project : _projects)
        ret.push_back(QString::fromStdString(project->experiment()->name()));
    return ret;
}

void Session::selectExperiment(int select)
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
        case nsx::DataFormat::HDF5: {
            format_string = QString("Data files(*.h5 *.hdf5 *.hdf);;all files (*.* *)");
            break;
        }
        case nsx::DataFormat::NEXUS: {
            format_string = QString("Data files(*.nxs);;all files (*.* *)");
            break;
        }
        default: {
            throw std::range_error("Session::LoadData can only load HDF5 or Nexus data files");
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

            std::string extension = fileinfo.completeSuffix().toStdString();
            nsx::sptrDataSet dataset_ptr = nsx::DataReaderFactory().create(
                extension, filename.toStdString(), exp->getDiffractometer());

            // choose a name for the dataset
            // default dataset name: basename of the first data-file
            const QStringList& datanames_pre{currentProject()->getDataNames()};
            const std::string dataset_nm0{nsx::fileBasename(filename.toStdString())};
            const std::string dataname{askDataName(dataset_nm0, &datanames_pre)};
            // add the list of sources as metadata
            dataset_ptr->metadata().add<std::string>(
                nsx::at_datasetSources, filename.toStdString());
            dataset_ptr->setName(dataname);
            // store the name of the first dataset
            if (dataset1_name.empty())
                dataset1_name = dataset_ptr->name();

            exp->addData(dataset_ptr, dataset_ptr->name());
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

    std::string numorname = currentProject()->getData(_selectedData)->name();
    currentProject()->experiment()->removeData(numorname);
    onDataChanged();
}

void Session::loadRawData()
{
    // Loading data requires an existing Experiment
    if (_currentProject < 0) {
        QMessageBox::critical(nullptr, "Error", "Please create an experiment before loading data.");
        return;
    }

    try {
        QSettings qset;
        qset.beginGroup("RecentDirectories");
        QString loadDirectory = qset.value("data_raw", QDir::homePath()).toString();

        QStringList qfilenames =
            QFileDialog::getOpenFileNames(gGui, "import raw data", loadDirectory);
        if (qfilenames.empty())
            return;

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

        RawDataDialog dialog;
        if (!dialog.exec())
            return;
        nsx::Experiment* exp = currentProject()->experiment();

        nsx::RawDataReaderParameters parameters;
        parameters.wavelength = dialog.wavelength();
        parameters.delta_omega = dialog.deltaOmega();
        parameters.delta_chi = dialog.deltaChi();
        parameters.delta_phi = dialog.deltaPhi();
        parameters.row_major = dialog.rowMajor();
        parameters.swap_endian = dialog.swapEndian();
        parameters.bpp = dialog.bpp();

        const double eps = 1e-8;
        if (parameters.wavelength < eps)
            throw std::runtime_error(
                "Wavelength, " + std::to_string(parameters.wavelength) + ", must be > 0");

        nsx::Diffractometer* diff = exp->getDiffractometer();
        auto reader{std::make_unique<nsx::RawDataReader>("::RawDataReader::", diff)};
        reader->setParameters(parameters);

        // Metadata for the DataSet, accumulated from the individual metadata
        nsx::MetaData metadata;

        for (size_t i = 0; i < filenames.size(); ++i) {
            reader->addFrame(filenames[i]);
            metadata.addMap(reader->metadata().map());
        }
        reader->end();
        // include the latest changes from reader metadata (eg., `npdone`)
        metadata.addMap(reader->metadata().map());


        const std::shared_ptr<nsx::DataSet> dataset{
            std::make_shared<nsx::DataSet>(std::move(reader))};

        // choose a name for the dataset
        // default data name: name of the first data-file
        const QStringList& datanames_pre{currentProject()->getDataNames()};
        const std::string dataset_nm0{nsx::fileBasename(filenames[0])};
        const std::string dataname{askDataName(dataset_nm0, &datanames_pre)};
        dataset->setName(dataname);
        metadata.add(nsx::at_datasetSources, nsx::join(filenames, ", "));
        dataset->metadata().setMap(metadata.map());
        exp->addData(dataset, dataset->name());
        // _selectedData = currentProject()->getIndex(qfilenames.at(0));
        onDataChanged();
    } catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Error", QString(e.what()));
    } catch (...) {
        return;
    }
}

void Session::onDataChanged()
{
    gGui->onDataChanged();
    onPeaksChanged();
}

void Session::onExperimentChanged()
{
    if (currentProject()->experiment()->getDiffractometer()) {
        gGui->onExperimentChanged();
    }
    onDataChanged();
    onUnitCellChanged();
}

void Session::onPeaksChanged()
{
    gGui->onPeaksChanged();
}

void Session::onUnitCellChanged()
{
    gGui->onUnitCellChanged();
}

void Session::loadExperimentFromFile(QString filename)
{
    std::unique_ptr<Project> project_ptr {
        createProject(QString::fromStdString(nsx::kw_experimentDefaultName),
                      QString::fromStdString(nsx::kw_diffractometerDefaultName))};

    if (!project_ptr)
        return;

    try {
        project_ptr->experiment()->loadFromFile(filename.toStdString());
        project_ptr->generatePeakModels();
    } catch(const std::exception& ex) {
        const std::string msg {"Loading experiment from '" + filename.toStdString()
                               + "' failed with error: " + ex.what() + "."};
        throw std::runtime_error(msg);
        return;
    }

    addProject(std::move(project_ptr));
}
