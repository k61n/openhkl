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

#include "core/analyse/PeakFilter.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/HardwareParameters.h"
#include "core/integration/GaussianIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"

#include "gui/dialogs/IntegrateDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/models/Session.h"
#include "gui/MainWin.h"
#include "gui/items/PeakCollectionItem.h"

#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <QStandardItem>

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

QStringList SessionExperiment::getPeakListNames(int depth)
{
    std::vector<std::string> names = _experiment->getCollectionNames();
    QStringList q_names;

    for (std::string name :names){
        q_names<<QString::fromStdString(name);
    }
    return q_names;
}

QStringList SessionExperiment::getFoundNames(int depth)
{
    std::vector<std::string> names = _experiment->getFoundCollectionNames();
    QStringList q_names;

    for (std::string name :names){
        q_names<<QString::fromStdString(name);
    }
    return q_names;
}

QStringList SessionExperiment::getPredictedNames(int depth)
{
    std::vector<std::string> names = _experiment->getPredictedCollectionNames();
    QStringList q_names;

    for (std::string name :names){
        q_names<<QString::fromStdString(name);
    }
    return q_names;
}


void SessionExperiment::generatePeakModel(const QString& peakListName)
{
    if( !_experiment->hasPeakCollection(peakListName.toStdString()))
        return;

    nsx::PeakCollection* peak_collection = _experiment->getPeakCollection(
        peakListName.toStdString());

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

    for (std::string name :names){
        nsx::PeakCollection* peak_collection = _experiment->getPeakCollection(name);

        PeakCollectionItem* peak_collection_item = new PeakCollectionItem(peak_collection);
        _peak_collection_items.push_back(peak_collection_item);

        PeakCollectionModel* peak_collection_model = new PeakCollectionModel();
        peak_collection_model->setRoot(peak_collection_item);
        _peak_collection_models.push_back(peak_collection_model);
    }
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
    if (i>=_peak_collection_models.size())
        return nullptr;
    return _peak_collection_models.at(i);
}

std::vector<nsx::Peak3D*> SessionExperiment::getPeaks(
    const QString& peakListName, 
    int upperindex, 
    int lowerindex){

    if( !_experiment->hasPeakCollection(peakListName.toStdString())){
        std::vector<nsx::Peak3D*> peaks;
        return peaks;
    }

    nsx::PeakCollection* peakCollection = _experiment->getPeakCollection(
        peakListName.toStdString());

    std::vector<nsx::Peak3D*> peaks = peakCollection->getPeakList();

    return peaks;
}

QStringList SessionExperiment::getUnitCellNames()
{
    std::vector<std::string> names = _experiment->getUnitCellNames();
    QStringList q_names;

    for (std::string name :names){
        q_names<<QString::fromStdString(name);
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

    if (success){
        _save_path = path.toStdString();
        _saved = true;
    }

    return success;
}
