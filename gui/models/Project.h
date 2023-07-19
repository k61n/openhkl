//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/Project.h
//! @brief     Defines class Project
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_MODELS_SESSIONEXPERIMENT_H
#define OHKL_GUI_MODELS_SESSIONEXPERIMENT_H

#include "core/data/DataTypes.h"
#include <QStringList>
#include <memory>
#include <string>
#include <vector>

namespace ohkl {
class DataSet;
class Experiment;
class Peak3D;
class PeakCollection;
class UnitCell;
enum class PeakCollectionType;
}
class PeakCollectionItem;
class PeakCollectionModel;

//! Controls and handles the Experiment and its Peaks and UnitCells
class Project {
 public:
    Project() = delete;
    Project(QString name, QString instrument, bool strategy = false);
    Project(const Project&) = delete;

    const ohkl::Experiment* experiment() const;
    ohkl::Experiment* experiment();
    //! Returns list of DataSet names.
    QStringList getDataNames() const;
    std::shared_ptr<ohkl::DataSet> getData(int index = -1) const;
    std::vector<std::shared_ptr<ohkl::DataSet>> allData() const;
    int getIndex(const QString&) const;
    void selectData(int selected);
    void changeInstrument(const QString& instrumentname);
    bool saved() const { return _saved; };
    unsigned int id() const { return _id; };
    QString directory() const { return _directory; };
    void setDirectory(const QString& dir) { _directory = dir; };
    void writeYaml() const;
    void readYaml();

    std::vector<ohkl::Peak3D*> getPeaks(
        const QString& peakListName, int upperindex = -1, int lowerindex = -1) const;

    QStringList getPeakListNames() const;
    QStringList getPeakCollectionNames(ohkl::PeakCollectionType lt) const;

    int numPeakCollections() const;

    void generatePeakModel(const QString& peakListName);
    void generatePeakModels();
    void removePeakModel(const QString& name);
    const PeakCollectionModel* peakModel(const QString& name) const;
    PeakCollectionModel* peakModelAt(int i);

    void clonePeakCollection(const QString& name, const QString& new_name);

    void addUnitCell(const std::string& name, const ohkl::UnitCell& unit_cell);
    QStringList getUnitCellNames() const;
    int numUnitCells() const;

    void saveToFile(QString path);
    void saveAs(QString /*path*/) const { return; };

    void setCurrentData(ohkl::sptrDataSet data) { _current_data = data; };
    ohkl::sptrDataSet currentData() const { return _current_data; };
    QString currentFileName() const;
    void setCurrentFileName(const QString& name);

    bool hasDataSet() const;
    bool hasPeakCollection() const;
    bool hasInstrumentStateSet() const;
    bool hasUnitCell() const;
    bool hasShapeModel() const;
    void setStrategyMode(bool flag) { _strategy = flag; };
    bool strategyMode() const { return _strategy; };

 private:
    std::unique_ptr<ohkl::Experiment> _experiment;

    std::vector<PeakCollectionModel*> _peak_collection_models;
    std::vector<PeakCollectionItem*> _peak_collection_items;

    ohkl::sptrDataSet _current_data;

    bool _strategy;

    bool _saved = false;
    std::string _save_path;
    int _dataIndex = -1;

    // Current save file name
    QString _file_name;
    // Project directory
    QString _directory;
    unsigned int _id;

    static unsigned int _last_id;
};

#endif // OHKL_GUI_MODELS_SESSIONEXPERIMENT_H
