#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>


#include <hdf5.h>

#include <QAbstractItemView>
#include <QDate>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>

#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>
#include <QVector>

#include <core/CC.h>
#include <core/CrystalTypes.h>
#include <core/DataReaderFactory.h>
#include <core/Detector.h>
#include <core/Diffractometer.h>
#include <core/Ellipsoid.h>
#include <core/GeometryTypes.h>
#include <core/InstrumentState.h>
#include <core/Logger.h>
#include <core/MergedData.h>
#include <core/MergedPeak.h>
#include <core/MillerIndex.h>
#include <core/Peak3D.h>
#include <core/PeakFilter.h>
#include <core/PeakFinder.h>
#include <core/PixelSumIntegrator.h>
#include <core/ProgressHandler.h>
#include <core/ReciprocalVector.h>
#include <core/Sample.h>
#include <core/Source.h>
#include <core/SpaceGroup.h>
#include <core/UnitCell.h>

#include "DataItem.h"
#include "DetectorItem.h"
#include "DetectorScene.h"
#include "DialogExperiment.h"
#include "ExperimentItem.h"
#include "GLSphere.h"
#include "GLWidget.h"
#include "InstrumentItem.h"
#include "MetaTypes.h"
#include "NumorItem.h"
#include "PeakListItem.h"
#include "PeakTableView.h"
#include "PeaksItem.h"
#include "ProgressView.h"
#include "QCustomPlot.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "TreeItem.h"
#include "UnitCellItem.h"

#include "ui_MainWindow.h"

SessionModel::SessionModel()
{
    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));
}

SessionModel::~SessionModel() {}

ExperimentItem* SessionModel::selectExperiment(nsx::sptrDataSet data)
{
    ExperimentItem* experiment_item = nullptr;

    for (auto i = 0; i < rowCount(); ++i) {
        experiment_item = dynamic_cast<ExperimentItem*>(item(i));
        if (!experiment_item) {
            continue;
        }

        auto data_item = experiment_item->dataItem();
        for (auto j = 0; j < data_item->rowCount(); ++j) {
            auto numor_item = dynamic_cast<NumorItem*>(data_item->child(j));
            if (!numor_item) {
                continue;
            }

            if (numor_item->data(Qt::UserRole).value<nsx::sptrDataSet>() == data) {
                return experiment_item;
            }
        }
    }

    return experiment_item;
}

void SessionModel::selectData(nsx::sptrDataSet data)
{
    emit signalSelectedDataChanged(data, 0);
}

void SessionModel::onItemChanged(QStandardItem* item)
{
    Q_UNUSED(item)

    emit updatePeaks();
}

nsx::PeakList SessionModel::peaks(nsx::sptrDataSet data) const
{
    nsx::PeakList list;

    for (auto i = 0; i < rowCount(); ++i) {
        auto exp_item = dynamic_cast<ExperimentItem*>(item(i));
        auto&& peaks = exp_item->peaksItem()->selectedPeaks();

        for (auto peak : peaks) {
            if (data == nullptr || peak->data() == data) {
                list.push_back(peak);
            }
        }
    }
    return list;
}

void SessionModel::createNewExperiment()
{
    std::unique_ptr<DialogExperiment> dlg;

    // DialogExperiment could throw an exception if it fails to read the resource
    // files
    try {
        dlg = std::unique_ptr<DialogExperiment>(new DialogExperiment());

        // The user pressed cancel, return
        if (!dlg->exec()) {
            return;
        }

        // If no experiment name is provided, pop up a warning
        if (dlg->getExperimentName().isEmpty()) {
            throw std::runtime_error("Empty experiment name");
        }
    } catch (std::exception& e) {
        nsx::error() << e.what();
        return;
    }

    try {
        auto experimentName = dlg->getExperimentName().toStdString();
        auto instrumentName = dlg->getInstrumentName().toStdString();

        // Create an experiment
        nsx::sptrExperiment expPtr(new nsx::Experiment(experimentName, instrumentName));

        // Create an experiment item out of the experiment
        ExperimentItem* expt = new ExperimentItem(expPtr);
        appendRow(expt);
    } catch (const std::runtime_error& e) {
        nsx::error() << e.what();
        return;
    }
}
