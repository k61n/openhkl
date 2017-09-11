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

#include "../crystal/CrystalTypes.h"
#include "../crystal/Peak3D.h"
#include "../crystal/PeakPredictor.h"
#include "../crystal/SpaceGroup.h"
#include "../crystal/UnitCell.h"
#include "../data/DataSet.h"
#include "../data/DataTypes.h"
#include "../geometry/GeometryTypes.h"
#include "../geometry/Octree.h"
#include "../instrument/DetectorEvent.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../utils/ProgressHandler.h"

namespace nsx {

PeakPredictor::PeakPredictor():
    _dmin(2.0),
    _dmax(50.0),
    _peakScale(3.0),
    _bkgScale(6.0),
    _searchRadius(100.0),
    _frameRadius(5.0),
    _minimumRadius(2.0),
    _minimumPeakDuration(1.0),
    _minimumNeighbors(10),
    _Isigma(2.0),
    _handler(nullptr)
{

}

PeakSet PeakPredictor::predictPeaks(sptrDataSet data, bool keepObserved)
{

    class compare_fn {
    public:
        auto operator()(const Eigen::RowVector3i a, const Eigen::RowVector3i b) -> bool
        {
            if (a(0) != b(0))
                return a(0) < b(0);

            if (a(1) != b(1))
                return a(1) < b(1);

            return a(2) < b(2);
        }
    };


    int predicted_peaks = 0;

    auto& mono = data->getDiffractometer()->getSource()->getSelectedMonochromator();
    const double wavelength = mono.getWavelength();
    PeakSet calculated_peaks;

    auto sample = data->getDiffractometer()->getSample();
    unsigned int ncrystals = static_cast<unsigned int>(sample->getNCrystals());

    for (unsigned int i = 0; i < ncrystals; ++i) {
        SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
        auto cell = sample->getUnitCell(i);
        auto UB = cell->reciprocalBasis();

        _handler->setStatus("Calculating peak locations...");

        //auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
        auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInShell(_dmin, _dmax, wavelength);

        // todo: clean up DataSet interface for predicted peaks
        std::vector<Eigen::RowVector3d> hkls_double;

        for (auto&& hkl: predicted_hkls) {
            hkls_double.emplace_back(hkl.cast<double>());
        }

        predicted_peaks += predicted_hkls.size();

        PeakList peaks = data->hasPeaks(hkls_double, UB);
        int current_peak = 0;

        _handler->setStatus("Building set of previously found peaks...");

        PeakSet found_peaks = data->getPeaks();
        std::set<Eigen::RowVector3i, compare_fn> found_hkls;

        Eigen::Vector3d lb = {0.0, 0.0, 0.0};
        Eigen::Vector3d ub = {double(data->getNCols()), double(data->getNRows()), double(data->getNFrames())};
        auto&& octree = Octree(lb, ub);

        octree.setMaxDepth(4);
        octree.setMaxStorage(50);

        _handler->log("Building peak octree...");

        for (sptrPeak3D p: found_peaks) {
            found_hkls.insert(p->getIntegerMillerIndices());

            // ignore deselected and masked peaks
            if (!p->isSelected() || p->isMasked()) {
                continue;
            }

            Intensity inten = p->getCorrectedIntensity();

            // peak must have minimum I / sigma ratio
            if (inten.value() / inten.sigma() < _Isigma) {
                continue;
            }

            octree.addData(&p->getShape());
        }

        _handler->log("Done building octree; number of chambers is " + std::to_string(octree.numChambers()));
        _handler->setStatus("Adding calculated peaks...");

        int done_peaks = 0;
        int last_done = -1;

        #pragma omp parallel for
        for (size_t peak_id = 0; peak_id < peaks.size(); ++peak_id) {
            Peak3D& p = *peaks[peak_id];
            p.linkData(data);
            p.addUnitCell(cell, true);
            ++current_peak;

            Eigen::RowVector3i hkl = p.getIntegerMillerIndices();

            // try to find this reflection in the list of peaks, skip if found
            if (std::find(found_hkls.begin(), found_hkls.end(), hkl) != found_hkls.end() ) {
                continue;
            }

            // now we must add it, calculating shape from nearest peaks
             // K is outside the ellipsoid at PsptrPeak3D
            sptrPeak3D new_peak = averagePeaks(octree, p.getShape().center());
            //sptrPeak3D new_peak = p.averagePeaks(numor);

            if (!new_peak) {
                continue;
            }

            new_peak->linkData(data);
            new_peak->setSelected(true);
            new_peak->addUnitCell(cell, true);
            new_peak->setObserved(false);

            #pragma omp critical
            calculated_peaks.insert(new_peak);

            #pragma omp atomic
            ++done_peaks;
            int done = int(std::lround(done_peaks * 100.0 / peaks.size()));

            if ( done != last_done) {
                _handler->setProgress(done);
                last_done = done;
            }
        }
    }
    return calculated_peaks;
}

sptrPeak3D PeakPredictor::averagePeaks(const Octree& tree, const Eigen::Vector3d& center)
{
    // create the shape used for the neighbor search
    Eigen::Matrix3d search_metric;
    search_metric.setZero();
    search_metric(0,0) = 1.0 / _searchRadius / _searchRadius;
    search_metric(1,1) = search_metric(0,0);
    search_metric(2,2) = 1.0 / _frameRadius / _frameRadius;
    Ellipsoid search_shape(center, search_metric);

    // get neighbors
    auto&& neighbors = tree.getCollisions(search_shape);

    // too few neighbors
    if (neighbors.size() < size_t(_minimumNeighbors)) {
        return nullptr;
    }

    Eigen::Matrix3d covariance;
    covariance.setZero();

    for(auto p: neighbors) {
        covariance += p->inverseMetric();
    }
    covariance /= neighbors.size();


    // scale factor for shape
    Eigen::Vector3d scale;
    Eigen::Vector3d min(_minimumRadius, _minimumRadius, _minimumPeakDuration);

    for (auto i = 0; i < 3; ++i) {
        const double ratio = covariance(i,i) / min(i) / min(i);
        scale(i) = ratio > 1 ? 1.0 : 1.0 / std::sqrt(ratio);
    }

    for (auto i = 0; i < 3; ++i) {
        for (auto j = 0; j < 3; ++j) {
            covariance(i,j) *= scale(i)*scale(j);
        }
    }
  
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(covariance);
    auto vals = solver.eigenvalues();

    // something went wrong...
    if (vals.minCoeff() < 1e-3 || vals.maxCoeff() > 100*_minimumRadius) {
        return nullptr;
    }

    sptrPeak3D peak = std::make_shared<Peak3D>(Ellipsoid(center, covariance.inverse()));

    // An averaged peak is by definition not an observed peak but a calculated peak
    peak->setObserved(false);
    return peak;
}

} // end namespace nsx
