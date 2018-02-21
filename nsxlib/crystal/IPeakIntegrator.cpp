/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
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

#include "DataSet.h"
#include "Intensity.h"
#include "IPeakIntegrator.h"
#include "Logger.h"
#include "Peak3D.h"
#include "ProgressHandler.h"
#include "UtilsTypes.h"

namespace nsx {

IPeakIntegrator::IPeakIntegrator(): _meanBackground(), _integratedIntensity(), _rockingCurve()
{

}

IPeakIntegrator::~IPeakIntegrator()
{

}

Intensity IPeakIntegrator::meanBackground() const
{
    return _meanBackground;
}

Intensity IPeakIntegrator::integratedIntensity() const
{
    return _integratedIntensity;
}

const std::vector<Intensity>& IPeakIntegrator::rockingCurve() const
{
    return _rockingCurve;
}

void IPeakIntegrator::integrate(PeakList& peaks, sptrDataSet data, double peak_end, double bkg_begin, double bkg_end)
{
    sptrProgressHandler handler = nullptr;
    std::vector<sptrPeak3D> peak_list;
    const size_t num_peaks = peaks.size();
    peak_list.reserve(num_peaks);

    for (auto&& peak: peaks ) {
        // skip if peak does not belong to this dataset
        if (peak->data() != data) {
            continue;
        }
        peak_list.emplace_back(peak);
    }

    std::string status = "Integrating " + std::to_string(peak_list.size()) + " peaks...";
    nsx::info() << status;

    if (handler) {
        handler->setStatus(status.c_str());
        handler->setProgress(0);
    }

    size_t idx = 0;
    int num_frames_done = 0;

    std::map<sptrPeak3D, IntegrationRegion> regions;
    std::map<sptrPeak3D, bool> integrated;

    for (auto peak: peak_list) {
        regions.emplace(std::make_pair(peak, IntegrationRegion(peak, peak_end, bkg_begin, bkg_end)));
        integrated.emplace(std::make_pair(peak, false));
    }

    for (idx = 0; idx < data->nFrames(); ++idx ) {
        Eigen::MatrixXi current_frame, mask;
        current_frame = data->frame(idx);

        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));

        for (auto peak: peak_list) {
            regions[peak].updateMask(mask, idx);
        }

        for (auto& peak: peak_list) {
            bool result = regions[peak].advanceFrame(current_frame, mask, idx);

            // done reading peak data
            if (result && !integrated[peak]) {
                regions[peak].peakData().computeQs();              
                regions[peak].bkgData().computeQs();              
                try {
                    if (compute(peak, regions[peak])) {
                        peak->updateIntegration(*this);
                    } else {
                        peak->setSelected(false);
                    }
                } catch(std::exception& e) {
                    // integration failed...
                    nsx::info() << "integration failed: " << e.what();
                    peak->setSelected(false);
                }
                // free memory (important!!)
                regions[peak].reset();
                integrated[peak] = true;
            }                
        }

        if (handler) {
            ++num_frames_done;
            double progress = num_frames_done * 100.0 / data->nFrames();
            handler->setProgress(progress);
        }
    }
}

} // end namespace nsx
