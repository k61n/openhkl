//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/Project.cpp
//! @brief     Implements class Project
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/Project.h"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "gui/MainWin.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QDebug>
#include <QDir>
#include <QStringList>

#include <vector>

unsigned int Project::_last_id = 0;

Project::Project()
    : _experiment(new ohkl::Experiment{})
    , _current_data(nullptr)
    , _strategy(false)
    , _id(++_last_id)
{
}

Project::Project(QString name, QString instrument, bool strategy)
    : _experiment{new ohkl::Experiment{name.toStdString(), instrument.toStdString()}}
    , _current_data(nullptr)
    , _strategy(strategy)
    , _id(++_last_id)
{
    _experiment->setStrategy(strategy);
}

const ohkl::Experiment* Project::experiment() const
{
    return _experiment.get();
}

ohkl::Experiment* Project::experiment()
{
    return _experiment.get();
}

QStringList Project::getDataNames() const
{
    QStringList ret;
    for (auto const& [key, value] : *_experiment->getDataMap())
        ret.push_back(QString::fromStdString(key));
    return ret;
}

void Project::writeYaml() const
{
    if (_directory.isEmpty())
        return;

    QString filename = QString::fromStdString(_experiment->name()) + ".yml";
    QString path = QDir(_directory).filePath(filename);
    _experiment->saveToYaml(path.toStdString());
}

void Project::readYaml()
{
    QString filename = QString::fromStdString(_experiment->name()) + ".yml";
    QString path = QDir(_directory).filePath(filename);
    QFileInfo info(path);
    if (info.exists() && info.isFile())
        _experiment->readFromYaml(path.toStdString());
}


// TODO: move logic to core
ohkl::sptrDataSet Project::getData(int index) const
{
    if (index == -1)
        index = _dataIndex;

    if (!_experiment->numData())
        return nullptr;

    std::string selected = getDataNames().at(index).toStdString();

    return _experiment->getData(selected);
}

// TODO: move logic to core
int Project::getIndex(const QString& dataname) const
{
    QStringList liste = getDataNames();
    return liste.indexOf(dataname);
}

void Project::selectData(int selected)
{
    _dataIndex = selected;
}

std::vector<ohkl::sptrDataSet> Project::allData() const
{
    std::vector<ohkl::sptrDataSet> ret;
    for (auto const& [key, value] : *_experiment->getDataMap())
        ret.push_back(value);
    return ret;
}

QStringList Project::getPeakCollectionNames(ohkl::sptrDataSet data /* = nullptr */) const
{
    auto collections = _experiment->getPeakCollections(data);
    QStringList ret;
    for (auto* collection : collections)
        ret.push_back(QString::fromStdString(collection->name()));
    return ret;
}

QStringList Project::getPeakCollectionNames(
    ohkl::PeakCollectionType lt, ohkl::sptrDataSet data /* = nullptr */) const
{
    auto collections = _experiment->getPeakCollections(data);
    QStringList ret;
    for (auto* collection : collections)
        if (collection->type() == lt)
            ret.push_back(QString::fromStdString(collection->name()));
    return ret;
}

int Project::numPeakCollections() const
{
    return _experiment->numPeakCollections();
}

void Project::generatePeakModel(const QString& peakListName)
{
    if (!_experiment->hasPeakCollection(peakListName.toStdString()))
        return;

    ohkl::PeakCollection* peak_collection =
        _experiment->getPeakCollection(peakListName.toStdString());

    PeakCollectionItem* peak_collection_item = new PeakCollectionItem(peak_collection);
    _peak_collection_items.push_back(peak_collection_item);

    PeakCollectionModel* peak_collection_model = new PeakCollectionModel;
    peak_collection_model->setRoot(peak_collection_item);
    _peak_collection_models.push_back(peak_collection_model);
}

void Project::generatePeakModels()
{
    _peak_collection_models.clear();
    auto peak_collections = _experiment->getPeakCollections();

    for (const auto* collection : peak_collections) {
        PeakCollectionItem* peak_collection_item = new PeakCollectionItem(collection);
        _peak_collection_items.push_back(peak_collection_item);

        PeakCollectionModel* peak_collection_model = new PeakCollectionModel();
        peak_collection_model->setRoot(peak_collection_item);
        _peak_collection_models.push_back(peak_collection_model);
    }
}

void Project::removePeakModel(const QString& name)
{
    const std::string std_name = name.toStdString();
    if (!_experiment->getPeakCollection(std_name))
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

const PeakCollectionModel* Project::peakModel(const QString& name) const
{
    std::string std_name = name.toStdString();
    for (int i = 0; i < _peak_collection_models.size(); ++i) {
        if (_peak_collection_models.at(i)->name() == std_name)
            return _peak_collection_models.at(i);
    }
    return nullptr;
}

PeakCollectionModel* Project::peakModelAt(int i)
{
    if (i >= _peak_collection_models.size())
        return nullptr;
    return _peak_collection_models.at(i);
}

void Project::clonePeakCollection(const QString& name, const QString& new_name)
{
    if (_experiment->hasPeakCollection(name.toStdString())) {
        _experiment->clonePeakCollection(name.toStdString(), new_name.toStdString());
        generatePeakModels();
    }
}

std::vector<ohkl::Peak3D*> Project::getPeaks(
    const QString& peakListName, int /*upperindex*/, int /*lowerindex*/) const
{
    if (!_experiment->hasPeakCollection(peakListName.toStdString()))
        return {};
    return _experiment->getPeakCollection(peakListName.toStdString())->getPeakList();
}

void Project::addUnitCell(const std::string& name, const ohkl::UnitCell& unit_cell)
{
    _experiment->addUnitCell(name, unit_cell);
}

QStringList Project::getUnitCellNames() const
{
    QStringList ret;
    for (const std::string& name : _experiment->getUnitCellNames())
        ret << QString::fromStdString(name);
    return ret;
}

int Project::numUnitCells() const
{
    return _experiment->numUnitCells();
}

void Project::changeInstrument(const QString& instrumentname)
{
    // Avoid changing the instrument, if Experiment has already some data
    if (_experiment->numData())
        return;
    _experiment.reset(new ohkl::Experiment{_experiment->name(), instrumentname.toStdString()});
}

void Project::saveToFile(QString path)
{
    try {
        experiment()->saveToFile(path.toStdString());
        _save_path = path.toStdString();
        _saved = true;
        _file_name = path;
    } catch (const std::exception& ex) {
        throw;
    } catch (...) {
        throw std::runtime_error("Failed to save the project to file '" + path.toStdString() + "'");
    }
}

QString Project::currentFileName() const
{
    if (_file_name.isEmpty())
        return QString::fromStdString(_experiment->name());
    return _file_name;
}

void Project::setCurrentFileName(const QString& name)
{
    _file_name = name;
}

bool Project::hasDataSet() const
{
    return _experiment->numData() > 0;
}

bool Project::hasPeakCollection() const
{
    return _experiment->numPeakCollections() > 0;
}

bool Project::hasUnitCell() const
{
    return _experiment->numUnitCells() > 0;
}

bool Project::hasInstrumentStateSet() const
{
    return _experiment->numInstrumentStateSets() > 0;
}

bool Project::hasShapeModel() const
{
    return _experiment->numShapeModels() > 0;
}
