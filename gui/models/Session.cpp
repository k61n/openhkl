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
#include "core/instrument/HardwareParameters.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/IDataReader.h"

#include "gui/MainWin.h"
#include "gui/dialogs/ExperimentDialog.h"
#include "gui/dialogs/RawDataDialog.h"
#include "gui/graphics/DetectorScene.h"

Session* gSession;

Session::Session()
{
    gSession = this;
    loadDirectory = QDir::homePath();
}

bool Session::createExperiment(QString experimentName, QString instrumentName)
{
    QList<QString> temp = experimentNames();
    QList<QString>::iterator it;
    for (it = temp.begin(); it != temp.end(); ++it) {
        if (*it == experimentName)
            return false;
    }

    SessionExperiment* experiment = new SessionExperiment(experimentName, instrumentName);
    _experiments.append(experiment);
    selectedExperiment_ = _experiments.size() - 1;
    onExperimentChanged();

    return true;
}

bool Session::createExperiment(QString experimentName)
{

    QList<QString> temp = experimentNames();
    QList<QString>::iterator it;
    for (it = temp.begin(); it != temp.end(); ++it) {
        if (*it == experimentName)
            return false;
    }

    SessionExperiment* experiment = new SessionExperiment;
    experiment->experiment()->setName(experimentName.toStdString());
    _experiments.append(experiment);
    selectedExperiment_ = _experiments.size() - 1;
    onExperimentChanged();

    return true;
}

void Session::createDefaultExperiment()
{
    SessionExperiment* experiment = new SessionExperiment("lol", "BioDiff2500");
    _experiments.append(experiment);
    selectedExperiment_ = _experiments.size() - 1;
    onExperimentChanged();
}

QList<QString> Session::experimentNames() const
{
    QList<QString> names;

    for (int i = 0; i < _experiments.size(); i++)
        names.append(QString::fromStdString(_experiments.at(i)->experiment()->name()));
    return names;
}

void Session::removeExperiment()
{
    if (_experiments.size() == 0)
        return;
    if (selectedExperiment_ == -1)
        _experiments.removeFirst();

    selectedExperiment_ = _experiments.size() > 0 ? 0 : -1;
    onExperimentChanged();
}

void Session::selectExperiment(int select)
{
    if (select < _experiments.size() && select >= 0)
        selectedExperiment_ = select;
    onExperimentChanged();
}

SessionExperiment* Session::selectedExperiment()
{
    return _experiments.at(selectedExperiment_);
}

void Session::loadData()
{
    QStringList filenames = QFileDialog::getOpenFileNames(
        gGui, "import data", loadDirectory,
        "Data files(*.h5 *.hdf5 *.hdf *.fake *.nxs *.raw *.tif *.tiff);;all files (*.* *)");

    if (filenames.empty())
        return;

    QFileInfo info(filenames.at(0));
    loadDirectory = info.absolutePath();

    if (selectedExperiment_ < 0)
        createDefaultExperiment();

    for (QString filename : filenames) {
        QFileInfo fileinfo(filename);
        nsx::sptrExperiment exp = selectedExperiment()->experiment();

        // If the experiment already stores the current numor, skip it
        if (exp->hasData(filename.toStdString()))
            return; // nullptr;

        nsx::sptrDataSet data_ptr;

        std::string extension = fileinfo.completeSuffix().toStdString();
        data_ptr = nsx::DataReaderFactory().create(
            extension, filename.toStdString(), exp->diffractometer());
        exp->addData(data_ptr);
    }
    selectedExperiment()->selectData(selectedExperiment()->getIndex(filenames.at(0)));
    onDataChanged();
}

void Session::removeData()
{
    if (selectedExperiment_ == -1)
        return;

    if (selectedData == -1)
        return;
    std::string numorname = selectedExperiment()->getData(selectedData)->filename();
    selectedExperiment()->experiment()->removeData(numorname);
    onDataChanged();
}

void Session::loadRawData()
{
    if (selectedExperiment_ < 0)
        createDefaultExperiment();

    QStringList qfilenames = QFileDialog::getOpenFileNames();

    if (qfilenames.empty())
        return;

    QFileInfo info(qfilenames.at(0));
    loadDirectory = info.absolutePath();

    std::vector<std::string> filenames;

    for (QString filename : qfilenames)
        filenames.push_back(filename.toStdString());

    RawDataDialog dialog;

    if (!dialog.exec())
        return;
    nsx::sptrExperiment exp = selectedExperiment()->experiment();

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
    // selectedData = selectedExperiment()->getIndex(qfilenames.at(0));
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
    selectedExperiment()->experiment()->loadFromFile(filename.toStdString());
    selectedExperiment()->generatePeakModels();
    onExperimentChanged();
}
