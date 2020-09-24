//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/AutoIndexer.h
//! @brief     Defines class AutoIndexer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_ALGO_AUTOINDEXER_H
#define NSX_CORE_ALGO_AUTOINDEXER_H

#include "base/utils/ProgressHandler.h"
#include "core/shape/PeakCollection.h"

namespace nsx {

using RankedSolution = std::pair<std::shared_ptr<UnitCell>, double>;
    using SolutionList = std::vector<RankedSolution>;

enum class Level;

//! Struct to store parameters used by autoindexer.
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

    void log(const Level& level);
};

//! Class to automatically index a set of peaks
class AutoIndexer {
 public:
    //! Constructor
    AutoIndexer();
    //! Return the parameters
    IndexerParameters parameters() const { return _params; };
    //! Set the parameters
    void setParameters(IndexerParameters parameters) { _params = parameters; };
    //! Performs the auto-indexing
    void autoIndex(const std::vector<Peak3D*>& peaks);
    //! Autoindex by passing a peak collection (avoid SWIG memory leak)
    void autoIndex(PeakCollection* peaks);
    //! Returns list of the best solutions ordered by percentage of successfully indexed peaks
    const std::vector<RankedSolution>& solutions() const;

    //! Set the handler
    void setHandler(std::shared_ptr<ProgressHandler> handler) { _handler = handler; };
    //! Set the handler
    void unsetHandler() { _handler = nullptr; };
    //! Dump SolutionList as a string
    std::string solutionsToString() const;
    //! Set solution to be unit cell for given peak list
    void acceptSolution(const UnitCell* solution, const std::vector<nsx::Peak3D*>& peaks);
    //! Check if list of solutions contains reference unit cell. If it does,
    //! return a pointer, otherwise return nullptr
    UnitCell* goodSolution(UnitCell* ref_cell, double length_tol, double angle_tol);

 private:
    void computeFFTSolutions(const std::vector<Peak3D*>& peaks);
    void refineSolutions(const std::vector<Peak3D*>& peaks);
    void rankSolutions();
    void refineConstraints();
    void removeBad(double quality);

    IndexerParameters _params;
    std::vector<RankedSolution> _solutions;
    std::shared_ptr<ProgressHandler> _handler;
};

} // namespace nsx

#endif // NSX_CORE_ALGO_AUTOINDEXER_H
