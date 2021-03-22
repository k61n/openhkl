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
#include "gui/models/Project.h"

#include <QCollator>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

Session* gSession;

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

std::vector<QString> Session::experimentNames() const
{
    std::vector<QString> ret;
    for (const auto& project : _projects)
        ret.push_back(QString::fromStdString(project->experiment()->name()));
    return ret;
}

void Session::removeExperiment()
{
    std::cerr << "TODO: implement Session::removeExperiment\n";
    /*
        if (_projects.size() == 0)
            return;
        if (_currentProject == -1)
            _projects.removeFirst();

        _currentProject = _projects.size() > 0 ? 0 : -1;
    */
    onExperimentChanged();
}

void Session::selectExperiment(int select)
{
    if (select < _projects.size() && select >= 0)
        _currentProject = select;
    onExperimentChanged();
}

void Session::loadData(nsx::DataFormat format)
{
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

    if (_currentProject < 0)
        createExperiment();

    for (QString filename : filenames) {
        QFileInfo fileinfo(filename);
        nsx::Experiment* exp = currentProject()->experiment();

        // If the experiment already stores the current numor, skip it
        if (exp->hasData(filename.toStdString()))
            return; // nullptr;

        try {
            nsx::sptrDataSet data_ptr;

            std::string extension = fileinfo.completeSuffix().toStdString();
            data_ptr = nsx::DataReaderFactory().create(
                extension, filename.toStdString(), exp->getDiffractometer());
            exp->addData(data_ptr);
        } catch (const std::exception& ex) {
            QString msg = QString("Loading file \"") + filename + QString("\" failed with error: ")
                + QString(ex.what()) + QString(".");

            QMessageBox::critical(nullptr, "Error", msg);
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
    if (_currentProject < 0)
        createExperiment();

    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("data_raw", QDir::homePath()).toString();

    QStringList qfilenames = QFileDialog::getOpenFileNames(gGui, "import raw data", loadDirectory);
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
    for (QString filename : qfilenames)
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
    double eps = 1e-8;
    try {
        nsx::Diffractometer* diff = exp->getDiffractometer();
        auto reader{std::make_unique<nsx::RawDataReader>(filenames[0], diff)};
        reader->setParameters(parameters);
        for (size_t i = 0; i < filenames.size(); ++i)
            reader->addFrame(filenames[i]);
        reader->end();
        if (parameters.wavelength < eps)
            throw std::runtime_error("Wavelength not set");
        auto data{std::make_shared<nsx::DataSet>(std::move(reader))};
        exp->addData(data);
    } catch (std::exception& e) {
        return;
    } catch (...) {
        return;
    }
    // _selectedData = currentProject()->getIndex(qfilenames.at(0));
    onDataChanged();
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
