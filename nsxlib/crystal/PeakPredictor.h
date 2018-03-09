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
#include "ShapeLibrary.h"
#include "UtilsTypes.h"

namespace nsx {

//! \class PeakPredictor
//! \brief Class to predict peak shapes based on observed peaks.
class PeakPredictor {
public:
    //! Return predicted peaks on a given data set. Parameter \p keepObserved determines whether to include
    //! predictions for peaks which are already part of the data set.
    PeakPredictor(sptrUnitCell cell, double dmin, double dmax, sptrShapeLibrary library);

    //! Is the peak h,k,l in Bragg condition in this dataset. Return Peak pointer if true,
    //! otherwise nullptr.
    PeakList predict(sptrDataSet data) const;        

    //! Helper method
    PeakList predictPeaks(sptrDataSet data, const std::vector<MillerIndex>& hkls, const Eigen::Matrix3d& BU) const;
   
public:
    //! Unit cell
    sptrUnitCell _cell;
    //! Minimum d value used in prediction.
    double _dmin;
    //! Maximum d value used in prediction.
    double _dmax;
    //! Library used for shape prediction
    sptrShapeLibrary _library;
};

} // end namespace nsx
