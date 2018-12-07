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

#pragma once

#include <tiffio.h>

#include "IDataReader.h"

namespace nsx {

//! Read detector image from Tiff file
class TiffDataReader: public IDataReader {

public:

    TiffDataReader() = delete;

    TiffDataReader(const TiffDataReader &other);

    TiffDataReader(const std::string& filename, Diffractometer *diffractometer);

    ~TiffDataReader() = default;

    TiffDataReader& operator=(const TiffDataReader &other);

    IDataReader* clone() const final;

    void open() final;

    void close() final;

    Eigen::MatrixXi data(std::size_t frame) final;

private:

    //! Type of encoding for each pixel.
    uint16 _bits;

    //! The handler to the TIFF file
    TIFF* _file;
};

} // end namespace nsx
