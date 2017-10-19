/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include <algorithm>
#include <iomanip>
#include <ostream>

#include "../crystal/MergedPeak.h"
#include "../crystal/Peak3D.h"
#include "../crystal/ResolutionShell.h"
#include "../crystal/SpaceGroup.h"
#include "../crystal/UnitCell.h"
#include "../data/XDS.h"

static const std::string space = "    ";
static const std::vector<std::string> merged_records = {"H", "K", "L", "IOBS", "SIGMA(IOBS)"};
static const std::vector<std::string> unmerged_records = {"H", "K", "L", "XD", "YD", "ZD", "IOBS", "SIGMA(IOBS)"};

namespace nsx {

#if 0

XDS::XDS(const PeakList &peaks, bool merge, bool friedel, const std::string &filename, const std::string &date):
    _peaks(peaks),
    _merge(merge),
    _friedel(friedel),
    _filename(filename),
    _date(date),
    _records(merge ? merged_records : unmerged_records)
{
    std::sort(_peaks.begin(), _peaks.end(), [](const sptrPeak3D& a, const sptrPeak3D& b) { return *a < *b; });
}

bool XDS::writeHeader(std::ostream &str) const
{
    // write XDS header
    str << "!FORMAT=XDS_ASCII" << space;
    str << "MERGE=" << (_merge ? "TRUE" : "FALSE") << space;
    str << "FRIEDEL'S_LAW=" << (_friedel ? "TRUE" : "FALSE") << space;
    str << std::endl;

    if (!_filename.empty()) {
        str << "!OUTPUT_FILE=" << _filename << space;
    }
    if (!_date.empty()) {
        str << "DATE=" << _date;
    }
    str << std::endl;

    // more header....

    // data record specification
    str << "!NUMBER_OF_ITEMS_IN_EACH_DATA_RECORD=" << _records.size() << std::endl;
    for(size_t id = 0; id < _records.size(); ++id) {
        str << "!ITEM_" << _records[id] << "=" << id+1 << std::endl;
    }
    str << "!END_OF_HEADER" << std::endl;
    return str.good();
}

bool XDS::writePeaks(std::ostream &str) const
{
    for (auto&& peak: _peaks) {
        auto&& center = peak->getShape().aabb().center();
        auto&& index = peak->getIntegerMillerIndices();

        const int h = index[0];
        const int k = index[1];
        const int l = index[2];

        const double x = center[0];
        const double y = center[1];
        const double z = center[2];

        const double iobs = peak->getScaledIntensity().value();
        const double sigma = peak->getScaledIntensity().sigma();

        str << std::setw(5) << h;
        str << std::setw(5) << k;
        str << std::setw(5) << l;

        if (!_merge) {
            str.width(8);
            str.precision(1);
            str << std::fixed << std::setw( 10 ) << std::setprecision( 1 ) << x;
            str << std::fixed << std::setw( 10 ) << std::setprecision( 1 ) << y;
            str << std::fixed << std::setw( 10 ) << std::setprecision( 1 ) << z;

        }
        str << std::fixed << std::setw( 15 ) << std::setprecision( 1 ) << iobs;
        str << std::fixed << std::setw( 15 ) << std::setprecision( 1 ) << sigma;
        str << std::endl;
    }
    return str.good();
}

bool XDS::writeFooter(std::ostream &str) const
{
    str << "!END_OF_DATA" << std::endl;
    return str.good();
}

bool XDS::write(std::ostream& str) const
{
    if (!writeHeader(str)) {
        return false;
    }
    if (!writePeaks(str)) {
        return false;
    }
    return writeFooter(str);
}

RecordList XDS::getMergedRecords() const
{
    const double dmin = 0.0;
    const double dmax = 200.0;
    const size_t num_shells = 10;
    ResolutionShell res = {dmin, dmax, num_shells};
    RecordList records;

    if (_peaks.size() == 0) {
        return records;
    }

    auto cell = _peaks[0]->getActiveUnitCell();
    auto grp = SpaceGroup(cell->getSpaceGroup());

    for (auto&& peak: _peaks) {
        if (cell != peak->getActiveUnitCell()) {
            // qCritical() << "Only one unit cell is supported at this time!!";
            // todo(jonathan): better handling of this case!
            continue;
        }
        res.addPeak(peak);
    }

    auto&& shells = res.getShells();
    std::vector<PeakList> peak_equivs;

    for (size_t i = 0; i < size_t(num_shells); ++i) {
        auto equivs = grp.findEquivalences(shells[i], _friedel);

        for (auto&& equiv: equivs)
            peak_equivs.push_back(equiv);
    }

    for (auto equiv: peak_equivs) {
        MergedPeak new_peak(grp, _friedel);

        for (auto peak: equiv) {
            // skip bad/masked peaks
            if (peak->isMasked() || !peak->isSelected())
                continue;

            new_peak.addPeak(peak);
        }
        records.emplace_back(new_peak);
    }

    std::sort(records.begin(), records.end());
    return records;
}

RecordList XDS::getUnmergedRecords() const
{
    RecordList records;

    for (auto&& peak: _peaks) {
        records.emplace_back(*peak);
    }
    std::sort(records.begin(), records.end());
    return records;
}

#endif

} // end namespace nsx
