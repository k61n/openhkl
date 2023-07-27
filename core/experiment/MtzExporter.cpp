//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/MtxExporter.cpp
//! @brief     Handles peak manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "MtzExporter.h"

#include "base/utils/Units.h"
#include "cmtzlib.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InterpolatedState.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/PeakMerger.h"
#include "mtzdata.h"

#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Geometry/Quaternion.h>

#include <functional>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace ohkl {
MtzExporter::MtzExporter(
    MergedPeakCollection* merged_data, sptrDataSet data, sptrUnitCell cell, bool merged,
    bool sum_intensities)
    : _merged_data(merged_data)
    , _ohkl_data(data)
    , _ohkl_cell(cell.get())
    , _merged(merged)
    , _sum_intensities(sum_intensities)
    , _mtz(nullptr)
{
    auto gonio = _ohkl_data->diffractometer()->sample().gonio(); // Sample 3-axis gonio
    // auto gonio = _ohkl_data->diffractometer()->detector()->gonio(); // Detector 2-axis gonio
    auto& instrument_states = _ohkl_data->instrumentStates();

    const auto& sample_gonio = _ohkl_data->diffractometer()->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    _omega_idx = -1, _phi_idx = -1, _chi_idx = -1;
    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const std::string axis_name = sample_gonio.axis(i).name();
        _omega_idx = axis_name == ohkl::ax_omega ? int(i) : _omega_idx;
        _chi_idx = axis_name == ohkl::ax_chi ? int(i) : _chi_idx;
        _phi_idx = axis_name == ohkl::ax_phi ? int(i) : _phi_idx;
    }
}

MtzExporter::~MtzExporter()
{
    // delete _mtz;
    CMtz::MtzFree(_mtz);
}

void MtzExporter::buildBatches(CMtz::MTZSET* mtz_set)
{
    int nframes = _ohkl_data->nFrames();

    CMtz::MTZBAT* batch;
    CMtz::MTZBAT* previous_batch;

    auto gonio = _ohkl_data->diffractometer()->sample().gonio(); // Sample 3-axis gonio
    auto& instrument_states = _ohkl_data->instrumentStates();
    const auto& sample_gonio = _ohkl_data->diffractometer()->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    Eigen::Vector3d axis1, axis2, axis3;
    axis1 = sample_gonio.axis(0).axis();
    if (n_sample_gonio_axes > 1)
        axis2 = sample_gonio.axis(1).axis();
    if (n_sample_gonio_axes > 2)
        axis3 = sample_gonio.axis(2).axis();

    const std::vector<std::vector<double>>& sample_states =
        _ohkl_data->diffractometer()->sampleStates;

    double fwhm =
        _ohkl_data->diffractometer()->source().selectedMonochromator().fullWidthHalfMaximum();

    for (std::size_t frame = 0; frame < nframes; ++frame) {
        batch = CMtz::MtzMallocBatch();

        if (frame == 0)
            _mtz->batch = batch;
        else
            previous_batch->next = batch;
        previous_batch = batch;

        std::string name = std::to_string(frame);
        // IMPORTANT - Aimless hash table can only take positive non-zero integer as a key!
        // We must start indexing from 1!
        batch->num = frame + 1;

        batch->ngonax = gonio.nAxes();
        for (int i = 0; i < gonio.nAxes(); i++)
            strncpy(batch->gonlab[i], gonio.axis(i).name().c_str(), 9);

        batch->iortyp = 0; // this values needs to be 0 according to mtz documentation

        for (int i = 0; i < 6; i++)
            batch->lbcell[i] = 0; // refinement flags

        batch->misflg = 0; // phixyz 0,1,2
        batch->jumpax = 0; // reciprocal axis close to rotation
        batch->ncryst = 0;
        batch->lcrflg = 0; // mosacity 0 = isotropic, 1 = anisotropic
        batch->ldtype = 2; // type of  data: 2d (1), 3d(2)
        batch->nbscal = 0; // number of batch scales & Bfactors 0 if unset)
        batch->lbmflg = 0; // flag for beam info alambd(0), delcor, divhd, divvd(1)
        batch->ndet = 1; // Number of detectors
        batch->nbsetid = mtz_set->setid; // id of this dataset

        // cell dimensions
        batch->cell[0] = _ohkl_cell->character().a;
        batch->cell[1] = _ohkl_cell->character().b;
        batch->cell[2] = _ohkl_cell->character().c;
        batch->cell[3] = _ohkl_cell->character().alpha / deg;
        batch->cell[4] = _ohkl_cell->character().beta / deg;
        batch->cell[5] = _ohkl_cell->character().gamma / deg;

        // Orientation Matrix
        Eigen::Quaterniond quat = instrument_states[0].sampleOrientationOffset;
        auto orientation = quat.toRotationMatrix();
        for (int i = 0; i < 9; ++i)
            batch->umat[i] = orientation(i % 3, i / 3); // om(x,y) or om(y,x) ?????

        // Missetting angles at beginning and end of oscillation
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                batch->phixyz[i][j] = 0;

        // Mosacity
        for (int i = 0; i < 12; i++)
            batch->crydat[i] = 0;

        // start and stop time
        batch->time1 = 0;
        batch->time2 = 0;

        // Batch scale, factor and temperature
        batch->bscale = 1;
        batch->bbfac = 1;
        batch->sdbfac = 1;

        for (int i = 0; i < 3; ++i) {
            batch->e1[i] = axis1[i];
            if (n_sample_gonio_axes > 1)
                batch->e2[i] = axis2[i];
            if (n_sample_gonio_axes > 2)
                batch->e3[i] = axis3[i];
        }

        // Rotation axis in lab frame
        batch->jsaxs = _omega_idx; // goniostat scan axis number
        for (int i = 0; i < 3; i++)
            batch->scanax[i] = sample_gonio.axis(_omega_idx).axis()[i];

        // Source (wave) vector
        Eigen::RowVector3d ni =
            _ohkl_data->diffractometer()->source().selectedMonochromator().ki().rowVector();
        ni.normalize();
        for (int i = 0; i < 3; i++) {
            batch->source[i] = ni[i]; // idealised
            batch->so[i] = ni[i]; // including tilt
        }

        // phi start and phi end angles
        batch->phistt = sample_states[frame][_omega_idx] / deg;
        // batch->phistt = start;
        if (frame < nframes - 1)
            batch->phiend = batch->phistt +
                (sample_states[frame + 1][_omega_idx] - sample_states[frame][_omega_idx]) / deg;
        else
            batch->phiend = sample_states[frame][_omega_idx] + sample_states[frame - 1][_omega_idx]
                - sample_states[frame - 2][_omega_idx];

        // datum values of goniostat axes
        for (int i = 0; i < 3; i++) batch->datum[i] = sample_states[frame][i] / deg;

        batch->phirange =
            (sample_states[0][_omega_idx] - sample_states[sample_states.size() - 1][_omega_idx]) / deg;

        batch->alambd = _ohkl_data->wavelength(); // wavelength

        // Dispersion
        batch->delamb = 0;
        batch->delcor = 0;

        // Beam divergence (FWHM)
        batch->divhd = fwhm;
        batch->divvd = fwhm;

        // xtal to detector distance
        batch->dx[0] = 0;
        batch->dx[1] = 0;

        // detector tilt angle
        batch->theta[0] = 0;
        batch->theta[1] = 0;

        batch->next = nullptr;
    }

    _mtz->n_orig_bat = nframes;
}

void MtzExporter::buildSymInfo()
{
    // Extracting SpaceGrp symbol, remove whitespaces
    std::string symbol = _ohkl_cell->spaceGroup().symbol();
    std::regex r("\\s+");
    symbol = std::regex_replace(symbol, r, "");

    SymOpList symops = _ohkl_cell->spaceGroup().groupElements();

    _mtz->mtzsymm.spcgrp = _ohkl_cell->spaceGroup().id();
    strncpy(_mtz->mtzsymm.spcgrpname, symbol.c_str(), symbol.size());
    _mtz->mtzsymm.nsym = symops.size();

    // Filling symmetry operation array
    // From what I can tell by example files this is just affineTransformation matix form SymOp
    // Phenix uses this to figure out the space group?
    int n = 0;
    for (auto& e : symops) {
        auto m = e.getMatrix();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++)
                _mtz->mtzsymm.sym[n][i][j] = m(i, j);
        }
        ++n;
    }

    _mtz->mtzsymm.nsymp = symops.size();
    ;
    _mtz->mtzsymm.symtyp = symbol.c_str()[0];
    strncpy(_mtz->mtzsymm.pgname, "PntGrName\0", 10);
    _mtz->mtzsymm.spg_confidence = _ohkl_cell->spaceGroup().bravaisType();
}

void MtzExporter::populateColumns(CMtz::MTZCOL** columns, int ncol)
{
    MergedPeakSet peaks = _merged_data->mergedPeakSet();

    int npeaks;
    if (_merged)
        npeaks = _merged_data->nUnique();
    else
        npeaks = _merged_data->totalSize();

    float adata[ncol];
    int irefl = 0;
    if (_merged) {
        for (const MergedPeak& peak : peaks) {
            const auto hkl = peak.index();
            Intensity intensity = peak.intensity();

            adata[0] = hkl.h();
            adata[1] = hkl.k();
            adata[2] = hkl.l();
            adata[3] = intensity.value();
            adata[4] = intensity.sigma();
            CMtz::ccp4_lwrefl(_mtz, adata, columns, ncol, irefl + 1);
            irefl++;
        }
    } else {
        for (auto& peak : peaks) {
            InstrumentStateList states = _ohkl_data->instrumentStates();
            for (auto unmerged_peak : peak.peaks()) {
                int m_isym = 1;
                const UnitCell& cell = *(unmerged_peak->unitCell());
                const ReciprocalVector& q = unmerged_peak->q();
                const MillerIndex hkl = unmerged_peak->hkl();
                double frame = unmerged_peak->shape().center()[2];
                int frame_int = std::floor(frame);
                const std::vector<std::vector<double>>& sample_states =
                    _ohkl_data->diffractometer()->sampleStates;

                Intensity intensity;
                if (_sum_intensities)
                    intensity = unmerged_peak->correctedSumIntensity();
                else
                    intensity = unmerged_peak->correctedProfileIntensity();
                auto state = InterpolatedState::interpolate(states, frame);
                double offset = sample_states[frame_int][_omega_idx] / deg;

                adata[0] = hkl.h();
                adata[1] = hkl.k();
                adata[2] = hkl.l();
                adata[3] = m_isym;
                adata[4] = std::round(frame);
                adata[5] = offset;
                adata[6] = intensity.value();
                adata[7] = intensity.sigma();
                CMtz::ccp4_lwrefl(_mtz, adata, columns, ncol, irefl + 1);
                irefl++;
            }
        }
    }

    _mtz->ncol_read = ncol;

    _mtz->nref = irefl;
    _mtz->nref_filein = irefl;
    }

bool MtzExporter::writeToFile(std::string filename)
{
    ohklLog(Level::Info, "MtzExporter::writeToFile");

    _mtz = CMtz::MtzMalloc(0, nullptr);
    CMtz::ccp4_lwtitl(_mtz, _ohkl_data->name().c_str(), 0);

    _mtz->refs_in_memory = 0; // Keep reflections in memory
    _mtz->fileout = CMtz::MtzOpenForWrite(filename.c_str());
    if (!_mtz->fileout)
        throw std::runtime_error("MtzExporter::writeToFile: can't open file " + filename);

    buildSymInfo();

    float cell[6];
    cell[0] = _ohkl_cell->character().a;
    cell[1] = _ohkl_cell->character().b;
    cell[2] = _ohkl_cell->character().c;
    cell[3] = _ohkl_cell->character().alpha / deg;
    cell[4] = _ohkl_cell->character().beta / deg;
    cell[5] = _ohkl_cell->character().gamma / deg;


    // Add base set and crystal explicitly
    CMtz::MTZXTAL* base_xtal = MtzAddXtal(_mtz, "HKL_base", "HKL_base", cell);
    CMtz::MTZSET* base_set = MtzAddDataset(_mtz, base_xtal, "HKL_base", 0.0);

    CMtz::MTZXTAL* xtal =
        MtzAddXtal(_mtz, _ohkl_cell->name().c_str(), _ohkl_data->name().c_str(), cell);
    CMtz::MTZSET* mtz_set =
        MtzAddDataset(_mtz, xtal, _ohkl_data->name().c_str(), _ohkl_data->wavelength());

    int ncol;
    if (_merged)
        ncol = 5;
    else
        ncol = 8;

    CMtz::MTZCOL* mtz_cols[ncol];

    int col = 0;
    // Columns belong to base set
    mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "H", "H");
    mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "K", "H");
    mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "L", "H");
    if (!_merged) { // only if we are processing unmerged data
        mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "M/ISYM", "Y");
        mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "BATCH", "B");
        mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "ROT", "R");
    }
    mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "I", "J");
    mtz_cols[col++] = CMtz::MtzAddColumn(_mtz, base_set, "SIGI", "Q");

    populateColumns(mtz_cols, ncol);
    CMtz::MtzSetSortOrder(_mtz, mtz_cols);

    buildBatches(mtz_set);

    // CMtz::ccp4_lhprt_adv(_mtz, 2);
    if (!CMtz::MtzPut(_mtz, " "))
        throw std::runtime_error("MtzExporter::writeToFile: Can't write to file " + filename);

    // CMtz::MtzFree(_mtz);

    return true;
}

} // ohkl namespace
