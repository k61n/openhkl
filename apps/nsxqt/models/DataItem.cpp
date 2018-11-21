#include <set>
#include <vector>

#include <QIcon>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QStandardItem>
#include <QString>

#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/PixelSumIntegrator.h>
#include <nsxlib/RawDataReader.h>

#include "DataItem.h"
#include "DialogHDF5Converter.h"
#include "FramePeakFinder.h"
#include "DialogRawData.h"
#include "FrameInstrumentStates.h"
#include "ExperimentItem.h"
#include "MainWindow.h"
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

    setDragEnabled(false);
    setDropEnabled(false);

    setEditable(false);

    setSelectable(false);

    setCheckable(false);
}

void DataItem::removeSelectedData()
{
    std::vector<NumorItem*> selected_numor_items;
    selected_numor_items.reserve(rowCount());

    for (int i = 0; i < rowCount(); ++i) {
        auto numor_item = dynamic_cast<NumorItem*>(child(i));
        if (numor_item) {
            if (numor_item->checkState() == Qt::Checked) {
                selected_numor_items.push_back(numor_item);
            }
        }
    }

    auto peaks_item = experimentItem()->peaksItem();
    auto all_peaks = peaks_item->allPeaks();

    std::set<nsx::sptrDataSet> used_data;

    for (auto peak : all_peaks) {
        auto data = peak->data();
        if (!data) {
            continue;
        }
        used_data.insert(data);
    }

    for (auto numor_item : selected_numor_items) {
        auto data = numor_item->data(Qt::UserRole).value<nsx::sptrDataSet>();
        auto it = used_data.find(data);
        if (it != used_data.end()) {
            nsx::info()<<"The numor "<<numor_item->text().toStdString()<<" is currently used. Can not be removed";
            continue;
        }
        removeRow(numor_item->row());
        experiment()->removeData(data->filename());
    }
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

        std::string extension = fileinfo.completeSuffix().toStdString();
        nsx::sptrDataSet data_ptr(new nsx::DataSet(extension, filename.toStdString(), exp->diffractometer()));
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

    std::shared_ptr<nsx::IDataReader> reader;

    nsx::RawDataReaderParameters parameters;

    parameters.wavelength = dialog.wavelength();
    parameters.delta_omega = dialog.deltaOmega();
    parameters.delta_chi = dialog.deltaChi();
    parameters.delta_phi = dialog.deltaPhi();
    parameters.row_major = dialog.rowMajor();
    parameters.swap_endian = dialog.swapEndian();
    parameters.bpp = dialog.bpp();

    auto diff = exp->diffractometer();
    std::shared_ptr<nsx::DataSet> data(new nsx::DataSet("raw",filenames[0],diff));

    try {
        auto raw_data_reader = dynamic_cast<nsx::RawDataReader*>(data->reader());
        for (size_t i = 1; i < filenames.size(); ++i) {
            raw_data_reader->addFrame(filenames[i]);
        }
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
    nsx::DataList data = selectedData();

    if (data.empty()) {
        nsx::error()<<"No numors selected for finding peaks";
        return;
    }

    auto experiment_item = experimentItem();

    FramePeakFinder* frame = FramePeakFinder::create(experiment_item, data);

    frame->show();
}

nsx::DataList DataItem::allData()
{
    nsx::DataList data;
    for (int i = 0; i < rowCount(); ++i) {
        if (auto numor_item = dynamic_cast<NumorItem*>(child(i))) {
            data.push_back(numor_item->data(Qt::UserRole).value<nsx::sptrDataSet>());
        }
    }
    return data;
}

nsx::DataList DataItem::selectedData()
{
    nsx::DataList selectedNumors;
    for (int i = 0; i < rowCount(); ++i) {
        if (child(i)->checkState() == Qt::Checked) {
            if (auto numor_item = dynamic_cast<NumorItem*>(child(i))) {
                selectedNumors.push_back(numor_item->data(Qt::UserRole).value<nsx::sptrDataSet>());
            }
        }
    }
    return selectedNumors;
}

void DataItem::convertToHDF5()
{
    nsx::DataList selected_data = selectedData();

    if (selected_data.empty()) {
        nsx::error()<<"No numors selected for HDF5 conversion";
        return;
    }

    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogHDF5Converter> dialog_ptr(new DialogHDF5Converter(selected_data));

    if (!dialog_ptr->exec()) {
        return;
    }
}

void DataItem::openInstrumentStatesDialog()
{
    nsx::DataList selected_data = selectedData();

    if (selected_data.empty()) {
        nsx::error()<<"No numors selected for exploring instrument states";
        return;
    }

    FrameInstrumentStates *frame = FrameInstrumentStates::create(selected_data);

    frame->show();

    frame->raise();
}
