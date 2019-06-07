//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/session.cpp
//! @brief     Implements class Session
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/models/session.h"
#include "gui/graphics/detectorscene.h"

#include "core/experiment/DataSet.h"
#include "core/algo/DataReaderFactory.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"

#include "gui/dialogs/experimentdialog.h"
#include "gui/mainwin.h"
#include <QCR/engine/logger.h>
#include <QCR/engine/mixin.h>
#include <QCR/widgets/modal_dialogs.h>

Session* gSession;

Session::Session()
{
    gSession = this;
}

void Session::createExperiment()
{
    std::unique_ptr<ExperimentDialog> dlg;

    // DialogExperiment could throw an exception if it fails to read the resource files
    try {
        dlg = std::unique_ptr<ExperimentDialog>(new ExperimentDialog());
        if (!dlg->exec())
            return;
        if (dlg->experimentName().isEmpty()) {
            gLogger->log("[WARNING] Failed adding new experiment due to empty experiment name");
            return;
        }
    } catch (std::exception& e) {
        gLogger->log(QString::fromStdString(e.what()));
        return;
    }

    try {
        auto experimentName = dlg->experimentName().toStdString();
        auto instrumentName = dlg->instrumentName().toStdString();
        nsx::sptrExperiment expPtr(new nsx::Experiment(experimentName, instrumentName));

        ExperimentModel* expt = new ExperimentModel(expPtr);
        experiments.push_back(expt);
        selected = experiments.size() - 1;
        gLogger->log("Experiment \"" + QString::fromStdString(experimentName) + "\" added");
    } catch (const std::runtime_error& e) {
        gLogger->log(QString::fromStdString(e.what()));
        return;
    }
    onExperimentChanged();
}

void Session::removeExperiment()
{
    if (experiments.size() == 0) {
        gLogger->log("[WARNING] nothing to remove");
        return;
    }
    if (selected == -1) {
        gLogger->log(
            "removing experiment \""
            + QString::fromStdString(experiments.at(0)->experiment()->name()) + "\"");
        experiments.removeFirst();
    }
    gLogger->log(
        "removing experiment \""
        + QString::fromStdString(experiments.at(selected)->experiment()->name()) + "\"");
    experiments.removeAt(selected);
    selected = experiments.size() > 0 ? 0 : -1;
    onExperimentChanged();
}

void Session::selectExperiment(int select)
{
    if (select < experiments.size() && select >= 0)
        selected = select;
    onExperimentChanged();
}

ExperimentModel* Session::selectedExperiment()
{
    return experiments.at(selected);
}

void Session::loadData()
{
    QStringList filenames;
    filenames = QcrFileDialog::getOpenFileNames(
        gGui, "import data", QDir::homePath(),
        "Data files(*.h5 *.hdf5 *.hdf *.fake *.nxs *.raw *.tif *.tiff);;all files (*.* *)");
    for (QString filename : filenames) {
        QFileInfo fileinfo(filename);
        auto exp = selectedExperiment()->experiment();

        // If the experiment already stores the current numor, skip it
        if (exp->hasData(filename.toStdString()))
            return; // nullptr;

        nsx::sptrDataSet data_ptr;

        std::string extension = fileinfo.completeSuffix().toStdString();
        data_ptr = nsx::DataReaderFactory().create(
            extension, filename.toStdString(), exp->diffractometer());
        exp->addData(data_ptr);
        selectedExperiment()->addData(data_ptr);
    }

    onDataChanged();
}

void Session::removeData()
{
    if (selected == -1) {
        gLogger->log("[ERROR] No experiment to remove data from");
        return;
    }

    if (!selectedExperiment()->data()->selectedData()) {
        gLogger->log("[ERROR] No data to remove");
        return;
    }
    std::string numorname = selectedExperiment()->data()->selectedData()->filename();
    selectedExperiment()->experiment()->removeData(numorname);
    selectedExperiment()->data()->removeSelectedData();
}

void Session::loadRawData()
{
    //    QStringList qfilenames;
    //    qfilenames = QcrFileDialog::getOpenFileNames(
    //        nullptr, "select raw data", "", "", nullptr,
    //        QFileDialog::Option::DontUseNativeDialog);

    //    if (qfilenames.empty()) {
    //        return;
    //    }

    //    std::vector<std::string> filenames;

    //    for (auto& filename : qfilenames) {
    //        filenames.push_back(filename.toStdString());
    //    }

    //    DialogRawData dialog;

    //    if (!dialog.exec()) {
    //        return;
    //    }
    //    auto exp = selectedExperiment()->experiment();

    //    // If the experience already stores the current numor, skip it
    //    if (exp->hasData(filenames[0])) {
    //        return;
    //    }

    //    std::shared_ptr<nsx::DataSet> data;
    //    std::shared_ptr<nsx::IDataReader> reader;

    //    nsx::RawDataReaderParameters parameters;

    //    parameters.wavelength = dialog.wavelength();
    //    parameters.delta_omega = dialog.deltaOmega();
    //    parameters.delta_chi = dialog.deltaChi();
    //    parameters.delta_phi = dialog.deltaPhi();
    //    parameters.row_major = dialog.rowMajor();
    //    parameters.swap_endian = dialog.swapEndian();
    //    parameters.bpp = dialog.bpp();

    //    try {
    //        auto diff = exp->diffractometer();
    //        reader.reset(new nsx::RawDataReader(filenames[0], diff));
    //        auto raw_data_reader = std::dynamic_pointer_cast<nsx::RawDataReader>(reader);
    //        for (size_t i = 1; i < filenames.size(); ++i) {
    //            raw_data_reader->addFrame(filenames[i]);
    //        }
    //        data = std::make_shared<nsx::DataSet>(reader);
    //    } catch (std::exception& e) {
    //        gLogger->log(
    //            "reading numor " + qfilenames.at(0) + " failed: " +
    //            QString::fromStdString(e.what()));
    //        return;
    //    } catch (...) {
    //        gLogger->log("reading numor " + qfilenames.at(0) + " failed, reason not known");
    //        return;
    //    }

    //    exp->addData(data);
    //    selectedExperiment()->addData(data);
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
