#include <QIcon>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QStandardItem>
#include <QString>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/Logger.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/StrongPeakIntegrator.h>
#include <nsxlib/RawDataReader.h>

#include "DataItem.h"
#include "DialogHDF5Converter.h"
#include "DialogPeakFind.h"
#include "DialogRawData.h"
#include "ExperimentItem.h"
#include "MetaTypes.h"
#include "NumorItem.h"
#include "PeaksItem.h"
#include "PeakListItem.h"
#include "ProgressView.h"

DataItem::DataItem() : TreeItem()
{
    setText("Data");
    QIcon icon(":/resources/dataIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
}

void DataItem::importData()
{
    QStringList filenames;
    filenames = QFileDialog::getOpenFileNames(nullptr,"select numors","","",nullptr,QFileDialog::Option::DontUseNativeDialog);

    for (auto i = 0; i < filenames.size(); ++i) {
        auto&& filename = filenames[i];
        QFileInfo fileinfo(filename);
        auto exp = experiment();

        // If the experience already stores the current numor, skip it
        if (exp->hasData(filename.toStdString())) {
            return; // nullptr;
        }


        nsx::sptrDataSet data_ptr;

        std::string extension = fileinfo.completeSuffix().toStdString();
        data_ptr = nsx::DataReaderFactory().create(extension, filename.toStdString(), exp->diffractometer());
        exp->addData(data_ptr);      

        // Get the basename of the current numor
        auto&& basename = fileinfo.baseName();

        NumorItem* item = new NumorItem(data_ptr);
        item->setText(basename);
        item->setToolTip(filename);
        item->setCheckable(true);
        appendRow(item);
    }
}

void DataItem::importRawData()
{
    QStringList qfilenames;
    qfilenames = QFileDialog::getOpenFileNames(nullptr,"select raw data","","",nullptr,QFileDialog::Option::DontUseNativeDialog);

    if (qfilenames.empty()) {
        return;
    }

    std::vector<std::string> filenames;

    for (auto& filename: qfilenames) {
        filenames.push_back(filename.toStdString());
    }

    DialogRawData dialog;

    if (!dialog.exec()) {
        return;
    }
   
    // Get the basename of the current numor
    QFileInfo fileinfo(qfilenames[0]);
    std::string basename = fileinfo.fileName().toStdString();
    auto exp = experiment();

    // If the experience already stores the current numor, skip it
    if (exp->hasData(filenames[0])) {
        return;
    }

    std::shared_ptr<nsx::DataSet> data;
    std::shared_ptr<nsx::IDataReader> reader;

    auto wavelength = dialog.wavelength();
    auto delta_chi = dialog.deltaChi();
    auto delta_omega = dialog.deltaOmega();
    auto delta_phi = dialog.deltaPhi();
    auto rowMajor = dialog.rowMajor();
    auto swapEndian = dialog.swapEndian();
    auto bpp = dialog.bpp();

    try {
        auto diff = exp->diffractometer();
        reader.reset(new nsx::RawDataReader(filenames, diff, wavelength, delta_chi, delta_omega, delta_phi, rowMajor, swapEndian, bpp));
        data = std::make_shared<nsx::DataSet>(reader);
    }
    catch(std::exception& e) {
        nsx::error() << "reading numor:" << filenames[0].c_str() << e.what();
        return;
    }
    catch(...)  {
        nsx::error() << "reading numor:" << filenames[0].c_str() << " reason not known:";
        return;
    }

    // Get the basename of the current numor
    NumorItem* item = new NumorItem(data);
    item->setText(basename.c_str());
    item->setToolTip(qfilenames[0]);
    item->setCheckable(true);
    appendRow(item);
}

void DataItem::findPeaks()
{
    nsx::DataList selectedNumors = selectedData();

    if (selectedNumors.empty()) {
        nsx::error()<<"No numors selected for finding peaks";
        return;
    }

    // reset progress handler
    auto progressHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);

    // set up peak finder
    if ( !_peakFinder) {
        _peakFinder = nsx::sptrPeakFinder(new nsx::PeakFinder);
    }
    _peakFinder->setHandler(progressHandler);

    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogPeakFind> dialog_ptr(new DialogPeakFind(selectedNumors, _peakFinder, nullptr));

    if (!dialog_ptr->exec()) {
        return;
    }

    size_t max = selectedNumors.size();
    nsx::info() << "Peak find algorithm: Searching peaks in " << max << " files";

    // create a pop-up window that will show the progress
    ProgressView* progressView = new ProgressView(nullptr);
    progressView->watch(progressHandler);

    nsx::PeakList peaks;

    // execute in a try-block because the progress handler may throw if it is aborted by GUI
    try {
        peaks = _peakFinder->find(selectedNumors);
    }
    catch(std::exception& e) {
        nsx::debug() << "Caught exception during peak find: " << e.what();
        return;
    }

    // integrate peaks
    for (auto numor: selectedNumors) {
        nsx::StrongPeakIntegrator integrator(true, true);
        integrator.integrate(peaks, numor, dialog_ptr->peakScale(), dialog_ptr->bkgBegin(), dialog_ptr->bkgEnd());
    }

    // delete the progressView
    delete progressView;

    nsx::debug() << "Peak search complete., found " << peaks.size() << " peaks.";
    auto peaks_item = experimentItem()->peaksItem();
    model()->setData(peaks_item->index(),QVariant::fromValue(peaks),Qt::UserRole);
}

nsx::DataList DataItem::selectedData()
{
    nsx::DataList selectedNumors;
    for (int i = 0; i < rowCount(); ++i) {
        if (child(i)->checkState() == Qt::Checked) {
            if (auto ptr = dynamic_cast<NumorItem*>(child(i))) {
                selectedNumors.push_back(ptr->getData());
            }
        }
    }
    return selectedNumors;
}

void DataItem::convertToHDF5()
{
    nsx::DataList selectedNumors = selectedData();

    if (selectedNumors.empty()) {
        nsx::error()<<"No numors selected for HDF5 conversion";
        return;
    }

    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogHDF5Converter> dialog_ptr(new DialogHDF5Converter(selectedNumors));

    if (!dialog_ptr->exec()) {
        return;
    }
}
