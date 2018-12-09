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

#include "BrillouinZone.h"
#include "DataSet.h"
#include "Intensity.h"
#include "IPeakIntegrator.h"
#include "Logger.h"
#include "Peak3D.h"
#include "UnitCell.h"


namespace nsx {

IPeakIntegrator::IPeakIntegrator(): _meanBackground(), _integratedIntensity(), _rockingCurve(), _handler(nullptr)
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

void IPeakIntegrator::integrate(PeakList peaks, sptrDataSet data, double peak_end, double bkg_begin, double bkg_end)
{
    // integrate only those peaks that belong to the specified dataset
    auto it = std::remove_if(peaks.begin(), peaks.end(), [&](const sptrPeak3D& peak) { return peak->data() != data;});
    peaks.erase(it, peaks.end());

    std::string status = "Integrating " + std::to_string(peaks.size()) + " peaks...";
    nsx::info() << status;

    if (_handler) {
        _handler->setStatus(status.c_str());
        _handler->setProgress(0);
    }

    size_t idx = 0;
    int num_frames_done = 0;

    std::map<sptrPeak3D, IntegrationRegion> regions;
    std::map<sptrPeak3D, bool> integrated;

    for (auto peak: peaks) {
        try {
            // IntegrationRegion constructor may throw (e.g. peak on boundary of image)
            regions.emplace(std::make_pair(peak, IntegrationRegion(peak, peak_end, bkg_begin, bkg_end)));
            integrated.emplace(std::make_pair(peak, false));
        } catch (...) {
            peak->setSelected(false);
            continue;
        }

        // ignore partials
        auto bb = regions[peak].peakBB();
        auto data = peak->data();
        auto lo = bb.lower();
        auto hi = bb.upper();

        if (lo[0] < 0 || lo[1] < 0 || lo[2] < 0) {
            peak->setSelected(false);
        }

        if (hi[0] >= data->nCols() || hi[1] >= data->nRows() || hi[2] >= data->nFrames()) {
            peak->setSelected(false);
        }
    }

    // only integrate the peaks with valid integration regions
    it = std::remove_if(peaks.begin(), peaks.end(), [&](const sptrPeak3D& p) { return regions.find(p) == regions.end(); });
    peaks.erase(it, peaks.end());

    for (idx = 0; idx < data->nFrames(); ++idx ) {
        Eigen::MatrixXd current_frame;
        Eigen::MatrixXi mask;
        current_frame = data->transformedFrame(idx);

        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));

        for (auto peak: peaks) {
            assert(peak != nullptr);
            assert(regions.find(peak) != regions.end());
            regions[peak].updateMask(mask, idx);
        }

        for (auto peak: peaks) {

            bool result = regions[peak].advanceFrame(current_frame, mask, idx);
            // this allows for partials at end of data
            result |= idx == data->nFrames()-1;

            // done reading peak data
            if (result && !integrated[peak]) {
                regions[peak].data().computeStandard();
                try {
                    if (compute(peak, regions[peak])) {
                        peak->updateIntegration(*this, peak_end, bkg_begin, bkg_end);
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

        if (_handler) {
            ++num_frames_done;
            double progress = num_frames_done * 100.0 / data->nFrames();
            _handler->setProgress(progress);
        }
    }
}

void IPeakIntegrator::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

} // end namespace nsx
