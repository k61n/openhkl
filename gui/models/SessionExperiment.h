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

#include <QStandardItemModel>

//! Controls and handles the Experiment and its Peaks and UnitCells
class SessionExperiment {

 public:
    SessionExperiment();
    SessionExperiment(QString name, QString instrument);

 public:
    nsx::sptrExperiment experiment() { return _experiment; }
    QStringList getDataNames();
    nsx::sptrDataSet getData(int index = -1);
    QList<nsx::sptrDataSet> allData();
    int getIndex(const QString&);
    void selectData(int selected) { dataIndex_ = selected; }
    void changeInstrument(const QString& instrumentname);
    bool saved() const { return _saved; };

 public:
    //! Get the associated peaks
    std::vector<nsx::Peak3D*>
    getPeaks(const QString& peakListName, int upperindex = -1, int lowerindex = -1);
    //! Get the names of peaks present in the core
    QStringList getPeakListNames();
    //! Get the names of peaks present in the core
    QStringList getFoundNames();
    //! Get the names of peaks present in the core
    QStringList getPredictedNames();
    //! Get the number of peak lists
    int numPeakCollections() const { return _experiment->numPeakCollections(); };
    //! Generate a peak model based on the Peak collection in the core
    void generatePeakModel(const QString& peakListName);
    //! Generate a peak model based on the Peak collection in the core
    void generatePeakModels();
    //! Generate a peak model based on the Peak collection in the core
    void removePeakModel(const QString& name);
    //! Get the peaklist model by name
    PeakCollectionModel* peakModel(const QString& name);
    //! Get the peaklist model by number
    PeakCollectionModel* peakModel(int i);
    //! Tell the gui that peaks have changed
    void onPeaksChanged();

 public:
    //! Add a unit cell to the experiment
    void addUnitCell(std::string& name, nsx::UnitCell* unit_cell)
    {
        _experiment->addUnitCell(name, unit_cell);
    }
    //! Get the names of the Unit cells
    QStringList getUnitCellNames();
    //! Get the number of unit cells
    int numUnitCells() const { return _experiment->numUnitCells(); };

 public:
    //! The save method
    void saveToFile(QString path);
    //! Save as
    void saveAs(QString /*path*/) const { return; };

 private:
    //! Pointer to the core experiment
    nsx::sptrExperiment _experiment;
    //! The list of models for the peaks
    std::vector<PeakCollectionModel*> _peak_collection_models;
    //! The list of models for the peaks
    std::vector<PeakCollectionItem*> _peak_collection_items;
    //! Is this session experiment saved
    bool _saved = false;
    //! Save path variable
    std::string _save_path;
    //! TODO update
    QList<nsx::sptrUnitCell> unitCells_;
    QString selectedList_;
    int dataIndex_ = -1;
};

#endif // NSX_GUI_MODELS_SESSIONEXPERIMENT_H
