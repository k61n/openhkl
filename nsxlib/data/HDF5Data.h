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
#ifndef NSXTOOL_HDF5DATA_H_
#define NSXTOOL_HDF5DATA_H_
#include <string>
#include <memory>
#include "IData.h"
#include "H5Cpp.h"

namespace SX
{
namespace Data
{
/*! \brief HDF5 Data Format.
 *
 */
class HDF5Data final: public IData
{
public:
    static IData* create(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer);
    HDF5Data(const std::string& filename, const std::shared_ptr<Diffractometer>& instrument);
    virtual ~HDF5Data();
    Eigen::MatrixXi getFrame(std::size_t frame) override;
    void open() override;
    void close() override;
private:
    std::unique_ptr<H5::H5File> _file;
    std::unique_ptr<H5::DataSet> _dataset;
    std::unique_ptr<H5::DataSpace> _space;
    std::unique_ptr<H5::DataSpace> _memspace;
};

} // Namespace Data
} // Namespace SX

#endif /* NSXTOOL_HDF5DATA_H_ */
