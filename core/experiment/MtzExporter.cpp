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

#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/PeakMerger.h"

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
    , _mtz_data(nullptr)
    , _mtz_xtal(nullptr)
{
    _mtz_sets.clear();
    _mtz_cols.clear();
}

MtzExporter::~MtzExporter()
{
    if (_mtz_data != nullptr)
        delete _mtz_data;
    for (auto& e : _mtz_cols) {
        if (e != nullptr) {
            delete[] e->ref; // cleans up ref array in each col
            delete e;
        }
    }
    _mtz_cols.clear();

    // CMtz::MtzFreeBatch(_mtz_batch);
}

void MtzExporter::buildMtz()
{
    /* top level structure */
    _mtz_data = new CMtz::MTZ();

    strncpy(_mtz_data->title, "OpenHKLProjectExport\0", 21);

    /* xtal ptr needs to be prepaired before calling lib method */
    //_mtz_data->nxtal = 1; // the system will keep this updated automatically
    _mtz_data->xtal = new CMtz::MTZXTAL*[1]; // but we need to crate this structure or we will
                                             // encounter crashes along the way
    _mtz_data->xtal[0] = new CMtz::MTZXTAL();

    // Keep reflections in memory
    _mtz_data->refs_in_memory = 1;
    _mtz_data->nref_filein = 1;

    /*
        This is an additional place to store any kind of information
        There are no rules to this at all
        At first we keep ignoring it. maybe later some ohkl information ?
    */
    _mtz_data->xml = nullptr;
}

void MtzExporter::buildMNF()
{
    // we don't use this right now
    strncpy(_mtz_data->mnf.amnf, "", 0);
    _mtz_data->mnf.fmnf = 0;
}

void MtzExporter::buildBatch()
{
    /*
     *  Note:
     *  Not every field is modified - only as much as it works
     *
     */

    /* Retrieving ohkl data */
    // Sample gonio
    auto gonio = _ohkl_data->diffractometer()->sample().gonio(); // three axis

    // Detector gonio
    // auto gonio = _ohkl_data->diffractometer()->detector()->gonio(); // two axis

    auto omatrix = _ohkl_cell->orientation();

    /* Building Mtz Batch */
    _mtz_data->batch = CMtz::MtzMallocBatch();

    // We use one batch for the start
    _mtz_data->batch->num = 1;

    // labels and names
    strncpy(_mtz_data->batch->title, "BATCH01\0", 8); // later maybe we want more batches ?

    /* Getting axes Information */
    _mtz_data->batch->ngonax = gonio.nAxes();
    for (int i = 0; i < gonio.nAxes(); i++)
        strncpy(_mtz_data->batch->gonlab[i], gonio.axis(i).name().c_str(), 9);

    // this values needs to be 0 according to documentation of mtz file format
    _mtz_data->batch->iortyp = 0;

    // refinement flags .. needs documentation
    for (int i = 0; i < 6; i++)
        _mtz_data->batch->lbcell[i] = 0;

    // phixyz 0,1,2
    _mtz_data->batch->misflg = 0;

    // reciprocal axis closes to rotation
    _mtz_data->batch->jumpax = 0;

    // crystal number
    // to which this batch is refering to .. I assume
    // only one crystal -> one batch -> at least for now
    _mtz_data->batch->ncryst = 1;

    // mosacity 0 = isotropic, 1 = anisotropic
    _mtz_data->batch->lcrflg = 0;

    // type of  data: 2d (1), 3d(2)
    _mtz_data->batch->ldtype = 2; //?????

    /**< goniostat scan axis number */
    _mtz_data->batch->jsaxs = 1;

    /**< number of batch scales & Bfactors 0 if unset) */
    _mtz_data->batch->nbscal = 0;

    // flag for beam info
    // alambd(0), delcor, divhd, divvd(1)
    _mtz_data->batch->lbmflg = 0;

    /* Number of detectors - stays at 1 for then start */
    _mtz_data->batch->ndet = 1;

    /* id of this dataset */
    _mtz_data->batch->nbsetid = 1;

    // cell dimensions
    // this information has been written twice to the file then
    // in older mtz file version there was a third instance of cell information
    // but this seemed to have been removed
    _mtz_data->batch->cell[0] = _ohkl_cell->character().a;
    _mtz_data->batch->cell[1] = _ohkl_cell->character().b;
    _mtz_data->batch->cell[2] = _ohkl_cell->character().c;
    _mtz_data->batch->cell[3] = 180.0 / M_PI * _ohkl_cell->character().alpha;
    _mtz_data->batch->cell[4] = 180.0 / M_PI * _ohkl_cell->character().beta;
    _mtz_data->batch->cell[5] = 180.0 / M_PI * _ohkl_cell->character().gamma;

    /* Writing Orientaion Matrix */
    for (int i = 0; i < 9; ++i)
        _mtz_data->batch->umat[i] = omatrix(i % 3, i / 3); // om(x,y) or om(y,x) ?????

    /* Misseeting angles */
    // we keep this at zero for the beginning
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 3; j++)
            _mtz_data->batch->phixyz[i][j] = 0;

    /* Mosacity */
    // keeps this at zero for now
    for (int i = 0; i < 12; i++)
        _mtz_data->batch->crydat[i] = 0;

    /* datum values of goniostat axes */
    for (int i = 0; i < 3; i++)
        _mtz_data->batch->datum[i] = 0;

    _mtz_data->batch->phistt = 0; // relative to datum
    _mtz_data->batch->phiend = 0; // rel;ative to datum

    /* Rotation axis in lab frame */
    for (int i = 0; i < 3; i++)
        _mtz_data->batch->scanax[i] = 0;

    /* start and stop time */
    _mtz_data->batch->time1 = 0;
    _mtz_data->batch->time2 = 0;

    /* Batch scale, factor and temperature */
    _mtz_data->batch->bscale = 1;
    _mtz_data->batch->bbfac = 1;
    _mtz_data->batch->sdbfac = 1;

    /* Phi range */
    _mtz_data->batch->phirange = 0;

    /* vector 1,2,3, source and idealied source vector */
    // keep this at zero
    for (int i = 0; i < 3; i++) {
        _mtz_data->batch->e1[i] = 0;
        _mtz_data->batch->e2[i] = 0;
        _mtz_data->batch->e3[i] = 0;
        _mtz_data->batch->source[i] = 0;
        _mtz_data->batch->so[i] = 0;
    }

    /* Setting wavelength */
    _mtz_data->batch->alambd = _ohkl_data->wavelength();

    /* Dispersion */
    _mtz_data->batch->delamb = 0;
    _mtz_data->batch->delcor = 0;

    /* Beam divergence */
    _mtz_data->batch->divhd = 0.0; // FWHM
    _mtz_data->batch->divvd = 0.0; // FWHM

    /* xtal to detector distance */
    _mtz_data->batch->dx[0] = 0;
    _mtz_data->batch->dx[1] = 0;

    /* detector tilt angle */
    _mtz_data->batch->theta[0] = 0;
    _mtz_data->batch->theta[1] = 0;

    // set next batch ptr to nullptr
    // last node needs as always be set to nullptr
    _mtz_data->batch->next = NULL;

    // only for one batch for now
    _mtz_data->n_orig_bat = 1;
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
    _mtz_data->mtzsymm.spcgrp = _ohkl_cell->spaceGroup().id();
    strncpy(_mtz_data->mtzsymm.spcgrpname, symbol.c_str(), symbol.size());
    _mtz_data->mtzsymm.nsym = symops.size();

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
                _mtz_data->mtzsymm.sym[n][i][j] = m(i, j);
        }
        ++n;
    }

    _mtz_data->mtzsymm.nsymp = symops.size();
    ;
    _mtz_data->mtzsymm.symtyp = symbol.c_str()[0];
    strncpy(_mtz_data->mtzsymm.pgname, "PntGrName\0", 10);
    _mtz_data->mtzsymm.spg_confidence = _ohkl_cell->spaceGroup().bravaisType();
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
    _mtz_xtal = MtzAddXtal(_mtz_data, _ohkl_data->name().c_str(), _ohkl_cell->name().c_str(), cell);
}

void MtzExporter::buildMtzSet()
{
    if (!_mtz_data)
        throw std::runtime_error("MtzExporter::buildMtzSet invalid _mtz structure");

    if (!_mtz_xtal)
        throw std::runtime_error("MtzExporter::buildMtzSet invalid _mtz xtal");

    if (!_ohkl_data)
        throw std::runtime_error("MtzExporter::buildMtzSet invalid ohkl data");

    /* GENERATE MTZ SETS */
    std::string name = "DATASET01";
    auto ptr = MtzAddDataset(_mtz_data, _mtz_xtal, name.c_str(), _ohkl_data->wavelength());
    if (!ptr)
        throw std::runtime_error("MtzExporter::buildMtzSet unable to create dataset in mtz export");
}

CMtz::MTZCOL* MtzExporter::CreateMtzCol(
    std::string name, std::string label, int grp, int set_id, int active, int src)
{
    std::string grpname = _merged ? "MergedPeakData" : "UnmergedPeakData";

    CMtz::MTZCOL* ptr = nullptr;
    ptr = MtzAddColumn(_mtz_data, _mtz_xtal->set[set_id], name.c_str(), label.c_str());
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
    if (!_mtz_data)
        throw std::runtime_error("Error MtzExporter::buildMtzColData Invalid Mtz data structures");

    int grp = 0; // grp idx, counter variable
    MergedPeakSet peaks;
    peaks = _merged_data->mergedPeakSet();

    /*
        This part needs to be extended later
        for this time being we only save the following information from selected dataset
            H, K, L, IMEAN, SIGMA
            for unmerged Data als Frame numbers will be included (wished by a.ostermann)
        as individual Cols (5/6 in total)
        Names and Labels are fixed and cannot be changed freely -> documentation

        see https://www.ccp4.ac.uk/html/mtzformat.html

        * CREATING MTZ DATA COLS

        CreateMtzCol(
            NAME
            LABEL
            GRP_ID
            SET_ID
            ACTIVE
            SRC
    */
    CreateMtzCol("H", "H", grp++, 0, 1, 0);
    CreateMtzCol("K", "H", grp++, 0, 1, 0);
    CreateMtzCol("L", "H", grp++, 0, 1, 0);
    if (!_merged) { // only if we are processing unmerged data
        CreateMtzCol("M/ISYM", "Y", grp++, 0, 1, 0);
        CreateMtzCol("BATCH", "B", grp++, 0, 1, 0);
    }
    CreateMtzCol("I", "J", grp++, 0, 1, 0);
    CreateMtzCol("SIGI", "Q", grp++, 0, 1, 0);


    /*
     *   Filling MtzCols with data
     */
    int idx = 0;
    if (_merged) { /* MERGED DATA */
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
    } else { /* UNMERGED DATA */
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
                _mtz_cols[4]->ref[idx] = 1;
                _mtz_cols[5]->ref[idx] = intensity.value();
                _mtz_cols[6]->ref[idx] = intensity.sigma();
                idx++;
            }
        }
    }

    // unsure about this .. better would be more one value in each col
    _mtz_data->ncol_read = _mtz_cols.size();

    // needs to right value or it wont workj
    _mtz_data->nref = idx;
    _mtz_data->nref_filein = idx;

    /* SORT ORDER */
    // Let"s ignore this for now
    /*_mtz_data->order[0] = _mtz_cols.at(0);
    _mtz_data->order[1] = _mtz_cols.at(1);
    _mtz_data->order[2] = _mtz_cols.at(2);
    _mtz_data->order[3] = _mtz_cols.at(3);
    _mtz_data->order[4] = _mtz_cols.at(4);*/
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
    // buildBatch();
    buildHistory();

    MtzAddHistory(_mtz_data, (const char(*)[80])_comment.c_str(), 1);
}

void MtzExporter::buildHistory()
{
    /* File History */
    // Important for saving data processing steps of OHKL into other programs ?
    // maybe reverse order?
    for (auto& e : _history)
        MtzAddHistory(_mtz_data, (const char(*)[80])e.c_str(), 1);
}

void MtzExporter::addHistory(std::string line)
{
    _history.push_back(line);
}

bool MtzExporter::exportToFile(std::string filename)
{
    ohklLog(Level::Debug, "Export OpenHKL project to Mtz file ... '");

    /* Check and build Mtz data structure if needed */
    if (!_mtz_data)
        buildMtzData();

    /* Print out mtz data structure */
    // ccp4_lhprt(_mtz_data, 4);
    // ccp4_lhprt_adv(_mtz_data, 4);

    /* saving mtz file */
    if (CMtz::MtzPut(_mtz_data, filename.c_str()) == 1)
        ohklLog(Level::Info, "Project has been succesfully exported to '" + filename + "'");
    else {
        ohklLog(Level::Error, "Project export to '" + filename + " failed '");
        return false;
    }

    return true;
}

} // ohkl namespace
