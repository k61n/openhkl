/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <vector>

#include "CrystalTypes.h"
#include "InstrumentTypes.h"
#include "UtilsTypes.h"

namespace nsx {

//! \class IndexerParameters
//! \brief Struct to store parameters used by autoindexer.
struct IndexerParameters {
    //! Expected maximum dimension of the unit cell, in angstroms
    double maxdim = 200.0;
    //! Number of candidate lattice vectors to use to search for a unit cell
    int nSolutions = 10;
    //! Number of points on the unit sphere to sample when looking for lattice directions
    int nVertices = 10000;
    //! Parameter used to control the number of histogram bins to use during FFT
    int subdiv = 30;
    //! Tolerance used to determine if a peak is indexed by the unit cell
    double indexingTolerance = 0.2;
    //! Tolerance to use when reducing to a Niggli cell
    double niggliTolerance = 1e-3;
    //! Tolerance to use during Gruber reduction
    double gruberTolerance = 4e-2;
    //! Use _only_ Niggli reduction (i.e., no subsequent Gruber reduction) if set true
    bool niggliReduction = false;
    //! Lower bound of expected unit cell volume, used to reject candidate unit cells
    double minUnitCellVolume = 20.0;
    //! Tolerance value used to decide of two unit cells are equivalent
    double unitCellEquivalenceTolerance = 0.05;
    //! Indexing quality cutoff (value indicates percentage of peaks indexed by cell)
    double solutionCutoff = 10.0;
};

//! \class AutoIndexer
//! \brief Class to automatically index a set of peaks
class AutoIndexer {

public:
    //! Constructor
    //! @param handler the handler that will monitor the progresses
    AutoIndexer(const std::shared_ptr<ProgressHandler>& handler = nullptr);

    //! Performs the auto-indexing
    //! @param params the parameter of the auto-indexing algorithm
    //! @return true if auto-indexing succeeded, false if failed
    void autoIndex(const IndexerParameters& params);

    //! Add a peak for the auto-indexing procedure
    void addPeak(sptrPeak3D peak);

    //! Returns the best solutions found for the auto-indexing
    //! @return a list of the best solutions ordered by percentage of successfully auto-indexed peaks
    const std::vector<RankedSolution>& solutions() const;

private:

    void computeFFTSolutions();

    void refineSolutions();

    void rankSolutions();

    void refineConstraints();

    void removeBad(double quality);
  
    IndexerParameters _params;

    PeakList _peaks;

    std::vector<RankedSolution> _solutions;

    std::shared_ptr<ProgressHandler> _handler;
};

} // end namespace nsx
