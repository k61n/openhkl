/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include <hdf5.h>
#include <H5Exception.h>

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
#include <core/ProgressHandler.h>
#include <core/ReciprocalVector.h>
#include <core/Sample.h>
#include <core/SpaceGroup.h>
#include <core/PixelSumIntegrator.h>
#include <core/Source.h>
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
#include "ProgressView.h"
#include "QCustomPlot.h"
#include "PeaksItem.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "TreeItem.h"
#include "UnitCellItem.h"

#include "ui_MainWindow.h"

SessionModel::SessionModel()
{
    connect(this,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(onItemChanged(QStandardItem*)));
}

SessionModel::~SessionModel()
{
}

ExperimentItem* SessionModel::selectExperiment(nsx::sptrDataSet data)
{
    ExperimentItem *experiment_item=nullptr;

    for (auto i = 0; i < rowCount(); ++i) {
        experiment_item = dynamic_cast<ExperimentItem*>(item(i));
        if (!experiment_item) {
            continue;
        }

        auto data_item  = experiment_item->dataItem();
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
    emit signalSelectedDataChanged(data,0);
}

void SessionModel::onItemChanged(QStandardItem *item)
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

        for (auto peak: peaks) {
            if (data == nullptr || peak->data() == data) {
                list.push_back(peak);
            }
        }
    }
    return list;
}


void SessionModel::addExperiment(nsx::sptrExperiment experiment)
{
    // Create an experiment item out of the experiment
    ExperimentItem* expt = new ExperimentItem(experiment);
    appendRow(expt);
}
