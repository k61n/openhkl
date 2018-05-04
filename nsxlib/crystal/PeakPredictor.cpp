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

#include "CrystalTypes.h"
#include "DataSet.h"
#include "DataTypes.h"
#include "Detector.h"
#include "DirectVector.h"
#include "Diffractometer.h"
#include "GeometryTypes.h"
#include "Gonio.h"
#include "InstrumentState.h"
#include "Logger.h"
#include "MillerIndex.h"
#include "Octree.h"
#include "Peak3D.h"
#include "PeakFilter.h"
#include "PeakPredictor.h"
#include "ProgressHandler.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "ShapeLibrary.h"
#include "SpaceGroup.h"
#include "PeakCoordinateSystem.h"
#include "Source.h"
#include "UnitCell.h"

#include <map>

namespace nsx {

PeakPredictor::PeakPredictor(sptrUnitCell cell, double dmin, double dmax, sptrShapeLibrary library):
    _cell(cell), _dmin(dmin), _dmax(dmax), _library(library)
{
}

PeakList PeakPredictor::predict(sptrDataSet data, double radius, double nframes) const
{
    if (!_library) {
        throw std::runtime_error("PeakPredictor cannot predict without a shape library");
    }

    auto& mono = data->diffractometer()->getSource()->getSelectedMonochromator();
    const double wavelength = mono.getWavelength();
    PeakList calculated_peaks;     
    std::set<MillerIndex> found_hkls;

    auto predicted_hkls = _cell->generateReflectionsInShell(_dmin, _dmax, wavelength); 
    PeakList peaks = predictPeaks(data, predicted_hkls, _cell->reciprocalBasis());

    nsx::info() << "Computing shapes of " << peaks.size() << " calculated peaks...";

    for (size_t peak_id = 0; peak_id < peaks.size(); ++peak_id) {
        sptrPeak3D p = peaks[peak_id];
        p->addUnitCell(_cell, true);
        p->setPredicted(true);
        p->setSelected(true);

        try {
            // can throw if there are too few neighboring peaks
            // todo: number of neighboring peaks should not be hard-coded
            Eigen::Matrix3d cov = _library->meanCovariance(p, radius, nframes, 20);
            //Eigen::Matrix3d cov = _library->predictCovariance(p);
            Eigen::Vector3d center = p->getShape().center();
            p->setShape(Ellipsoid(center, cov.inverse()));
        } catch (std::exception& e) {
            nsx::info() << e.what();
            continue;
        }
        calculated_peaks.push_back(p);
    }    
    return calculated_peaks;
}

PeakList PeakPredictor::predictPeaks(sptrDataSet data, const std::vector<MillerIndex>& hkls, const Eigen::Matrix3d& BU) const
{
    std::vector<ReciprocalVector> qs;
    PeakList peaks;

    for (auto idx: hkls) {
        qs.emplace_back(idx.rowVector().cast<double>()*BU);
    }

    auto events = data->getEvents(qs);
 
    for (auto event: events) {
        sptrPeak3D peak(new Peak3D(data));
        Eigen::Vector3d center = {event._px, event._py, event._frame};

        // dummy shape
        try {
            peak->setShape(Ellipsoid(center, 1.0));
            peaks.push_back(peak);
        } catch(...) {
            // invalid shape, nothing to do
        }
    }
    return peaks;
}

} // end namespace nsx
