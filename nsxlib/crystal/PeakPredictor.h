/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "DataTypes.h"
#include "GeometryTypes.h"
#include "InstrumentTypes.h"
#include "UtilsTypes.h"

namespace nsx {

//! \class PeakPredictor
//! \brief Class to predict peak shapes based on observed peaks.
class PeakPredictor {
public:
    //! Default constructor: sets paramters to reasonable default values;
    PeakPredictor(sptrDataSet data);
    //! Return predicted peaks on a given data set. Parameter \p keepObserved determines whether to include
    //! predictions for peaks which are already part of the data set.
    PeakList predictPeaks(bool keepObserved, const PeakList& reference_peaks);

    //! Is the peak h,k,l in Bragg condition in this dataset. Return Peak pointer if true,
    //! otherwise nullptr.
    PeakList predictPeaks(const std::vector<MillerIndex>& hkls, const Eigen::Matrix3d& BU);
    
    //! Return vector of detector events corresponding to the given q values.
    std::vector<DirectVector> getEvents(const std::vector<ReciprocalVector>& qs) const;

    //! Transform an ellipsoid in q space to detector space.
    Ellipsoid toDetectorSpace(const Ellipsoid& qshape) const;

    //! Return the average shape in q-space of a set of peaks
    static Eigen::Matrix3d averageQShape(const PeakList& peaks);
   
public:
    //! Minimum d value used in prediction.
    double _dmin;
    //! Maximum d value used in prediction.
    double _dmax;
    //! Minimum value of \f$I / \sigma_I\f$ to use for neighbor search.
    double _Isigma;
    //! Minimum number of nearby peaks to use for shape determination.
    int _minimumNeighbors;
    //! Optional progress handler to monitor progress.
    sptrProgressHandler _handler;
    sptrDataSet _data;
};

} // end namespace nsx
