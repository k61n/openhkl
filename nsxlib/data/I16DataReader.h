/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
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

#ifndef NSXTOOL_I16DATAREADER_H_
#define NSXTOOL_I16DATAREADER_H_

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "../data/IDataReader.h"
#include "../instrument/Diffractometer.h"

namespace SX {

namespace Data {

class I16DataReader final: public IDataReader
{
public:

    static IDataReader* create(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer);

    //! Default constructor
    I16DataReader(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer);
    //! Copy constructor
    I16DataReader(const I16DataReader& other)=delete;
    //! Destructor
    virtual ~I16DataReader()=default;

    //! Assignment operator
    I16DataReader& operator=(const I16DataReader& other)=delete;

    // Other methods
    void open() override;
    void close() override;
    //! Read a single frame
    Eigen::MatrixXi getData(size_t frame) override;

private:
    //! Vector of all TIFF files.
    std::vector<std::string> _tifs;
    std::string _basedirectory;
};

} // end namespace Data

} // end namespace SX

#endif /* NSXTOOL_I16DATAREADER_H_ */
