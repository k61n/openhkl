#include <set>
#include <vector>

#include <QIcon>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QStandardItem>
#include <QString>

#include <core/DataSet.h>
#include <core/Experiment.h>
#include <core/IDataReader.h>
#include <core/Logger.h>
#include <core/Peak3D.h>
#include <core/ProgressHandler.h>
#include <core/PixelSumIntegrator.h>
#include <core/RawDataReader.h>

#include "DataItem.h"
#include "DialogHDF5Converter.h"
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
    std::set<NumorItem*> _selected_datasets_for_removal;
    bool accept_removal(true);

    for (int i = 0; i < rowCount(); ++i) {
        auto numor_item = dynamic_cast<NumorItem*>(child(i));
        if (numor_item) {
            if (numor_item->checkState() == Qt::Checked) {
                auto dataset = numor_item->data(Qt::UserRole).value<nsx::sptrDataSet>();
                auto use_count = dataset.use_count();
                // If the dataset is not used the use count should be 3
                // (1 in experiment, 1 in NumorItem and 1 in dataset local variable)
                if (use_count > 3) {
                    nsx::error()<<"The dataset "<<dataset->reader()->basename()
                                <<" is currently used by "<< std::to_string(use_count - 3)
                                << "other resources.";
                    accept_removal = false;
                } else {
                    _selected_datasets_for_removal.insert(numor_item);
                }
            }
        }
    }

    if (!accept_removal) {
        nsx::error()<<"One or more datasets are in use. Data removal aborted.";
        return;
    }

    for (auto* dataset_item : _selected_datasets_for_removal) {
        auto data = dataset_item->data(Qt::UserRole).value<nsx::sptrDataSet>();
        removeRow(dataset_item->row());
        experiment()->removeData(data->filename());
    }
}

void DataItem::importData()
{
    QStringList filenames;
    filenames = QFileDialog::getOpenFileNames(
        nullptr,"select numors","","",nullptr,QFileDialog::Option::DontUseNativeDialog);

    for (auto i = 0; i < filenames.size(); ++i) {
        auto&& filename = filenames[i];
        QFileInfo fileinfo(filename);
        auto exp = experiment();

        // If the experience already stores the current numor, skip it
        if (exp->hasData(filename.toStdString()))
            return; // nullptr;

        std::string extension = fileinfo.completeSuffix().toStdString();
        nsx::sptrDataSet data_ptr(
            new nsx::DataSet(extension, filename.toStdString(), exp->diffractometer()));
        exp->addData(data_ptr);

        // Get the basename of the current numor
        auto&& basename = fileinfo.baseName();

        NumorItem* item = new NumorItem(data_ptr);
        item->setText(basename);
        item->setToolTip(filename);
        appendRow(item);
    }
}

void DataItem::importRawData()
{
    QStringList qfilenames;
    qfilenames = QFileDialog::getOpenFileNames(
        nullptr,"select raw data","","",nullptr,QFileDialog::Option::DontUseNativeDialog);

    if (qfilenames.empty())
        return;

    std::vector<std::string> filenames;

    for (auto& filename: qfilenames)
        filenames.push_back(filename.toStdString());

    DialogRawData dialog;

    if (!dialog.exec())
        return;

    // Get the basename of the current numor
    QFileInfo fileinfo(qfilenames[0]);
    std::string basename = fileinfo.fileName().toStdString();
    auto exp = experiment();

    // If the experience already stores the current numor, skip it
    if (exp->hasData(filenames[0]))
        return;

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
        for (size_t i = 1; i < filenames.size(); ++i)
            raw_data_reader->addFrame(filenames[i]);
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
    appendRow(item);
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

    if (!dialog_ptr->exec())
        return;
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
