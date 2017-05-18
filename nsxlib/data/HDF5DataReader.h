/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

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

#ifndef NSXLIB_HDF5DATAREADER_H
#define NSXLIB_HDF5DATAREADER_H

#include <memory>
#include <string>

#include "H5Cpp.h"

#include "../data/IDataReader.h"

namespace nsx {

class HDF5DataReader final: public IDataReader
{

public:
    static IDataReader* create(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer);

    HDF5DataReader(const std::string& filename, std::shared_ptr<Diffractometer> instrument);
    virtual ~HDF5DataReader();

    Eigen::MatrixXi getData(size_t frame) override;

    void open() override;
    void close() override;

private:
    std::unique_ptr<H5::H5File> _file;
    std::unique_ptr<H5::DataSet> _dataset;
    std::unique_ptr<H5::DataSpace> _space;
    std::unique_ptr<H5::DataSpace> _memspace;
};

} // end namespace nsx

#endif // NSXLIB_HDF5DATAREADER_H
