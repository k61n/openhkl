//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/SessionExperiment.h
//! @brief     Defines class SessionExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_MODELS_SESSIONEXPERIMENT_H
#define NSX_GUI_MODELS_SESSIONEXPERIMENT_H

#include "core/experiment/Experiment.h"

namespace nsx {
class Peak3D;
class PeakCollection;
class UnitCell;
}
class PeakCollectionItem;
class PeakCollectionModel;

//! Controls and handles the Experiment and its Peaks and UnitCells
class SessionExperiment {
 public:
    SessionExperiment() = delete;
    SessionExperiment(QString name, QString instrument);
    SessionExperiment(const SessionExperiment&) = delete;

    const nsx::Experiment* experiment() const { return _experiment.get(); }
    nsx::Experiment* experiment() { return _experiment.get(); }
    QStringList getDataNames() const;
    nsx::sptrDataSet getData(int index = -1) const;
    std::vector<nsx::sptrDataSet> allData() const;
    int getIndex(const QString&) const;
    void selectData(int selected) { _dataIndex = selected; }
    void changeInstrument(const QString& instrumentname);
    bool saved() const { return _saved; };

    std::vector<nsx::Peak3D*>
    getPeaks(const QString& peakListName, int upperindex = -1, int lowerindex = -1) const;

    QStringList getPeakListNames() const;
    QStringList getFoundNames() const;
    QStringList getPredictedNames() const;

    int numPeakCollections() const { return _experiment->numPeakCollections(); };

    void generatePeakModel(const QString& peakListName);
    void generatePeakModels();
    void removePeakModel(const QString& name);
    const PeakCollectionModel* peakModel(const QString& name) const;
    PeakCollectionModel* peakModelAt(int i);

    void onPeaksChanged();

    void addUnitCell(std::string& name, nsx::UnitCell* unit_cell);
    QStringList getUnitCellNames() const;
    int numUnitCells() const { return _experiment->numUnitCells(); };

    void saveToFile(QString path);
    void saveAs(QString /*path*/) const { return; };

 private:
    std::shared_ptr<nsx::Experiment> _experiment;

    std::vector<PeakCollectionModel*> _peak_collection_models;
    std::vector<PeakCollectionItem*> _peak_collection_items;

    bool _saved = false;
    std::string _save_path;
    int _dataIndex = -1;
};

#endif // NSX_GUI_MODELS_SESSIONEXPERIMENT_H
