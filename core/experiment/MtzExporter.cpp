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

#include "cmtzlib.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/PeakMerger.h"
#include "mtzdata.h"

#include <functional>
#include <regex>
#include <string.h>
#include <string>
#include <vector>

namespace ohkl {
MtzExporter::MtzExporter(
    MergedPeakCollection* merged_data, sptrDataSet data, sptrUnitCell cell, bool merged,
    bool sum_intensities, std::string comment)
    : _merged_data(merged_data)
    , _ohkl_data(data)
    , _ohkl_cell(cell.get())
    , _merged(merged)
    , _sum_intensities(sum_intensities)
    , _comment(comment)
    , _mtz(nullptr)
    , _mtz_xtal(nullptr)
{
}

MtzExporter::~MtzExporter()
{
    if (_mtz != nullptr)
        delete _mtz;
    for (auto& e : _mtz_cols) {
        if (e != nullptr) {
            delete[] e->ref;
            delete e;
        }
    }
    _mtz_cols.clear();
}

void MtzExporter::buildMtz()
{
    _mtz = new CMtz::MTZ();

    strncpy(_mtz->title, "OpenHKLProjectExport\0", 21);

    // xtal ptr needs to be prepaired before calling lib method
    _mtz->xtal = new CMtz::MTZXTAL*[1];
    _mtz->xtal[0] = new CMtz::MTZXTAL();

    _mtz->refs_in_memory = 1; // Keep reflections in memory
    _mtz->nref_filein = 1;

    _mtz->xml = nullptr;
}

void MtzExporter::buildMNF()
{
    // we don't use this right now
    strncpy(_mtz->mnf.amnf, "", 0);
    _mtz->mnf.fmnf = 0;
}

void MtzExporter::buildBatches()
{
    // int nframes = _ohkl_data->nFrames();
    int nframes = 1;

    auto gonio = _ohkl_data->diffractometer()->sample().gonio(); // Sample 3-axis gonio
    // auto gonio = _ohkl_data->diffractometer()->detector()->gonio(); // Detector 2-axis gonio
    auto ubmatrix = _ohkl_cell->orientation();

    _mtz->batch = CMtz::MtzMallocBatch();

    CMtz::MTZBAT* batch = CMtz::MtzMallocBatch();
    for (std::size_t frame = 0; frame < nframes; ++frame) {
        if (frame == 0) {
            _mtz->batch = batch;
        }

        std::string name = std::to_string(frame);
        batch->num = frame;
        strncpy(batch->title, name.c_str(), 4);

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
        batch->jsaxs = 1; // goniostat scan axis number
        batch->nbscal = 0; // number of batch scales & Bfactors 0 if unset)
        batch->lbmflg = 0; // flag for beam info alambd(0), delcor, divhd, divvd(1)
        batch->ndet = 1; // Number of detectors
        batch->nbsetid = frame; // id of this dataset

        // cell dimensions
        batch->cell[0] = _ohkl_cell->character().a;
        batch->cell[1] = _ohkl_cell->character().b;
        batch->cell[2] = _ohkl_cell->character().c;
        batch->cell[3] = 180.0 / M_PI * _ohkl_cell->character().alpha;
        batch->cell[4] = 180.0 / M_PI * _ohkl_cell->character().beta;
        batch->cell[5] = 180.0 / M_PI * _ohkl_cell->character().gamma;

        // Orientaion Matrix
        for (int i = 0; i < 9; ++i)
            batch->umat[i] = ubmatrix(i % 3, i / 3); // om(x,y) or om(y,x) ?????

        // Missetting angles at beginning and end of oscillation
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                batch->phixyz[i][j] = 0;

        // Mosacity
        for (int i = 0; i < 12; i++)
            batch->crydat[i] = 0;

        // datum values of goniostat axes
        for (int i = 0; i < 3; i++)
            batch->datum[i] = 0;

        batch->phistt = 0; // relative to datum
        batch->phiend = 0; // rel;ative to datum

        // Rotation axis in lab frame
        for (int i = 0; i < 3; i++)
            batch->scanax[i] = 0;

        // start and stop time
        batch->time1 = 0;
        batch->time2 = 0;

        // Batch scale, factor and temperature
        batch->bscale = 1;
        batch->bbfac = 1;
        batch->sdbfac = 1;

        batch->phirange = 0; // Phi range

        // vector 1,2,3, source and idealied source vector
        for (int i = 0; i < 3; i++) {
            batch->e1[i] = 0;
            batch->e2[i] = 0;
            batch->e3[i] = 0;
            batch->source[i] = 0;
            batch->so[i] = 0;
        }

        batch->alambd = _ohkl_data->wavelength(); // wavelength

        // Dispersion
        batch->delamb = 0;
        batch->delcor = 0;

        // Beam divergence (FWHM)
        batch->divhd = 0.0;
        batch->divvd = 0.0;

        // xtal to detector distance
        batch->dx[0] = 0;
        batch->dx[1] = 0;

        // detector tilt angle
        batch->theta[0] = 0;
        batch->theta[1] = 0;

        if (frame < nframes - 1) {
            batch->next = CMtz::MtzMallocBatch();
            batch = batch->next;
        } else {
            batch->next = NULL; // Last batch must have null as next
        }
    }

    _mtz->n_orig_bat = nframes;
}

void MtzExporter::buildSyminfo()
{
    // Extracting SpaceGrp symbol
    // remove whitespaces
    // this doesnt seemd to be directly processed by phenix
    std::string symbol = _ohkl_cell->spaceGroup().symbol();
    std::regex r("\\s+");
    symbol = std::regex_replace(symbol, r, "");

    SymOpList symops = _ohkl_cell->spaceGroup().groupElements();

    /* Bulding symgrp */
    _mtz->mtzsymm.spcgrp = _ohkl_cell->spaceGroup().id();
    strncpy(_mtz->mtzsymm.spcgrpname, symbol.c_str(), symbol.size());
    _mtz->mtzsymm.nsym = symops.size();

    /*
        Filling symmetry operation array
        From what I can tell by example files this is just affineTransformation matix form SzmOp
        Phenix uses this to figure out the spacegrp ?
    */
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

void MtzExporter::buildXTAL()
{
    /* getting cell information */
    float cell[6];
    cell[0] = _ohkl_cell->character().a;
    cell[1] = _ohkl_cell->character().b;
    cell[2] = _ohkl_cell->character().c;
    cell[3] = 180.0 / M_PI * _ohkl_cell->character().alpha;
    cell[4] = 180.0 / M_PI * _ohkl_cell->character().beta;
    cell[5] = 180.0 / M_PI * _ohkl_cell->character().gamma;

    /* GENERATE XTAL STRUCTURE */
    _mtz_xtal = MtzAddXtal(_mtz, _ohkl_data->name().c_str(), _ohkl_cell->name().c_str(), cell);
}

void MtzExporter::buildMtzSet()
{
    if (!_mtz)
        throw std::runtime_error("MtzExporter::buildMtzSet invalid _mtz structure");

    if (!_mtz_xtal)
        throw std::runtime_error("MtzExporter::buildMtzSet invalid _mtz xtal");

    if (!_ohkl_data)
        throw std::runtime_error("MtzExporter::buildMtzSet invalid ohkl data");

    /* GENERATE MTZ SETS */
    std::string name = "DATASET01";
    auto ptr = MtzAddDataset(_mtz, _mtz_xtal, name.c_str(), _ohkl_data->wavelength());
    if (!ptr)
        throw std::runtime_error("MtzExporter::buildMtzSet unable to create dataset in mtz export");
}

CMtz::MTZCOL* MtzExporter::CreateMtzCol(
    std::string name, std::string label, int grp, int set_id, int active, int src)
{
    std::string grpname = _merged ? "MergedPeakData" : "UnmergedPeakData";

    CMtz::MTZCOL* ptr = nullptr;
    ptr = MtzAddColumn(_mtz, _mtz_xtal->set[set_id], name.c_str(), label.c_str());
    if (ptr != nullptr) {
        auto nPeaks = _merged_data->totalSize();
        strncpy(ptr->grpname, grpname.c_str(), grpname.size());
        ptr->active = active;
        ptr->source = src;
        ptr->min = nPeaks;
        ptr->max = nPeaks;
        ptr->grpposn = grp;
        ptr->ref = new float[nPeaks];
        _mtz_cols.emplace_back(ptr);
    }
    return ptr;
}

void MtzExporter::buildMtzCols()
{
    if (!_mtz)
        throw std::runtime_error("Error MtzExporter::buildMtzColData Invalid Mtz data structures");

    int grp = 0; // grp idx, counter variable
    MergedPeakSet peaks;
    peaks = _merged_data->mergedPeakSet();

    CreateMtzCol("H", "H", grp++, 0, 1, 0);
    CreateMtzCol("K", "H", grp++, 0, 1, 0);
    CreateMtzCol("L", "H", grp++, 0, 1, 0);
    if (!_merged) { // only if we are processing unmerged data
        CreateMtzCol("M/ISYM", "Y", grp++, 0, 1, 0);
        CreateMtzCol("BATCH", "B", grp++, 0, 1, 0);
    }
    CreateMtzCol("I", "J", grp++, 0, 1, 0);
    CreateMtzCol("SIGI", "Q", grp++, 0, 1, 0);


    int idx = 0;
    if (_merged) {
        for (const MergedPeak& peak : peaks) {
            const auto hkl = peak.index();
            Intensity intensity = peak.intensity();

            _mtz_cols[0]->ref[idx] = hkl.h();
            _mtz_cols[1]->ref[idx] = hkl.k();
            _mtz_cols[2]->ref[idx] = hkl.l();
            _mtz_cols[3]->ref[idx] = intensity.value();
            _mtz_cols[4]->ref[idx] = intensity.sigma();
            idx++;
        }
    } else {
        for (auto& peak : peaks) {
            for (auto unmerged_peak : peak.peaks()) {
                int m_isym = 1;
                const UnitCell& cell = *(unmerged_peak->unitCell());
                const ReciprocalVector& q = unmerged_peak->q();
                const MillerIndex hkl = unmerged_peak->hkl();
                Intensity intensity;
                if (_sum_intensities)
                    intensity = unmerged_peak->correctedSumIntensity();
                else
                    intensity = unmerged_peak->correctedProfileIntensity();

                _mtz_cols[0]->ref[idx] = hkl.h();
                _mtz_cols[1]->ref[idx] = hkl.k();
                _mtz_cols[2]->ref[idx] = hkl.l();
                _mtz_cols[3]->ref[idx] = 1;
                _mtz_cols[4]->ref[idx] = 0;
                _mtz_cols[5]->ref[idx] = intensity.value();
                _mtz_cols[6]->ref[idx] = intensity.sigma();
                idx++;
            }
        }
    }

    _mtz->ncol_read = _mtz_cols.size();

    _mtz->nref = idx;
    _mtz->nref_filein = idx;

    // Sort order
    _mtz->order[0] = _mtz_cols.at(0);
    _mtz->order[1] = _mtz_cols.at(1);
    _mtz->order[2] = _mtz_cols.at(2);
    _mtz->order[3] = _mtz_cols.at(3);
    _mtz->order[4] = _mtz_cols.at(4);
}

void MtzExporter::buildMtzData()
{
    ohklLog(Level::Debug, "Building Mtz data structure'");
    buildMtz();
    buildSyminfo();
    buildXTAL();
    buildMtzSet();
    buildMtzCols();
    buildMNF();
    buildBatches();
    buildHistory();

    MtzAddHistory(_mtz, (const char(*)[80])_comment.c_str(), 1);
}

void MtzExporter::buildHistory()
{
    for (auto& e : _history)
        MtzAddHistory(_mtz, (const char(*)[80])e.c_str(), 1);
}

void MtzExporter::addHistory(std::string line)
{
    _history.push_back(line);
}

bool MtzExporter::exportToFile(std::string filename)
{
    ohklLog(Level::Debug, "Export OpenHKL project to Mtz file ... '");

    if (!_mtz)
        buildMtzData();

    // Print out mtz data structure
    // ccp4_lhprt(_mtz, 4);
    // ccp4_lhprt_adv(_mtz, 4);

    /* saving mtz file */
    if (CMtz::MtzPut(_mtz, filename.c_str()) == 1)
        ohklLog(Level::Info, "Project has been succesfully exported to '" + filename + "'");
    else {
        ohklLog(Level::Error, "Project export to '" + filename + " failed '");
        return false;
    }

    return true;
}

} // ohkl namespace
