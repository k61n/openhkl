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

#ifndef CORE_ALGO_AUTOINDEXER_H
#define CORE_ALGO_AUTOINDEXER_H

#include "base/utils/ProgressHandler.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCollection.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

using RankedSolution = std::pair<sptrUnitCell, double>;

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
    //! Returns list of the best solutions ordered by percentage of successfully indexed peaks
    const std::vector<RankedSolution>& solutions() const;

    //! Set the handler
    void setHandler(std::shared_ptr<ProgressHandler> handler) { _handler = handler; };
    //! Set the handler
    void unsetHandler() { _handler = nullptr; };

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

#endif // CORE_ALGO_AUTOINDEXER_H
