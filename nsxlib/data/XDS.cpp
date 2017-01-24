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

#include "XDS.h"
#include "Peak3D.h"

#include <ostream>
#include <algorithm>

using string = std::string;
using str_vector = std::vector<std::string>;
using Peak3D = SX::Crystal::Peak3D;

static const string space = "    ";
static const str_vector merged_records = {"H", "K", "L", "IOBS, SIGMA(IOBS)"};
static const str_vector unmerged_records = {"H", "K", "L", "XD", "YD", "ZD" "IOBS, SIGMA(IOBS)"};

namespace SX
{

namespace Data
{

using sptrPeak3D = XDS::sptrPeak3D;
using PeakList = XDS::PeakList;

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
    for(size_t id = 1; id <= _records.size(); ++id) {
        str << "!ITEM_" << _records[id] << "=" << id << std::endl;
    }
    str << "!END_OF_HEADER" << std::endl;
    return str.good();
}

bool XDS::writePeaks(std::ostream &str) const
{
    for (auto&& peak: _peaks) {
        auto&& center = peak->getPeak().getAABBCenter();
        auto&& index = peak->getIntegerMillerIndices();

        const int h = index[0];
        const int k = index[1];
        const int l = index[2];

        const double x = center[0];
        const double y = center[1];
        const double z = center[2];

        const double iobs = peak->getScaledIntensity();
        const double sigma = peak->getScaledSigma();

        str.width(5);
        str << h << k << l;
        str.width(15);
        str.precision(2);
        str << iobs << sigma;

        if (_merge) {
            continue;
        }
        str.width(8);
        str.precision(1);
        str << x << y << z;
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

} // Namespace Data
} // Namespace SX
