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

#ifndef GUI_MODELS_SESSIONEXPERIMENT_H
#define GUI_MODELS_SESSIONEXPERIMENT_H

#include "core/experiment/Experiment.h"
#include "core/instrument/InstrumentTypes.h"

#include "core/shape/ShapeLibrary.h"
#include "tables/crystal/UnitCell.h"

#include "core/peak/Peak3D.h"
#include "core/peak/PeakCollection.h"
#include "gui/models/PeakCollectionModel.h"

#include <QStandardItemModel>

//! Controls and handles the Experiment and its Peaks and UnitCells
class SessionExperiment {

public:
   SessionExperiment();

public:
   nsx::sptrExperiment experiment() { return _experiment; }
   QStringList getDataNames();
   nsx::sptrDataSet getData(int index = -1);
   QList<nsx::sptrDataSet> allData();
   int getIndex(const QString&);
   void selectData(int selected) { dataIndex_ = selected; }
   void changeInstrument(const QString& instrumentname);

public:
   //! Get the associated peaks
   std::vector<nsx::Peak3D*>* getPeaks(
      const QString& peakListName, 
      int upperindex = -1, 
      int lowerindex = -1);
   //! Get the names of peaks present in the core (depth obsolete)
   QStringList getPeakListNames(int depth = 1);
   //! Set the currently selected peakModel
   void setSelected(std::string name);
   //! get the currently selected peakModel
   PeakCollectionModel* selected();
   //! Generate a peak model based on the Peak collection in the core
   void generatePeakModel(const QString& peakListName);
   //! Get the peaklist model by name
   PeakCollectionModel* peakModel(const QString& name);
   //! Get the peaklist model by number
   PeakCollectionModel* peakModel(int i) {return _peak_models.at(i);};
   //! generate a peakListModel
   void generatePeakListModel();
   //! Get the Peak list model
   QStandardItemModel* peakListModel() {return &_peak_list_model;};
   //! Tell the gui that peaks have changed
   void onPeaksChanged();
   //! Integrate the the peaks (obsolete)
   void integratePeaks();

public:
   void addUnitCell(nsx::sptrUnitCell uc) { unitCells_.append(uc); }
   nsx::sptrUnitCell getUnitCell(int index = -1);
   void removeUnitCell(int index = -1);
   QStringList getUnitCellNames();
   void selectUnitCell(int select) { unitCellIndex_ = select; }
   QList<nsx::sptrUnitCell>* getUnitCellList(nsx::sptrUnitCell cell);
   
   
public:
   void setLibrary(nsx::sptrShapeLibrary shapeLibrary) { _library = shapeLibrary; }
   nsx::sptrShapeLibrary getLibrary() { return _library; }
   
 private:
   //! Pointer to the core experiment
   nsx::sptrExperiment _experiment;
   //! Pointer to the core library
   nsx::sptrShapeLibrary _library;
   //! The list of models for the peaks
   QList<PeakCollectionModel*> _peak_models; 
   //! The PeakListModel
   QStandardItemModel _peak_list_model;
   //! The selected list
   PeakCollectionModel* _selected = nullptr;
   //! TODO update
   QList<nsx::sptrUnitCell> unitCells_;
   QString selectedList_;
   int unitCellIndex_ = -1;
   int dataIndex_ = -1;

};

#endif // GUI_MODELS_SESSIONEXPERIMENT_H
