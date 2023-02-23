//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/AutoIndexer.h
//! @brief     Defines class AutoIndexer
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_ALGO_AUTOINDEXER_H
#define OHKL_CORE_ALGO_AUTOINDEXER_H

#include "base/utils/ProgressHandler.h"
#include "core/instrument/InstrumentState.h"
#include "core/shape/PeakCollection.h"

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

using RankedSolution = std::pair<sptrUnitCell, double>;
using SolutionList = std::vector<RankedSolution>;

enum class Level;

//! Parameters for indexing a peak collection
struct IndexerParameters {
    //! Expected maximum dimension of the unit cell, in angstroms
    double maxdim = 200.0;
    //! Number of candidate lattice vectors to use to search for a unit cell
    int nSolutions = 10;
    //! Number of points on the unit sphere to sample when looking for lattice
    //! directions
    int nVertices = 10000;
    //! Parameter used to control the number of histogram bins to use during FFT
    int subdiv = 30;
    //! Tolerance used to determine if a peak is indexed by the unit cell
    double indexingTolerance = 0.2;
    //! Tolerance to use when reducing to a Niggli cell
    double niggliTolerance = 1e-3;
    //! Tolerance to use during Gruber reduction
    double gruberTolerance = 4e-2;
    //! Use _only_ Niggli reduction (i.e., no subsequent Gruber reduction) if set
    //! true
    bool niggliReduction = false;
    //! Lower bound of expected unit cell volume, used to reject candidate unit
    //! cells
    double minUnitCellVolume = 20.0;
    //! Tolerance value used to decide of two unit cells are equivalent
    double unitCellEquivalenceTolerance = 0.05;
    //! Indexing quality cutoff (value indicates percentage of peaks indexed by
    //! cell)
    double solutionCutoff = 10.0;
    //! Frequency threshold: FFT peaks with 0.7 the value of the zero frequency
    //! peak are discarded
    double frequencyTolerance = 0.7;
    //! Frame from which to start autoindexing
    double first_frame = 0.0;
    //! Last frame of autoindexing set
    double last_frame = 10.0;
    //! Minimum detector range
    double d_min = 1.5;
    //! Maximum detector range
    double d_max = 50.0;
    //! Minimum peak strength
    double strength_min = 1.0;
    //! Maximum peak strength
    double strength_max = 1.0e7;
    //! Whether the peaks are integrated
    bool peaks_integrated = false;

    void log(const Level& level) const;
};

/*! \brief Class to automatically index a set of peaks
 *
 *  This class uses 1D Fourier transform algorithm to generate a list of
 *  candidate unit cells (I. Steller et al., J. Appl. Cryst., 30:1036, 1997.
 *  doi:10.1107/S0021889897008777).
 *
 *  A set of random q vectors is generated on the unit sphere, and project the
 *  peak q vectors onto each direction; if the direction corresponds to a
 *  lattice plane, the Fourier transform will be strongly periodic, and the
 *  first non-zero frequency corresponds to the lattice plane. Each peak is
 *  assigned a set of Miller indices and the unit cell determined from these.
 */
class AutoIndexer {
 public:
    //! Constructor
    AutoIndexer();
    //! Return the autoindexing parameters
    IndexerParameters* parameters();
    //! Perform the autoindexing, possibly for a single frame only
    bool autoIndex(
        const std::vector<Peak3D*>& peaks, const InstrumentState* state = nullptr,
        bool filter = true);
    //! Autoindex by passing a peak collection (avoid SWIG memory leak)
    bool autoIndex(
        PeakCollection* peaks, const InstrumentState* state = nullptr, bool filter = true);
    //! Return a list of the best solutions ordered by percentage of successfully indexed peaks
    const std::vector<RankedSolution>& solutions() const;

    //! Set the progress handler
    void setHandler(std::shared_ptr<ProgressHandler> handler) { _handler = handler; };
    //! Remove the progress handler
    void unsetHandler() { _handler = nullptr; };
    //! Dump SolutionList as a string
    std::string solutionsToString() const;
    //! Set solution to be unit cell for given peak list
    void acceptSolution(const sptrUnitCell solution, const std::vector<ohkl::Peak3D*>& peaks);
    //! Set solution to be unit cell for given peak collection
    void acceptSolution(const sptrUnitCell solution, PeakCollection* peaks);
    //! Check if list of solutions contains reference unit cell. If it does,
    //! return a pointer, otherwise return nullptr
    sptrUnitCell goodSolution(const UnitCell* reference_cell, double length_tol, double angle_tol);
    //! Get a list of filtered peaks used in indexing
    std::vector<Peak3D*>* filteredPeaks() { return &_filtered_peaks; };

 private:
    //! Filter the peaks according to AutoIndexerParameters
    void filterPeaks(
        const std::vector<Peak3D*>& peaks, const InstrumentState* state = nullptr);
    //! Get a vector of candidate unit cells from a list of peaks using the
    //! Fourier transform method
    bool computeFFTSolutions(
        const std::vector<Peak3D*>& peaks, const InstrumentState* state = nullptr);
    //! Do least squares minimisation to refine candidate unit cells
    void refineSolutions(const std::vector<Peak3D*>& peaks);
    //! Rand solutions by quality (percentage of peak indexed)
    void rankSolutions();
    // Unused - void refineConstraints();
    //! Remove bad candidate unit cells
    void removeBad(double quality);

    std::vector<Peak3D*> _filtered_peaks;
    std::unique_ptr<IndexerParameters> _params;
    std::vector<RankedSolution> _solutions;
    std::shared_ptr<ProgressHandler> _handler;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_ALGO_AUTOINDEXER_H
