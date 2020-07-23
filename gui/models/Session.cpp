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
#include "core/loader/RawDataReader.h"
#include "core/raw/IDataReader.h"
#include "gui/MainWin.h"
#include "gui/dialogs/RawDataDialog.h"
#include "gui/models/Project.h"

Session* gSession;

Session::Session()
{
    gSession = this;
    _loadDirectory = QDir::homePath();
}

Project* Session::currentProject() { return _projects.at(_currentProject).get(); }
const Project* Session::currentProject() const { return _projects.at(_currentProject).get(); }

Project* Session::experimentAt(int i) { return _projects.at(i).get(); }
const Project* Session::experimentAt(int i) const { return _projects.at(i).get(); }

int Session::currentProjectNum() const { return _currentProject; }
int Session::numExperiments() const { return _projects.size(); }

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
    for (int i = 0; i < _projects.size(); i++)
        ret.push_back(QString::fromStdString(_projects.at(i)->experiment()->name()));
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

void Session::loadData()
{
    QStringList filenames = QFileDialog::getOpenFileNames(
        gGui, "import data", _loadDirectory,
        "Data files(*.h5 *.hdf5 *.hdf *.fake *.nxs *.raw *.tif *.tiff);;all files (*.* *)");

    if (filenames.empty())
        return;

    QFileInfo info(filenames.at(0));
    _loadDirectory = info.absolutePath();

    if (_currentProject < 0)
        createExperiment();

    for (QString filename : filenames) {
        QFileInfo fileinfo(filename);
        nsx::Experiment* exp = currentProject()->experiment();

        // If the experiment already stores the current numor, skip it
        if (exp->hasData(filename.toStdString()))
            return; // nullptr;

        nsx::sptrDataSet data_ptr;

        std::string extension = fileinfo.completeSuffix().toStdString();
        data_ptr = nsx::DataReaderFactory().create(
            extension, filename.toStdString(), exp->diffractometer());
        exp->addData(data_ptr);
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

    QStringList qfilenames = QFileDialog::getOpenFileNames();
    if (qfilenames.empty())
        return;

    QFileInfo info(qfilenames.at(0));
    _loadDirectory = info.absolutePath();

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
    try {
        nsx::Diffractometer* diff = exp->diffractometer();
        auto reader {std::make_unique<nsx::RawDataReader>(filenames[0], diff)};
        for (size_t i = 1; i < filenames.size(); ++i)
            reader->addFrame(filenames[i]);
        reader->setParameters(parameters);
        reader->end();
        auto data {std::make_shared<nsx::DataSet>(std::move(reader))};
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
    gGui->onExperimentChanged();
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
    createExperiment(QString::fromStdString("default"));
    currentProject()->experiment()->loadFromFile(filename.toStdString());
    currentProject()->generatePeakModels();
    onExperimentChanged();
}
