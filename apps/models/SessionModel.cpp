//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SessionModel.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

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

#include "core/statistics/CC.h"
#include "core/experiment/CrystalTypes.h"
#include "core/loader/DataReaderFactory.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/geometry/Ellipsoid.h"
#include "core/geometry/GeometryTypes.h"
#include "core/instrument/InstrumentState.h"
#include "core/logger/Logger.h"
#include "core/peak/MergedData.h"
#include "core/peak/MergedPeak.h"
#include "core/crystal/MillerIndex.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakFilter.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/utils/ProgressHandler.h"
#include "core/geometry/ReciprocalVector.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"

#include "apps/models/DataItem.h"
#include "apps/models/DetectorItem.h"
#include "apps/models/DetectorScene.h"
#include "apps/dialogs/DialogExperiment.h"
#include "apps/models/ExperimentItem.h"
#include "apps/opengl/GLSphere.h"
#include "apps/opengl/GLWidget.h"
#include "apps/models/InstrumentItem.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/NumorItem.h"
#include "apps/models/PeakListItem.h"
#include "apps/views/PeakTableView.h"
#include "apps/models/PeaksItem.h"
#include "apps/views/ProgressView.h"
#include "QCustomPlot.h"
#include "apps/models/SampleItem.h"
#include "apps/models/SessionModel.h"
#include "apps/models/SourceItem.h"
#include "apps/models/TreeItem.h"
#include "apps/models/UnitCellItem.h"

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
