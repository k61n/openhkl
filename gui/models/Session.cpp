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
#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/IDataReader.h"
#include "gui/MainWin.h"
#include "gui/dialogs/RawDataDialog.h"
#include "gui/dialogs/DataNameDialog.h"
#include "gui/models/Project.h"
#include "base/utils/StringIO.h" // using join
#include "core/raw/MetaData.h"

#include <QCollator>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

Session* gSession;

namespace {

// open a dialog to choose a name for a dataset
std::string askDataName(const std::string dataname0) {
    // std::string dataname = dataset->filename();  // default name: name of the first data-file
    DataNameDialog dataname_dialog(dataname0);
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

bool Session::createExperiment(QString experimentName, QString instrumentName)
{
    for (const QString& name : experimentNames())
        if (name == experimentName)
            return false;

    auto experiment = std::make_unique<Project>(experimentName, instrumentName);
    _projects.push_back(std::move(experiment));
    _currentProject = _projects.size() - 1;
    onExperimentChanged();

    return true;
}

void Session::removeExperiment(const QString& name)
{
    if (_projects.size() == 0) {
        return;
    } else {
        for (int project_idx = 0; project_idx < _projects.size(); ++project_idx) {
            if (name.toStdString() == _projects[project_idx]->experiment()->name()) {
                _projects.erase(_projects.begin() + project_idx);
            }
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
        QMessageBox::critical(nullptr, "Error",
                              "Please create an experiment before loading data.");
        return;
    }

    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("data", QDir::homePath()).toString();

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
    s.setValue("data", loadDirectory);

    for (const QString& filename : filenames) {
        QFileInfo fileinfo(filename);
        nsx::Experiment* exp = currentProject()->experiment();

        // If the experiment already stores the current numor, skip it
        if (exp->hasData(filename.toStdString()))
            return;

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
            // default data name: name of the first data-file
            const std::string dataname {askDataName(dataset_ptr->filename())};
            // add the list of sources as metadata
            dataset_ptr->metadata().add<std::string>("sources", filename.toStdString());
            exp->addData(dataset_ptr, dataname);
        } catch (const std::exception& ex) {
            QString msg = QString("Loading file(s) '") + filename + QString("' failed with error: ")
                + QString(ex.what()) + QString(".");

            QMessageBox::critical(nullptr, "Error", msg);
            return;
        }
    }
    currentProject()->selectData(currentProject()->getIndex(filenames.at(0)));
    onDataChanged();
}

void Session::removeData()
{
    if (_currentProject == -1)
        return;
    if (_selectedData == -1)
        return;

    std::string numorname = currentProject()->getData(_selectedData)->filename();
    currentProject()->experiment()->removeData(numorname);
    onDataChanged();
}

void Session::loadRawData()
{
    // Loading data requires an existing Experiment
    if (_currentProject < 0) {
        QMessageBox::critical(nullptr, "Error",
                              "Please create an experiment before loading data.");
        return;
    }

    try {
        QSettings s;
        s.beginGroup("RecentDirectories");
        QString loadDirectory = s.value("data_raw", QDir::homePath()).toString();

        QStringList qfilenames =
            QFileDialog::getOpenFileNames(gGui, "import raw data", loadDirectory);
        if (qfilenames.empty())
            return;

        // Don't leave sorting the files to the OS. Use QCollator + std::sortto sort naturally
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
        s.setValue("data_raw", loadDirectory);

        std::vector<std::string> filenames;
        for (const QString& filename : qfilenames)
            filenames.push_back(filename.toStdString());

        RawDataDialog dialog;
        if (!dialog.exec())
            return;
        nsx::Experiment* exp = currentProject()->experiment();

        // If the experience already stores the current numor, skip it
        if (exp->hasData(filenames[0]))
            return;

        nsx::RawDataReaderParameters parameters;
        parameters.wavelength = dialog.wavelength();
        parameters.delta_omega = dialog.deltaOmega();
        parameters.delta_chi = dialog.deltaChi();
        parameters.delta_phi = dialog.deltaPhi();
        parameters.row_major = dialog.rowMajor();
        parameters.swap_endian = dialog.swapEndian();
        parameters.bpp = dialog.bpp();
        nsx::Diffractometer* diff = exp->getDiffractometer();
        auto reader{std::make_unique<nsx::RawDataReader>(filenames[0], diff)};
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

        const double eps = 1e-8;
        if (parameters.wavelength < eps)
            throw std::runtime_error("Wavelength not set");

        const std::shared_ptr<nsx::DataSet> dataset{std::make_shared<nsx::DataSet>(std::move(reader))};

        // choose a name for the dataset
        // default data name: name of the first data-file
        const std::string dataname {askDataName(dataset->filename())};
        dataset->setName(dataname);
        dataset->sources = filenames;
        metadata.add("sources", nsx::join(filenames, ", "));
        dataset->metadata().setMap(metadata.map());

        exp->addData(dataset, dataname);
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
    createExperiment("default");
    currentProject()->experiment()->loadFromFile(filename.toStdString());
    currentProject()->generatePeakModels();
    onExperimentChanged();
}
