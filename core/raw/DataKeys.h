//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/DataKeys.h
//! @brief     Defines keys for storing the data
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_RAW_DATAKEYS_H
#define NSX_CORE_RAW_DATAKEYS_H

#include <set>
#include <string>

namespace ohkl {

// TODO: Add documentation: Describe the keywords and their units (if any)

//-- HDF5 Group keys
const std::string gr_DataCollections{"DataCollections"};
const std::string gr_Detector{"Detector"};
const std::string gr_Instrument{"Instrument"};
const std::string gr_Sample{"Sample"};
const std::string gr_Metadata{"Metadata"};
const std::string gr_PeakCollections{"PeakCollections"};
const std::string gr_UnitCells{"UnitCells"};

//-- HDF5 DataSet keys
const std::string ds_Dataset{"Dataset"};
const std::string ds_DatasetNames{"DatasetNames"};
const std::string ds_UnitCellNames{"UnitCellNames"};

// instrument state
const std::string ds_detectorOrientation{"detector orientation"};
const std::string ds_detectorPositionOffset{"detector position offset"};
const std::string ds_sampleOrientation{"sample orientation"};
const std::string ds_sampleOrientationOffset{"sample orientation offset"};
const std::string ds_samplePosition{"sample position"};
const std::string ds_beamDirection{"incoming beam direction"};
const std::string ds_beamWavelength{"incoming beam wavelength"};
const std::string ds_isRefinedState{"is refined"};

// peaks
const std::string ds_PeakEnd{"PeakEnd"};
const std::string ds_BkgBegin{"BkgBegin"};
const std::string ds_BkgEnd{"BkgEnd"};
const std::string ds_Scale{"Scale"};
const std::string ds_Transmission{"Transmission"};
const std::string ds_Intensity{"Intensity"};
const std::string ds_Sigma{"Sigma"};
const std::string ds_BkgIntensity{"BkgIntensity"};
const std::string ds_BkgSigma{"BkgSigma"};
const std::string ds_Center{"Center"};
const std::string ds_Metric{"Metric"};
const std::string ds_Rejection{"Rejection"};
const std::string ds_Selected{"Selected"};
const std::string ds_Masked{"Masked"};
const std::string ds_Predicted{"Predicted"};
const std::string ds_hkl{"hkl"};
const std::string ds_hklError{"hklError"};

//-- HDF Attribute keys
// general
const std::string at_commitHash("commit hash");
const std::string at_nsxVersion("version");
const std::string at_experiment{"experiment"};
const std::string at_diffractometer{"diffractometer"};
const std::string at_datasetName{"dataset"};
const std::string at_formatVersion{"NSX-format version"};
const std::string at_datasetSources{"sources"}; // list of sources for the DataSet, separated by ';'
const std::string at_frameCount{"number of frames"};
const std::string at_wavelength{"wavelength"};
const std::string at_monitorSum{"monitor"}; // TODO: explain
// DataSet metadata
const std::string at_baseline{"baseline"};
const std::string at_gain{"gain"};
// Nexus-specific metadata
const std::string at_numor{"numor"}; // internal raw data labelling of ILL
const std::string at_totalSteps{"total steps"};
const std::string at_title{"title"};
const std::string at_time{"time"};
const std::string at_startTime{"start time"};
const std::string at_endTime{"end time"};
// peaks
const std::string at_peakCount{"number of peaks"};
const std::string at_peakType{"peak type"};
const std::string at_indexed{"is indexed"};
const std::string at_integrated{"is integrated"};
// unit cell
const std::string at_unitCellName{"unit cell name"};
const std::string at_rVec{"reciprocal"};
const std::string at_BravaisLattice{"Bravais lattice"};
const std::string at_indexingTol{"indexing tolerance"};
const std::string at_spacegroup{"spacegroup"};
const std::string at_z{"z"};

//-- Default names
const std::string kw_experimentDefaultName{"DEFAULT EXPERIMENT"}; // default name for an experiment
const std::string kw_diffractometerDefaultName{
    "DEFAULT INSTRUMENT"}; // default name for an experiment
const std::string kw_datasetDefaultName{"DEFAULT DATASET"}; // default name for an experiment
const std::string kw_unitcellDefaultName{"DEFAULT UNITCELL"}; // default name for a unit-cell
const std::string kw_monochromatorDefaultName{
    "DEFAULT MONOCHROMATOR"}; // default name for a monochromator
const std::string kw_goniometerDefaultName{"DEFAULT GONIOMETER"}; // default name for a goniometer
const std::string kw_axisDefaultName{"DEFUALT AXIS"}; // default name for an axis
const unsigned int kw_axisDefaultId{0}; // default axis id
const std::string kw_detectorDefaultName{"DEFAULT DETECTOR"}; // default name for a detector
const std::string kw_peakCollectionDefaultName{"DEFAULT PEAKCOLLECTION"};
const std::string kw_rotationAxisName{"rotation"};
const std::string kw_autoindexingCollection{"autoindexing"};
const std::string kw_fitCollection{"fit"};
const std::string kw_acceptedUnitcell{"accepted"};
const std::string kw_referenceUnitcell{"reference"};
const std::string kw_logFilename{"openhkl.log"};

//-- Axis names
// detector gonio
const std::string ax_2thetaGamma{"2theta(gamma)"};
const std::string ax_2thetaNu{"2theta(nu)"};
// sample gonio
const std::string ax_omega{"omega"};
const std::string ax_chi{"chi"};
const std::string ax_phi{"phi"};

//-- YAML keywords
const std::string ym_instrument{"instrument"};
const std::string ym_monochromator{"monochromator"};
const std::string ym_instrumentName{"name"};
const std::string ym_detectorType{"type"};
const std::string ym_detector{"detector"};
const std::string ym_sample{"sample"};
const std::string ym_source{"source"};
const std::string ym_units{"units"};
const std::string ym_value{"value"};
const std::string ym_width{"width"};
const std::string ym_angularWidth{"angular_width"};
const std::string ym_height{"height"};
const std::string ym_fwhm{"fwhm"};
const std::string ym_wavelength{"wavelength"};
const std::string ym_goniometerName{"name"};
const std::string ym_axis{"axis"};
const std::string ym_axisType{"type"};
const std::string ym_axisName{"name"};
const std::string ym_axisDirection{"direction"};
const std::string ym_axisPhysical{"physical"};
const std::string ym_axisId{"id"};
const std::string ym_dataOrdering{"data_ordering"};
const std::string ym_baseline{"baseline"};
const std::string ym_gain{"gain"};
const std::string ym_sampleDistance{"sample_distance"};
const std::string ym_colCount{"ncols"};
const std::string ym_rowCount{"nrows"};
const std::string ym_originX{"origin_x"};
const std::string ym_originY{"origin_y"};
const std::string ym_componentName{"name"};
const std::string ym_goniometer{"goniometer"};
const std::string ym_rotClockwise{"clockwise"};

//-- Recognized metadata keys
const std::set<std::string> RecognizedMetaDataKeys{at_experiment,     at_diffractometer,
                                                   at_formatVersion,  at_datasetSources,
                                                   at_frameCount,     at_wavelength,
                                                   at_monitorSum,     at_numor,
                                                   at_totalSteps,     at_title,
                                                   at_time,           at_startTime,
                                                   at_endTime,        at_peakCount,
                                                   at_peakType,       at_rVec,
                                                   at_BravaisLattice, at_indexingTol,
                                                   at_spacegroup,     at_z,
                                                   at_baseline,       at_gain};

} // namespace ohkl

#endif // NSX_CORE_RAW_DATAKEYS_H
