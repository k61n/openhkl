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
#ifndef NSXLIB_XDS_H
#define NSXLIB_XDS_H

#include <iosfwd>
#include <string>
#include <memory>
#include <vector>

#include "../crystal/PeakRecord.h"

namespace nsx {

class Peak3D;
class UnitCell;

class XDS {

public:

    using sptrPeak3D = std::shared_ptr<Peak3D>;
    using PeakList = std::vector<sptrPeak3D>;
    using RecordList = std::vector<PeakRecord>;

    XDS(const PeakList& peaks, bool merge, bool friedel, const std::string& filename = "", const std::string& date = "");

    ~XDS() = default;

    bool writeHeader(std::ostream& str) const;

    bool writePeaks(std::ostream& str) const;

    bool writeFooter(std::ostream& str) const;

    bool write(std::ostream& str) const;

private:

    RecordList getMergedRecords() const;

    RecordList getUnmergedRecords() const;

    PeakList _peaks;

    const bool _merge, _friedel;

    const std::string _filename;

    const std::string _date;

    const std::vector<std::string> _records;
};

} // end namespace nsx

#endif // NSXLIB_XDS_H
