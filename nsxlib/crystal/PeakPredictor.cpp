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

#include "PeakPredictor.h"
#include "../data/IData.h"
#include "../geometry/NDTree.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"


using SX::Data::DataSet;

namespace SX {
namespace Crystal {

void PeakPredictor::addPredictedPeaks(std::shared_ptr<DataSet> data)
{
    using SX::Instrument::Sample;
    using Octree = SX::Geometry::NDTree<double, 3>;

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
    std::vector<sptrPeak3D> calculated_peaks;

    std::shared_ptr<Sample> sample = data->getDiffractometer()->getSample();
    unsigned int ncrystals = static_cast<unsigned int>(sample->getNCrystals());

    for (unsigned int i = 0; i < ncrystals; ++i) {
        SX::Crystal::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
        auto cell = sample->getUnitCell(i);
        auto UB = cell->getReciprocalStandardM();

        _handler->setStatus("Calculating peak locations...");

        //auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
        auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInShell(_dmin, _dmax, wavelength);

        predicted_peaks += predicted_hkls.size();

        std::vector<SX::Crystal::PeakCalc> peaks = data->hasPeaks(predicted_hkls, UB);
        calculated_peaks.reserve(peaks.size());

        int current_peak = 0;

        _handler->setStatus("Building set of previously found peaks...");

        std::set<sptrPeak3D> found_peaks = data->getPeaks();
        std::set<Eigen::RowVector3i, compare_fn> found_hkls;


        Eigen::Vector3d lb = {0.0, 0.0, 0.0};
        Eigen::Vector3d ub = {double(data->getNCols()), double(data->getNRows()), double(data->getNFrames())};
        auto&& octree = Octree(lb, ub);

        octree.setMaxDepth(4);
        octree.setMaxStorage(50);

        _handler->log("Building peak octree...");

        for (sptrPeak3D p: found_peaks) {
            found_hkls.insert(p->getIntegerMillerIndices());

            if (!p->isSelected() || p->isMasked()) {
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
            PeakCalc& p = peaks[peak_id];
            ++current_peak;

            Eigen::RowVector3i hkl(int(std::lround(p._h)), int(std::lround(p._k)), int(std::lround(p._l)));

            // try to find this reflection in the list of peaks, skip if found
            if (std::find(found_hkls.begin(), found_hkls.end(), hkl) != found_hkls.end() ) {
                continue;
            }

            // now we must add it, calculating shape from nearest peaks
             // K is outside the ellipsoid at PsptrPeak3D
            sptrPeak3D new_peak = p.averagePeaks(octree, _searchRadius);
            //sptrPeak3D new_peak = p.averagePeaks(numor);

            if (!new_peak) {
                continue;
            }

            new_peak->linkData(data);
            new_peak->setSelected(true);
            new_peak->addUnitCell(cell, true);
            new_peak->setObserved(false);

            #pragma omp critical
            calculated_peaks.push_back(new_peak);

            #pragma omp atomic
            ++done_peaks;
            int done = int(std::lround(done_peaks * 100.0 / peaks.size()));

            if ( done != last_done) {
                _handler->setProgress(done);
                last_done = done;
            }
        }
    }
    for (sptrPeak3D peak: calculated_peaks) {
        data->addPeak(peak);
    }
    //qDebug() << "Integrating calculated peaks.";
    data->integratePeaks(_peakScale, _bkgScale, false, _handler);
}

} // namespace Crystal
} // namespace SX
