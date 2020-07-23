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
#include "core/instrument/InstrumentTypes.h"

#include "core/shape/ShapeLibrary.h"
#include "tables/crystal/UnitCell.h"

#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "gui/models/PeakCollectionModel.h"

//! Controls and handles the Experiment and its Peaks and UnitCells
class SessionExperiment {
 public:
    SessionExperiment();
    SessionExperiment(QString name, QString instrument);

 public:
    const nsx::Experiment* experiment() const { return _experiment.get(); }
    nsx::Experiment* experiment() { return _experiment.get(); }
    QStringList getDataNames() const;
    nsx::sptrDataSet getData(int index = -1) const;
    std::vector<nsx::sptrDataSet> allData() const;
    int getIndex(const QString&) const;
    void selectData(int selected) { dataIndex_ = selected; }
    void changeInstrument(const QString& instrumentname);
    bool saved() const { return _saved; };

    //! Get the associated peaks
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

 public:
    //! Add a unit cell to the experiment
    void addUnitCell(std::string& name, nsx::UnitCell* unit_cell)
    {
        _experiment->addUnitCell(name, unit_cell);
    }
    //! Get the names of the Unit cells
    QStringList getUnitCellNames() const;
    //! Get the number of unit cells
    int numUnitCells() const { return _experiment->numUnitCells(); };

 public:
    //! The save method
    void saveToFile(QString path);
    //! Save as
    void saveAs(QString /*path*/) const { return; };

 private:
    //! Pointer to the core experiment
    std::shared_ptr<nsx::Experiment> _experiment;
    //! The list of models for the peaks
    std::vector<PeakCollectionModel*> _peak_collection_models;
    //! The list of ? TODO
    std::vector<PeakCollectionItem*> _peak_collection_items;
    //! Is this session experiment saved
    bool _saved = false;
    //! Save path variable
    std::string _save_path;
    //! TODO update
    std::vector<nsx::sptrUnitCell> unitCells_;
    QString selectedList_;
    int dataIndex_ = -1;
};

#endif // NSX_GUI_MODELS_SESSIONEXPERIMENT_H
