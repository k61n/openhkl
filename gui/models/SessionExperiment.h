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
#include "core/peak/Peak3D.h"
#include "core/shape/ShapeLibrary.h"
#include "tables/crystal/UnitCell.h"

#include "core/peak/PeakCollection.h"
#include "gui/models/PeakCollectionModel.h"

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
   //! get the associated peaks
   std::vector<nsx::Peak3D*>* getPeaks(
      const QString& peakListName, 
      int upperindex = -1, 
      int lowerindex = -1);
   //! get the names of peaks present in the core (depth obsolete)
   QStringList getPeakListNames(int depth = 1);
   //! create a peak model based on the Peak collection in the core
   PeakCollectionModel* generatePeakModel(const QString& peakListName);
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

   //! TODO update
   QList<nsx::sptrUnitCell> unitCells_;
   QString selectedList_;
   int unitCellIndex_ = -1;
   int dataIndex_ = -1;
};

#endif // GUI_MODELS_SESSIONEXPERIMENT_H
