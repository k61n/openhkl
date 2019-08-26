//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/Experiment.h
//! @brief     Defines class Experiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_EXPERIMENT_EXPERIMENT_H
#define CORE_EXPERIMENT_EXPERIMENT_H

#include "core/experiment/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/analyse/PeakFinder.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCollection.h"
#include "tables/crystal/UnitCell.h"
#include "core/integration/PixelSumIntegrator.h"

namespace nsx {

//! Experiment class, a data type containing a diffractometer and data sets.

class Experiment {

 public:

   Experiment() = delete;
   Experiment(const Experiment& other);
   //! Construct an empty experiment from a given name and diffractometer
   Experiment(const std::string& name, const std::string& diffractometerName);
   ~Experiment() = default;
   Experiment& operator=(const Experiment& other);

public:

   //! Get the name of the Experiment
   const std::string& name() const;
   //! Sets the name of the experiment
   void setName(const std::string& name);
   //! Returns a non-const pointer to the diffractometer related to the experiment
   Diffractometer* diffractometer();
   //! Returns a const pointer to the diffractometer related to the experiment
   const Diffractometer* diffractometer() const;
   //! Set the diffractometer accordingly
   void setDiffractometer(const std::string& diffractometerName);

public:

   //! Gets a reference to the data
   const std::map<std::string, sptrDataSet>& data() const;
   //! Gets the pointer to a given data stored in the experiment
   sptrDataSet data(std::string name);
   //! Gets the pointer to a given data stored in the experiment
   sptrDataSet dataShortName(std::string name);
   //! Get number of data
   int numData() const {return  _data.size();};
   //! Add some data to the experiment
   void addData(sptrDataSet data);
   //! Returns true if the experiment has a data
   bool hasData(const std::string& name) const;
   //! Remove a data from the experiment
   void removeData(const std::string& name);

public:

   //! Add some data to the experiment
   void addPeakCollection(
      const std::string name, 
      const std::vector<std::shared_ptr<nsx::Peak3D>>* peaks);
   //! Returns true if the experiment has a data
   bool hasPeakCollection(const std::string& name) const;
   //! Returns the peak list denoted by the name
   PeakCollection* getPeakCollection(const std::string name);
   //! Remove a data from the experiment
   void removePeakCollection(const std::string& name);
   //! Get a list of loaded list names
   std::vector<std::string*> getCollectionNames() const;
   //! Get the number of peak lists
   int numPeakCollections()const {return _peakCollections.size();};

public:

   //! Add some data to the experiment
   void addUnitCell(const std::string& name, sptrUnitCell unit_cell);
   //! Returns true if the experiment has a data
   bool hasUnitCell(const std::string& name) const;
   //! Returns the unit cell denoted by the name
   sptrUnitCell getUnitCell(const std::string& name);
   //! Remove a data from the experiment
   void removeUnitCell(const std::string& name);

public:

   //! Get the address of the peak finder
   nsx::PeakFinder* peakFinder() {return _peak_finder;};
   //! Transfer current peaks as collection
   void acceptFoundPeaks(const std::string& name);
   //! Get the found peak integrator
   nsx::PixelSumIntegrator* peakFoundIntegrator() {return _found_peak_integrator;};
   //! Set the found peak integrator
   void integrateFoundPeaks( double peak_end, double bkg_begin, double bkg_end);

public:

   //! Save to file
   bool saveToFile(std::string path) const;
   //! Load from file
   bool loadFromFile(std::string path);

private:

   //! The name of this experiment
   std::string _name = "No_name";
   //! A pointer to the detector assigned to this experiment
   std::unique_ptr<Diffractometer> _diffractometer;
   //! A map of the data related to the experiment. The keys are the basename of
   //! their corresponding file.
   std::map<std::string, sptrDataSet> _data;
   //! A map of the peaklists with their name as index
   std::map<std::string, std::unique_ptr<PeakCollection>> _peakCollections;
   //! A map of the unit cells with their name as index
   std::map<std::string, sptrUnitCell> _unit_cells;
   //! The Peak finder
   nsx::PeakFinder* _peak_finder;// = new nsx::PeakFinder();
   //! The found peak integrator
   nsx::PixelSumIntegrator* _found_peak_integrator;// = new nsx::PixelSumIntegrator(true, true);

   //! The peak predictor

   //! The auto indexer

};

} // namespace nsx

#endif // CORE_EXPERIMENT_EXPERIMENT_H
