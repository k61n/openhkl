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

#ifndef NSXTOOL_PEAKPREDICTOR_H_
#define NSXTOOL_PEAKPREDICTOR_H_

#include <memory>

namespace nsx {

class DataSet;

class ProgressHandler;

class PeakPredictor {
public:

    void addPredictedPeaks(std::shared_ptr<nsx::Data::DataSet> data);

public:
    double _dmin, _dmax, _searchRadius, _peakScale, _bkgScale;
    std::shared_ptr<nsx::Utils::ProgressHandler> _handler;
};

} // end namespace nsx

#endif // NSXTOOL_PEAKPREDICTOR
