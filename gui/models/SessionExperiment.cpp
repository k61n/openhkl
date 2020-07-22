//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/SessionExperiment.cpp
//! @brief     Implements class SessionExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/SessionExperiment.h"

#include "core/data/DataSet.h"
#include "core/instrument/HardwareParameters.h"
#include "core/integration/GaussianIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/shape/PeakFilter.h"

#include "gui/MainWin.h"
#include "gui/dialogs/IntegrateDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/Session.h"

#include <QDateTime>
#include <QDebug>
#include <QStandardItem>
#include <QStringList>
#include <iostream>
#include <vector>

SessionExperiment::SessionExperiment()
    : _experiment{new nsx::Experiment{QDateTime::currentDateTime().toString().toStdString(),
                  *nsx::getResourcesName("instruments").begin()}}
{
}

SessionExperiment::SessionExperiment(QString name, QString instrument)
    : _experiment{new nsx::Experiment{name.toStdString(), instrument.toStdString()}}
{
}

QStringList SessionExperiment::getDataNames() const
{
    QStringList ret;
    for (auto data : _experiment->getDataMap())
        ret.append(QString::fromStdString(data.first));
    return ret;
}

// TODO: move logic to core
nsx::sptrDataSet SessionExperiment::getData(int index) const
{
    if (index == -1)
        index = dataIndex_;

    if (!_experiment->numData())
        return nullptr;

    std::string selected = getDataNames().at(index).toStdString();

    return _experiment->getData(selected);
}

// TODO: move logic to core
int SessionExperiment::getIndex(const QString& dataname) const
{
    QStringList liste = getDataNames();
    return liste.indexOf(dataname);
}

QList<nsx::sptrDataSet> SessionExperiment::allData() const
{
    QList<nsx::sptrDataSet> ret;
    for (auto data : _experiment->getDataMap())
        ret.append(data.second);
    return ret;
}

QStringList SessionExperiment::getPeakListNames() const
{
    QStringList ret;
    for (std::string name : _experiment->getCollectionNames())
        ret << QString::fromStdString(name);
    return ret;
}

QStringList SessionExperiment::getFoundNames() const
{
    QStringList ret;
    for (std::string name : _experiment->getFoundCollectionNames())
        ret << QString::fromStdString(name);
    return ret;
}

QStringList SessionExperiment::getPredictedNames() const
{
    QStringList ret;
    for (std::string name : _experiment->getPredictedCollectionNames())
        ret << QString::fromStdString(name);
    return ret;
}


void SessionExperiment::generatePeakModel(const QString& peakListName)
{
    if (!_experiment->hasPeakCollection(peakListName.toStdString()))
        return;

    nsx::PeakCollection* peak_collection =
        _experiment->getPeakCollection(peakListName.toStdString());

    PeakCollectionItem* peak_collection_item = new PeakCollectionItem(peak_collection);
    _peak_collection_items.push_back(peak_collection_item);

    PeakCollectionModel* peak_collection_model = new PeakCollectionModel;
    peak_collection_model->setRoot(peak_collection_item);
    _peak_collection_models.push_back(peak_collection_model);
}

void SessionExperiment::generatePeakModels()
{
    _peak_collection_models.clear();
    std::vector<std::string> names = _experiment->getCollectionNames();

    for (std::string name : names) {
        nsx::PeakCollection* peak_collection = _experiment->getPeakCollection(name);

        PeakCollectionItem* peak_collection_item = new PeakCollectionItem(peak_collection);
        _peak_collection_items.push_back(peak_collection_item);

        PeakCollectionModel* peak_collection_model = new PeakCollectionModel();
        peak_collection_model->setRoot(peak_collection_item);
        _peak_collection_models.push_back(peak_collection_model);
    }
}

void SessionExperiment::removePeakModel(const QString& name)
{
    std::string std_name = name.toStdString();

    nsx::PeakCollection* peak_collection = _experiment->getPeakCollection(std_name);

    if (!peak_collection)
        return;

    int item_index = 0;
    PeakCollectionItem* selected_item = nullptr;
    for (PeakCollectionItem* peak_collection_item : _peak_collection_items) {
        if (peak_collection_item->name() == std_name) {
            selected_item = peak_collection_item;
            break;
        }
        ++item_index;
    }

    if (!selected_item)
        return;

    int model_index = 0;
    PeakCollectionModel* selected_model = nullptr;
    for (PeakCollectionModel* peak_collection_model : _peak_collection_models) {
        if (peak_collection_model->root() == selected_item) {
            selected_model = peak_collection_model;
            break;
        }
        ++model_index;
    }

    if (!selected_model)
        return;

    _peak_collection_models.erase(_peak_collection_models.begin() + model_index);
    _peak_collection_items.erase(_peak_collection_items.begin() + item_index);

    delete selected_model;
    delete selected_item;
    _experiment->removePeakCollection(std_name);
}

PeakCollectionModel* SessionExperiment::peakModel(const QString& name) const
{
    std::string std_name = name.toStdString();
    for (int i = 0; i < _peak_collection_models.size(); ++i) {
        if (_peak_collection_models.at(i)->name() == std_name)
            return _peak_collection_models.at(i);
    }
    return nullptr;
}

PeakCollectionModel* SessionExperiment::peakModel(int i) const
{
    if (i >= _peak_collection_models.size())
        return nullptr;
    return _peak_collection_models.at(i);
}

std::vector<nsx::Peak3D*>
SessionExperiment::getPeaks(const QString& peakListName, int /*upperindex*/, int /*lowerindex*/) const
{
    if (!_experiment->hasPeakCollection(peakListName.toStdString()))
        return {};
    return _experiment->getPeakCollection(peakListName.toStdString())->getPeakList();
}

QStringList SessionExperiment::getUnitCellNames() const
{
    QStringList ret;
    for (std::string name : _experiment->getUnitCellNames())
        ret << QString::fromStdString(name);
    return ret;
}

void SessionExperiment::changeInstrument(const QString& instrumentname)
{
    if (_experiment->numData())
        return;
    _experiment = std::make_shared<nsx::Experiment>(
        _experiment->name(), instrumentname.toStdString());
}

void SessionExperiment::onPeaksChanged()
{
    gGui->onPeaksChanged();
}

void SessionExperiment::saveToFile(QString path)
{
    experiment()->saveToFile(path.toStdString());
    _save_path = path.toStdString();
    _saved = true;
}
