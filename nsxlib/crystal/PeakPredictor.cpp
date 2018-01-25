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
#include "MillerIndex.h"
#include "Octree.h"
#include "Peak3D.h"
#include "PeakPredictor.h"
#include "ProgressHandler.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "SpaceGroup.h"
#include "Source.h"
#include "UnitCell.h"

#include <map>

namespace nsx {

PeakPredictor::PeakPredictor(sptrDataSet data):
    _dmin(2.0),
    _dmax(50.0),   
    _Isigma(2.0),
    _minimumNeighbors(10),
    _handler(nullptr),
    _data(data)
{
}

PeakList PeakPredictor::predictPeaks(bool keepObserved, const PeakList& reference_peaks)
{
    int predicted_peaks = 0;

    auto& mono = _data->diffractometer()->getSource()->getSelectedMonochromator();
    const double wavelength = mono.getWavelength();
    PeakList calculated_peaks;

    auto sample = _data->diffractometer()->getSample();
    unsigned int ncrystals = static_cast<unsigned int>(sample->getNCrystals());

    for (unsigned int i = 0; i < ncrystals; ++i) {
        std::set<MillerIndex> found_hkls;
        auto cell = sample->unitCell(i);
        auto UB = cell->reciprocalBasis();        
        PeakList peaks_to_use;

        _handler->setStatus("Building set of previously found peaks...");
        for (auto&& peak: reference_peaks) {

            // ignore deselected and masked peaks
            if (!peak->isSelected()) {
                continue;
            }

            if (peak->data() != _data || peak->activeUnitCell() != cell) {
                continue;
            }            
            
            found_hkls.insert(cell->getIntegerMillerIndices(peak->getQ()));

            Intensity inten = peak->getCorrectedIntensity();

            // peak must have minimum I / sigma ratio
            if (inten.value() / inten.sigma() < _Isigma) {
                continue;
            }

            try {
                auto old_shape = peak->getShape().metric();
                auto q_shape = peak->qShape();
                auto new_shape = toDetectorSpace(q_shape).metric();
                double error = (new_shape-old_shape).norm() / old_shape.norm();

                // only makes contribution if conversion is consistent
                if (error < 0.1) {
                    peaks_to_use.add(peak);
                }
            } catch(...) {
                // could not convert back and forth to q-space, so skip
            }    
        }

        Eigen::Matrix3d qshape = averageQShape(peaks_to_use);

        _handler->setStatus("Calculating peak locations...");

        auto predicted_hkls = sample->unitCell(i)->generateReflectionsInShell(_dmin, _dmax, wavelength);

        // todo: clean up DataSet interface for predicted peaks
        std::vector<MillerIndex> hkls_keep;

        for (auto&& idx: predicted_hkls) {
            // if we keep reference peaks, check whether this hkl is part of reference set
            if (keepObserved && found_hkls.find(idx) != found_hkls.end()) {
                continue;
            }            
            hkls_keep.emplace_back(idx);
        }

        predicted_peaks += predicted_hkls.size();
        PeakList peaks = predictPeaks(hkls_keep, UB);
        int current_peak = 0;
  
        _handler->setStatus("Adding calculated peaks...");

        int done_peaks = 0;
        int last_done = -1;

        #pragma omp parallel for
        for (size_t peak_id = 0; peak_id < peaks.size(); ++peak_id) {
            sptrPeak3D p = peaks[peak_id];
            p->addUnitCell(cell, true);
            p->setObserved(false);
            p->setSelected(true);

            #pragma omp atomic
            ++current_peak;
            auto q = p->getQ();

            // now we must add it, calculating shape from nearest peaks
             // K is outside the ellipsoid at PsptrPeak3D

            try {
                Ellipsoid shape(p->getShape().center(), toDetectorSpace(Ellipsoid(q.rowVector(), qshape)).metric());
                p->setShape(shape);           
            } catch (...) {
                continue;
            }

            #pragma omp critical
            calculated_peaks.add(p);


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

Eigen::Matrix3d PeakPredictor::averageQShape(const PeakList& peaks)
{
    Eigen::Matrix3d covariance;
    covariance.setZero();
    double total_intensity = 0.0;

    for(const auto& p: peaks) {
        const double I = p->getCorrectedIntensity().value();
        try {
            covariance += I*p->qShape().inverseMetric();
            total_intensity += I;
        } catch (...) {
            // couldn't get q shape...
        }
    }

    // sanity check
    if (total_intensity < 1.0) {
        throw std::runtime_error("Could not find Q shape: too few valid peaks");
    }

    covariance /= total_intensity;
    return covariance.inverse();
}



PeakList PeakPredictor::predictPeaks(const std::vector<MillerIndex>& hkls, const Eigen::Matrix3d& BU)
{
    std::vector<ReciprocalVector> qs;
    PeakList peaks;

    for (auto idx: hkls) {
        qs.emplace_back(idx.rowVector().cast<double>()*BU);
    }

    std::vector<DirectVector> events = getEvents(qs);
 
    for (auto event: events) {
        sptrPeak3D peak(new Peak3D(_data));
        // this sets the center of the ellipse with a dummy value for radius
        peak->setShape(Ellipsoid(event.vector(), 1.0));
        peaks.add(peak);
    }
    return peaks;
}

std::vector<DirectVector> PeakPredictor::getEvents(const std::vector<ReciprocalVector>& sample_qs) const
{
    std::vector<DirectVector> events;
    unsigned int scanSize = _data->nFrames();

    std::vector<Eigen::RowVector3d> ki;
    ki.reserve(scanSize);

    std::vector<Eigen::Matrix3d> sample_to_lab;
    sample_to_lab.reserve(scanSize);
    auto diffractometer = _data->diffractometer();
    
    for (unsigned int s = 0; s < scanSize; ++s) {
        auto state = _data->interpolatedState(s);
        sample_to_lab.push_back(state.sampleOrientation().transpose());
        ki.push_back(state.ki().rowVector());
    } 

    for (const ReciprocalVector& sample_q : sample_qs) {

        const Eigen::RowVector3d& q_vect = sample_q.rowVector();

        bool sign = (q_vect*sample_to_lab[0] + ki[0]).squaredNorm() > ki[0].squaredNorm();

        for (size_t i = 1; i < scanSize; ++i) {
            const Eigen::RowVector3d kf = q_vect*sample_to_lab[i] + ki[i];
            const bool new_sign = kf.squaredNorm() > ki[i].squaredNorm();

            if (sign != new_sign) {
                sign = new_sign;

                const Eigen::RowVector3d kf0 = q_vect*sample_to_lab[i-1] + ki[i-1];
                const Eigen::RowVector3d kf1 = q_vect*sample_to_lab[i] + ki[i];
                const Eigen::RowVector3d dkf = q_vect*(sample_to_lab[i]-sample_to_lab[i-1]);
        
                double t = 0.5;
                const int max_count = 100;
                Eigen::RowVector3d kf;
                int c;
                
                for (c = 0; c < max_count; ++c) {
                    kf = (1-t)*kf0 + t*kf1;
                    auto ki_interp = (1-t)*ki[i-1] + t*ki[i];
                    const double f = kf.squaredNorm() - ki_interp.squaredNorm();
                    
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
                const InstrumentState& state = _data->interpolatedState(t);

                // transform back to detector
  
  
                //const ComponentState& dis = state.detector;
                double px,py;
                // If hit detector, new peak
                //const ComponentState& cs=state.sample;
        
                double time;
                auto detector = _data->diffractometer()->getDetector();
                bool accept = detector->receiveKf(px, py,DirectVector((kf*state.detectorOrientation).transpose()), DirectVector(state.samplePosition),time);

                if (accept) {
                    events.emplace_back(px, py, t);
                }
            }
        }        
    }
    return events;
}

Ellipsoid PeakPredictor::toDetectorSpace(const Ellipsoid& qshape) const
{
    const Eigen::Vector3d q = qshape.center();
    const Eigen::Matrix3d A = qshape.metric();

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(A);
    const Eigen::Matrix3d U = solver.eigenvectors();
    const Eigen::Vector3d l = solver.eigenvalues();

    std::vector<ReciprocalVector> qs;
    qs.push_back(ReciprocalVector(q));
    
    for (int i = 0; i < 3; ++i) {
        const double s = std::sqrt(1.0 / l(i));
        qs.push_back(ReciprocalVector(q+s*U.col(i)));
        qs.push_back(ReciprocalVector(q-s*U.col(i)));
    }
    auto evs = getEvents(qs);
    // something bad happened
    if (evs.size() != qs.size()) {
        throw std::runtime_error("could not transform ellipse from q space to detector space");
    }

    Eigen::Matrix3d delta;
    const Eigen::Vector3d& p0 = evs[0].vector();

    for (auto i = 0; i < 3; ++i) {
        const double s = std::sqrt(1.0 / l(i));
        const Eigen::Vector3d& p1 = evs[1+2*i].vector();
        const Eigen::Vector3d& p2 = evs[2+2*i].vector();
        delta.col(i) = 0.5 * (p1-p2) / s;
    }

    // approximate linear transformation detector space to q space
    const Eigen::Matrix3d BI = U * delta.inverse();

    return Ellipsoid(p0, BI.transpose()*A*BI);
}

} // end namespace nsx
