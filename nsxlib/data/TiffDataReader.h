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

#ifndef NSXTOOL_TIFFDATAREADER_H_
#define NSXTOOL_TIFFDATAREADER_H_

#include <memory>

#include <tiffio.h>

#include "../data/IDataReader.h"

namespace nsx {

class TiffDataReader final: public IDataReader {

public:
    static IDataReader* create(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer);

    TiffDataReader(const std::string& filename, const std::shared_ptr<Diffractometer>& diffractometer);

    virtual ~TiffDataReader()=default;

    void open() override;

    void close() override;

    Eigen::MatrixXi getData(std::size_t frame) override;

private:

    //! Type of encoding for each pixel.
    uint16 _bits;
    //!
    TIFF* _file;
};

} // end namespace nsx

#endif /* NSXTOOL_TIFFDATAREADER_H_ */
