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
#include "core/experiment/DataSet.h"
#include "core/instrument/HardwareParameters.h"
#include "core/loader/RawDataReader.h"
#include "core/raw/IDataReader.h"
#include "gui/MainWin.h"
#include "gui/dialogs/ExperimentDialog.h"
#include "gui/dialogs/RawDataDialog.h"
#include "gui/graphics/DetectorScene.h"
#include <QCR/engine/logger.h>
#include <QCR/engine/mixin.h>
#include <QCR/widgets/modal_dialogs.h>

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
    for (it = temp.begin(); it != temp.end(); ++it){
        if (*it == experimentName)
            return false;
    }

    SessionExperiment* experiment = new SessionExperiment(experimentName, instrumentName);
    _experiments.append(experiment);
    selectedExperiment_ = _experiments.size() - 1;
    gLogger->log("Experiment \"" + experimentName + "\" added");
    onExperimentChanged();

    return true;
}

bool Session::createExperiment(QString experimentName)
{

    QList<QString> temp = experimentNames();
    QList<QString>::iterator it;
    for (it = temp.begin(); it != temp.end(); ++it){
        if (*it == experimentName)
            return false;
    }

    SessionExperiment* experiment = new SessionExperiment;
    experiment->experiment()->setName(experimentName.toStdString());
    _experiments.append(experiment);
    selectedExperiment_ = _experiments.size() - 1;
    gLogger->log("Experiment \"" + experimentName + "\" added");
    onExperimentChanged();

    return true;
}

void Session::createDefaultExperiment()
{
    _experiments.push_back(new SessionExperiment);
    selectedExperiment_ = _experiments.size() - 1;
    onExperimentChanged();
}

QList<QString> Session::experimentNames() const
{
    QList<QString> names;

    for (int i = 0 ; i < _experiments.size(); i++) {
        names.append(QString::fromStdString(_experiments.at(i)->experiment()->name()));
    }
    return names;
}

void Session::removeExperiment()
{
    if (_experiments.size() == 0) {
        gLogger->log("[WARNING] nothing to remove");
        return;
    }
    if (selectedExperiment_ == -1) {
        gLogger->log(
            "removing experiment \""
            + QString::fromStdString(_experiments.at(0)->experiment()->name()) + "\"");
        _experiments.removeFirst();
    }
    gLogger->log(
        "removing experiment \""
        + QString::fromStdString(_experiments.at(selectedExperiment_)->experiment()->name()) + "\"");
    _experiments.removeAt(selectedExperiment_);
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
    QStringList filenames = QcrFileDialog::getOpenFileNames(
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
    if (selectedExperiment_ == -1) {
        gLogger->log("[ERROR] No experiment to remove data from");
        return;
    }

    if (selectedData == -1) {
        gLogger->log("[ERROR] No data to remove");
        return;
    }
    std::string numorname = selectedExperiment()->getData(selectedData)->filename();
    selectedExperiment()->experiment()->removeData(numorname);
    onDataChanged();
}

void Session::loadRawData()
{
    if (selectedExperiment_ < 0)
        createDefaultExperiment();

    QStringList qfilenames;
    qfilenames = QcrFileDialog::getOpenFileNames(
        nullptr, "select raw data", loadDirectory, "", nullptr,
        QFileDialog::Option::DontUseNativeDialog);

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

    std::shared_ptr<nsx::DataSet> data;
    std::shared_ptr<nsx::IDataReader> reader;

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
        reader.reset(new nsx::RawDataReader(filenames[0], diff));
        std::shared_ptr<nsx::RawDataReader> raw_data_reader =
            std::dynamic_pointer_cast<nsx::RawDataReader>(reader);
        for (size_t i = 1; i < filenames.size(); ++i)
            raw_data_reader->addFrame(filenames[i]);
        raw_data_reader->setParameters(parameters);
        raw_data_reader->end();
        data = std::make_shared<nsx::DataSet>(reader);
    } catch (std::exception& e) {
        gLogger->log(
            "reading numor " + qfilenames.at(0) + " failed: " + QString::fromStdString(e.what()));
        return;
    } catch (...) {
        gLogger->log("reading numor " + qfilenames.at(0) + " failed, reason not known");
        return;
    }

    exp->addData(data);
    selectedData = selectedExperiment()->getIndex(qfilenames.at(0));
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
}

void Session::onPeaksChanged()
{
    gGui->onPeaksChanged();
}

bool Session::loadExperimentFromFile(QString filename)
{
    bool success = createExperiment(QString::fromStdString("default"));

    if (success){
        success = selectedExperiment()->experiment()->loadFromFile(filename.toStdString());
    }
    if (success){
        selectedExperiment()->generatePeakModels();
        onExperimentChanged();
    }
    return success;

}

