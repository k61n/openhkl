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

#ifndef NSXTOOL_RAWDATAREADER_H_
#define NSXTOOL_RAWDATAREADER_H_

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Dense>

#include "../data/IDataReader.h"
#include "../instrument/Diffractometer.h"


namespace SX {

namespace Data {

class RawDataReader final: public IDataReader {

public:

    static IDataReader* create(const std::string& filename, const Diffractometer& diffractometer);

    //! Default constructor
    RawDataReader(const std::vector<std::string>& filenames, const Diffractometer& diffractometer,
            double wavelength, double delta_chi, double delta_omega, double delta_phi,
            bool rowMajor, bool swapEndian, unsigned int bpp);

    //! Copy constructor
    RawDataReader(const RawDataReader& other)=delete;

    //! Destructor
    virtual ~RawDataReader()=default;

    // Operators

    //! Assignment operator
    RawDataReader& operator=(const RawDataReader& other)=delete;

    // Other methods
    void open() override;
    void close() override;
    //! Read a single frame

    Eigen::MatrixXi getData(size_t frame) override;

    void swapEndian();
    void setBpp(unsigned int bpp);

private:
    template<typename T_>
    Eigen::Matrix<T_, Eigen::Dynamic, Eigen::Dynamic> matrixFromData() const;

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

} // end namespace Data

} // end namespace SX

#endif /* NSXTOOL_RAWDATAREADER_H_ */
