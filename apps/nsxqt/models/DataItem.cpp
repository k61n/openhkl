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
#include "DialogPeakFind.h"
#include "ExperimentItem.h"
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
        data_ptr = nsx::DataReaderFactory().create(extension, filename.toStdString(), exp->getDiffractometer());
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

#if 0
NumorItem* DataItem::importRawData(const std::vector<std::string> &filenames,
                                   double wavelength, double delta_chi, double delta_omega, double delta_phi,
                                   bool rowMajor, bool swapEndian, int bpp)
{
    // Get the basename of the current numor
    QString filename(filenames[0].c_str());
    QFileInfo fileinfo(filename);
    std::string basename = fileinfo.fileName().toStdString();
    auto exp = experiment();

    // If the experience already stores the current numor, skip it
    if (exp->hasData(filenames[0]))
        return nullptr;


    std::shared_ptr<nsx::DataSet> data;
    std::shared_ptr<nsx::IDataReader> reader;

    try {
        auto diff = exp->getDiffractometer();
        reader = std::shared_ptr<nsx::IDataReader>(new nsx::RawDataReader(filenames, diff,
                                              wavelength, delta_chi, delta_omega, delta_phi,
                                              rowMajor, swapEndian, bpp));
        data = nsx::sptrDataSet(new nsx::DataSet(reader, diff));

    }
    catch(std::exception& e) {
        nsx::error() << "reading numor:" << filenames[0].c_str() << e.what();
        return nullptr;
    }
    catch(...)  {
        nsx::error() << "reading numor:" << filenames[0].c_str() << " reason not known:";
        return nullptr;
    }

    return importData(data);
}
#endif

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

    DialogPeakFind* dialog = new DialogPeakFind(selectedNumors, _peakFinder, nullptr);
    //dialog->setColorMap(_colormap);

    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogPeakFind> dialog_ptr(dialog);

    if (!dialog->exec()) {
        return;
    }

    //ui->_dview->getScene()->clearPeaks();

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
        integrator.integrate(peaks, numor, dialog->peakScale(), dialog->bkgBegin(), dialog->bkgEnd());
    }

    // delete the progressView
    delete progressView;

    //updatePeaks();
    nsx::debug() << "Peak search complete., found " << peaks.size() << " peaks.";

    auto& peak_item = *experimentItem().peaks().createPeaksItem("Found peaks");
    std::swap(peak_item.peaks(), peaks);
}

nsx::DataList DataItem::selectedData()
{
    nsx::DataList selectedNumors;
    for (int i = 0; i < rowCount(); ++i) {
        if (child(i)->checkState() == Qt::Checked) {
            if (auto ptr = dynamic_cast<NumorItem*>(child(i)))
                selectedNumors.push_back(ptr->getData());
        }
    }
    return selectedNumors;
}
