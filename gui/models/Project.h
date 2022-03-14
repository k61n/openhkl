//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/Project.h
//! @brief     Defines class Project
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_MODELS_SESSIONEXPERIMENT_H
#define NSX_GUI_MODELS_SESSIONEXPERIMENT_H

#include <QStringList>
#include <memory>
#include <string>
#include <vector>

namespace nsx {
class DataSet;
class Experiment;
class Peak3D;
class PeakCollection;
class UnitCell;
enum class listtype;
}
class PeakCollectionItem;
class PeakCollectionModel;

//! Controls and handles the Experiment and its Peaks and UnitCells
class Project {
 public:
    Project() = delete;
    Project(QString name, QString instrument);
    Project(const Project&) = delete;

    const nsx::Experiment* experiment() const;
    nsx::Experiment* experiment();
    QStringList getDataNames() const;
    std::shared_ptr<nsx::DataSet> getData(int index = -1) const;
    std::vector<std::shared_ptr<nsx::DataSet>> allData() const;
    int getIndex(const QString&) const;
    void selectData(int selected);
    void changeInstrument(const QString& instrumentname);
    bool saved() const { return _saved; };

    std::vector<nsx::Peak3D*> getPeaks(
        const QString& peakListName, int upperindex = -1, int lowerindex = -1) const;

    QStringList getPeakListNames() const;
    QStringList getPeakCollectionNames(nsx::listtype lt) const;

    int numPeakCollections() const;

    void generatePeakModel(const QString& peakListName);
    void generatePeakModels();
    void removePeakModel(const QString& name);
    const PeakCollectionModel* peakModel(const QString& name) const;
    PeakCollectionModel* peakModelAt(int i);

    void clonePeakCollection(const QString& name, const QString& new_name);

    void onPeaksChanged();

    void addUnitCell(const std::string& name, const nsx::UnitCell& unit_cell);
    QStringList getUnitCellNames() const;
    int numUnitCells() const;

    void saveToFile(QString path);
    void saveAs(QString /*path*/) const { return; };

    QString currentFileName() const;
    void setCurrentFileName(const QString& name);

 private:
    std::unique_ptr<nsx::Experiment> _experiment;

    std::vector<PeakCollectionModel*> _peak_collection_models;
    std::vector<PeakCollectionItem*> _peak_collection_items;

    bool _saved = false;
    std::string _save_path;
    int _dataIndex = -1;

    // Current save file name
    QString _file_name;
};

#endif // NSX_GUI_MODELS_SESSIONEXPERIMENT_H
