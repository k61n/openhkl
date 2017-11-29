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
#include "DetectorEvent.h"
#include "Diffractometer.h"
#include "GeometryTypes.h"
#include "Gonio.h"
#include "InstrumentState.h"
#include "Octree.h"
#include "Peak3D.h"
#include "PeakPredictor.h"
#include "ProgressHandler.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "SpaceGroup.h"
#include "Source.h"
#include "UnitCell.h"

namespace nsx {

PeakPredictor::PeakPredictor(sptrDataSet data):
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
    _handler(nullptr),
    _data(data)
{

}

PeakSet PeakPredictor::predictPeaks(bool keepObserved, const PeakSet& reference_peaks)
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

    auto& mono = _data->getDiffractometer()->getSource()->getSelectedMonochromator();
    const double wavelength = mono.getWavelength();
    PeakSet calculated_peaks;    

    auto sample = _data->getDiffractometer()->getSample();
    unsigned int ncrystals = static_cast<unsigned int>(sample->getNCrystals());

    for (unsigned int i = 0; i < ncrystals; ++i) {
        std::set<Eigen::RowVector3i, compare_fn> found_hkls;
        PeakSet found_peaks;
        SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
        auto cell = sample->getUnitCell(i);
        auto UB = cell->reciprocalBasis();

        for (auto&& peak: reference_peaks) {
            if (peak->data() != _data || peak->getActiveUnitCell() != cell) {
                continue;
            }
            found_peaks.insert(peak);
            Eigen::RowVector3i hkl = cell->getIntegerMillerIndices(peak->getQ());
            found_hkls.insert(hkl);
        }

        _handler->setStatus("Calculating peak locations...");

        //auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
        auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInShell(_dmin, _dmax, wavelength);

        // todo: clean up DataSet interface for predicted peaks
        std::vector<Eigen::RowVector3d> hkls_double;

        for (auto&& hkl: predicted_hkls) {
            Eigen::RowVector3i int_hkl(std::lround(hkl(0)), std::lround(hkl(1)), std::lround(hkl(2)));

            // if we keep reference peaks, check whether this hkl is part of reference set
            if (keepObserved && found_hkls.find(int_hkl) != found_hkls.end()) {
                continue;
            }
            
            hkls_double.emplace_back(hkl.cast<double>());
        }

        predicted_peaks += predicted_hkls.size();
        PeakList peaks = predictPeaks(hkls_double, UB);
        int current_peak = 0;

        _handler->setStatus("Building set of previously found peaks...");

        Eigen::Vector3d lb = {0.0, 0.0, 0.0};
        Eigen::Vector3d ub = {double(_data->getNCols()), double(_data->getNRows()), double(_data->getNFrames())};
        auto&& octree = Octree(lb, ub);

        octree.setMaxDepth(4);
        octree.setMaxStorage(50);

        _handler->log("Building peak octree...");

        for (sptrPeak3D p: found_peaks) {
            // reflection doesn't belong to current crystal
            if (cell != p->getActiveUnitCell()) {
                continue;
            }

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
            p.addUnitCell(cell, true);
            ++current_peak;

            auto q = p.getQ();
            Eigen::RowVector3i hkl = cell->getIntegerMillerIndices(q);

            // now we must add it, calculating shape from nearest peaks
             // K is outside the ellipsoid at PsptrPeak3D
            sptrPeak3D new_peak = averagePeaks(octree, p.getShape().center());
            //sptrPeak3D new_peak = p.averagePeaks(numor);

            if (!new_peak) {
                continue;
            }

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

    sptrPeak3D peak = std::make_shared<Peak3D>(_data, Ellipsoid(center, covariance.inverse()));

    // An averaged peak is by definition not an observed peak but a calculated peak
    peak->setObserved(false);
    return peak;
}

PeakList PeakPredictor::predictPeaks(const std::vector<Eigen::RowVector3d>& hkls, const Eigen::Matrix3d& BU)
{
    std::vector<Eigen::RowVector3d> qs;
    PeakList peaks;

    for (auto hkl: hkls) {
        qs.emplace_back(hkl*BU);
    }

    std::vector<DetectorEvent> events = getEvents(qs);
 
    for (auto event: events) {
        Eigen::Vector3d p = event.coordinates();
        sptrPeak3D peak(new Peak3D(_data));
        // this sets the center of the ellipse with a dummy value for radius
        peak->setShape(Ellipsoid(p, 1.0));
        peaks.emplace_back(peak);
    }
    return peaks;
}

std::vector<DetectorEvent> PeakPredictor::getEvents(const std::vector<Eigen::RowVector3d>& qs) const
{
    std::vector<DetectorEvent> events;
    unsigned int scanSize = _data->getNFrames();

    auto diffractometer = _data->getDiffractometer();
    auto detector = diffractometer->getDetector();
    auto& mono = diffractometer->getSource()->getSelectedMonochromator();

    const Eigen::RowVector3d ki = mono.getKi().transpose();
    std::vector<Eigen::Matrix3d> rotMatrices;
    rotMatrices.reserve(scanSize);
    auto gonio = diffractometer->getSample()->getGonio();
    double wavelength_2 = -0.5 * mono.getWavelength();

    for (unsigned int s=0; s<scanSize; ++s) {
        auto state = _data->getInterpolatedState(s);
        rotMatrices.push_back(gonio->getHomMatrix(state.sample).rotation().transpose());
    } 

    for (const Eigen::RowVector3d& q: qs) {
        bool sign = (q*rotMatrices[0] + ki).squaredNorm() > ki.squaredNorm();

        for (int i = 1; i < scanSize; ++i) {
            const Eigen::RowVector3d kf = q*rotMatrices[i] + ki;
            const bool new_sign = kf.squaredNorm() > ki.squaredNorm();

            if (sign != new_sign) {
                sign = new_sign;

                const Eigen::RowVector3d kf0 = q*rotMatrices[i-1] + ki;
                const Eigen::RowVector3d kf1 = q*rotMatrices[i] + ki;
                //const Eigen::RowVector3d dkf = kf1-kf0;
                const Eigen::RowVector3d dkf = q*(rotMatrices[i]-rotMatrices[i-1]);
        
                const double a = dkf.squaredNorm();
                const double b = 2 * kf0.dot(dkf);
                const double c = kf0.squaredNorm() - ki.squaredNorm();
                const double discr = b*b - 4*a*c;
        
                double t = 0.5;
                const int max_count = 100;
                Eigen::RowVector3d kf;
                
                for (int c = 0; c < max_count; ++c) {
                    kf = (1-t)*kf0 + t*kf1;
                    const double f = kf.squaredNorm() - ki.squaredNorm();
                    
                    if (std::fabs(f) < 1e-10) {
                        break;
                    }
                    const double df = 2*dkf.dot(kf);
                    t -= f/df;
                }
        
                if (c == max_count || t < 0.0 || t > 1.0) {
                    continue;
                }
        
                t += i-1;
                const InstrumentState& state = _data->getInterpolatedState(t);
        
                //const ComponentState& dis = state.detector;
                double px,py;
                // If hit detector, new peak
                //const ComponentState& cs=state.sample;
                Eigen::Vector3d from = diffractometer->getSample()->getPosition(state.sample);
        
                double time;
                bool accept = diffractometer->getDetector()->receiveKf(px,py,kf,from,time,state.detector);
        
                if (accept) {
                    events.emplace_back(_data, px, py, t);
                }
            }
        }        
    }
    return events;
}


} // end namespace nsx
