/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "Diffractometer.h"
#include "IDataReader.h"

namespace nsx {

//! \brief Class to detector counts from raw binary data.
class RawDataReader: public IDataReader {

public:

    //! Create data reader from the given filename, associated to given diffractometer.
    static IDataReader* create(const std::string& filename, Diffractometer* diffractometer);

    RawDataReader() = delete;

    //! Deleted copy constructor
    RawDataReader(const RawDataReader &other) = delete;

    //! Construct a dataset from the list of files, with the give metadata.
    //! \param delta_chi per-frame change in chi axis of sample goniometer
    //! \param delta_omega per-frame change in omega axis of sample goniometer
    //! \param delta_phi per-frame change in phi axis of sample goniometer
    //! \param rowMajor determines if data is stored in row-major format (column major otherwise)
    //! \param swapEndian determines whether to swap the endianness of the input data
    //! \param bpp is the number of bytes per pixel
    RawDataReader(const std::vector<std::string>& filenames, Diffractometer *diffractometer,
            double wavelength, double delta_chi, double delta_omega, double delta_phi,
            bool rowMajor, bool swapEndian, unsigned int bpp);

    ~RawDataReader() = default;

    //! Deleted assignment operator
    RawDataReader& operator=(const RawDataReader &other) = delete;

    //! Open the file(s)
    void open() final;

    //! Close the file(s)
    void close() final;

    //! Read a single frame
    Eigen::MatrixXi data(size_t frame) final;

    //! Swap enddianness of the data
    void swapEndian();

    //! Set the bytes-per-pixel of the data
    void setBpp(unsigned int bpp);

private:

    template<typename T_>
    Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> matrixFromData() const;

private:

    unsigned int _bpp;

    size_t _length;

    bool _swapEndian;

    bool _rowMajor;

    std::vector<std::string> _filenames;

    std::vector<char> _data;

    double _wavelength;
};

template<typename T_>
Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> RawDataReader::matrixFromData() const
{
    assert(sizeof(T_)*_nRows*_nCols == _length);

    if (_rowMajor) {
        Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> frame;
        frame.resize(_nRows, _nCols);
        memcpy(&frame(0,0), &_data[0], _length);
        return frame;
    } else {
        Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> frame;
        frame.resize(_nRows, _nCols);
        memcpy(&frame(0,0), &_data[0], _length);
        return frame;
    }
}

} // end namespace nsx
