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

#include "core/shape/PeakFilter.h"
#include "core/data/DataSet.h"
#include "core/instrument/HardwareParameters.h"
#include "core/integration/GaussianIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"

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
{
    std::string experimentName = QDateTime::currentDateTime().toString().toStdString();
    std::set<std::string> instruments = nsx::getResourcesName("instruments");
    nsx::sptrExperiment expPtr(new nsx::Experiment(experimentName, *instruments.begin()));
    _experiment = expPtr;
}

SessionExperiment::SessionExperiment(QString name, QString instrument)
{
    nsx::sptrExperiment expPtr(new nsx::Experiment(name.toStdString(), instrument.toStdString()));
    _experiment = expPtr;
}

QStringList SessionExperiment::getDataNames()
{
    std::map<std::string, nsx::sptrDataSet> datamap = _experiment->data();
    QStringList names;
    for (auto data : datamap)
        names.append(QString::fromStdString(data.first));
    return names;
}

nsx::sptrDataSet SessionExperiment::getData(int index)
{
    if (index == -1)
        index = dataIndex_;

    if (_experiment->data().empty())
        return nullptr;

    std::string selected = getDataNames().at(index).toStdString();

    return _experiment->data().at(selected);
}

int SessionExperiment::getIndex(const QString& dataname)
{
    QStringList liste = getDataNames();
    return liste.indexOf(dataname);
}

QList<nsx::sptrDataSet> SessionExperiment::allData()
{
    std::map<std::string, nsx::sptrDataSet> map = _experiment->data();
    QList<nsx::sptrDataSet> list;
    for (auto data : map)
        list.append(data.second);
    return list;
}

QStringList SessionExperiment::getPeakListNames()
{
    std::vector<std::string> names = _experiment->getCollectionNames();
    QStringList q_names;

    for (std::string name : names) {
        q_names << QString::fromStdString(name);
    }
    return q_names;
}

QStringList SessionExperiment::getFoundNames()
{
    std::vector<std::string> names = _experiment->getFoundCollectionNames();
    QStringList q_names;

    for (std::string name : names) {
        q_names << QString::fromStdString(name);
    }
    return q_names;
}

QStringList SessionExperiment::getPredictedNames()
{
    std::vector<std::string> names = _experiment->getPredictedCollectionNames();
    QStringList q_names;

    for (std::string name : names) {
        q_names << QString::fromStdString(name);
    }
    return q_names;
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

PeakCollectionModel* SessionExperiment::peakModel(const QString& name)
{
    std::string std_name = name.toStdString();
    for (int i = 0; i < _peak_collection_models.size(); ++i) {
        if (_peak_collection_models.at(i)->name() == std_name)
            return _peak_collection_models.at(i);
    }
    return nullptr;
}

PeakCollectionModel* SessionExperiment::peakModel(int i)
{
    if (i >= _peak_collection_models.size())
        return nullptr;
    return _peak_collection_models.at(i);
}

std::vector<nsx::Peak3D*>
SessionExperiment::getPeaks(const QString& peakListName, int /*upperindex*/, int /*lowerindex*/)
{

    if (!_experiment->hasPeakCollection(peakListName.toStdString())) {
        std::vector<nsx::Peak3D*> peaks;
        return peaks;
    }

    nsx::PeakCollection* peakCollection =
        _experiment->getPeakCollection(peakListName.toStdString());

    std::vector<nsx::Peak3D*> peaks = peakCollection->getPeakList();

    return peaks;
}

QStringList SessionExperiment::getUnitCellNames()
{
    std::vector<std::string> names = _experiment->getUnitCellNames();
    QStringList q_names;

    for (std::string name : names) {
        q_names << QString::fromStdString(name);
    }
    return q_names;
}

void SessionExperiment::changeInstrument(const QString& instrumentname)
{
    if (!_experiment->data().empty())
        return;

    std::string expname = _experiment->name();
    _experiment = std::make_shared<nsx::Experiment>(expname, instrumentname.toStdString());
}

void SessionExperiment::onPeaksChanged()
{
    gGui->onPeaksChanged();
}

bool SessionExperiment::saveToFile(QString path)
{
    bool success = experiment()->saveToFile(path.toStdString());

    if (success) {
        _save_path = path.toStdString();
        _saved = true;
    }

    return success;
}
