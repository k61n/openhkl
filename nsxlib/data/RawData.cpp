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

 Forshungszentrum Juelich GmbH
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

#include <cmath>
#include <cstring>
#include <map>
#include <set>
#include <stdexcept>
#include <fstream>


#include "RawData.h"
#include "Component.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Gonio.h"
#include "Parser.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"
#include "Monochromator.h"

using std::ifstream;

namespace SX
{

namespace Data
{

using namespace SX::Utils;
using namespace SX::Units;

IData *RawData::create(const std::string &filename, std::shared_ptr<Diffractometer> diffractometer)
{
    std::vector<std::string> filenames;
    filenames.push_back(filename);
    return new RawData(filenames, diffractometer, 0, 0, 0, 0);
}

RawData::RawData(const std::vector<std::string>& filenames, std::shared_ptr<Diffractometer> diffractometer,
                 double wavelength, double delta_chi, double delta_omega, double delta_phi):
    IData(filenames[0], diffractometer),
    _filenames(filenames),
    _wavelength(wavelength),
    _delta_chi(delta_chi),
    _delta_omega(delta_omega),
    _delta_phi(delta_phi),
    _rowMajor(true),
    _swapEndian(true),
    _bpp(2)
{
    // ensure that there is at least one monochromator!
    if ( _diffractometer->getSource()->getNMonochromators() == 0 ) {
        Monochromator mono("mono");
        _diffractometer->getSource()->addMonochromator(&mono);
    }

    _length = _bpp * _nrows * _ncols;

    _diffractometer->getSource()->setWavelength(_wavelength);

    _metadata->add<std::string>("Instrument", _diffractometer->getName());
    _metadata->add<double>("wavelength", _wavelength);
    _metadata->add<int>("npdone", _filenames.size());

    _data.resize(_bpp*_nrows*_ncols);

    _nFrames = _filenames.size();


}

RawData::~RawData()
{

}

void RawData::open()
{

}

void RawData::close()
{

}

Eigen::MatrixXi RawData::readFrame(std::size_t idx)
{
    std::string filename = _filenames.at(idx);

    _data.resize(_length);

    ifstream file;
    file.open(filename, std::ios_base::binary | std::ios_base::in);

    if (!file.is_open())
        throw std::runtime_error("could not open data file " + filename);

    file.seekg(0, std::ios_base::end);

    if (_length != file.tellg()) {
        std::string err_msg = "data file " + filename + " is not of the expected size: ";
        err_msg += "expected " + std::to_string(_length) + " bytes but found " + std::to_string(file.tellg());
        throw std::runtime_error(err_msg);
    }

    file.seekg(0, std::ios_base::beg);
    file.read(&_data[0], _length);

    if ( _length != file.gcount()) {
        std::string err_msg = "did not read " + filename + " successfully: ";
        err_msg += "expected " + std::to_string(_length) + " bytes but read " + std::to_string(file.gcount());
        throw std::runtime_error(err_msg);
    }

    swapEndian();

    switch(_bpp) {
    case 1:
        return matrixFromData<uint8_t>().cast<int>();
    case 2:
        return matrixFromData<uint16_t>().cast<int>();
    case 3:
        return matrixFromData<uint32_t>().cast<int>();
    default:
        throw std::runtime_error("bpp unsupported: " + std::to_string(_bpp));
    }
}

void RawData::swapEndian()
{
    if (!_swapEndian)
        return;

    for (int i = 0; i < _nrows*_ncols; ++i) {
        for (int byte = 0; byte < _bpp/2; ++byte) {
            std::swap(_data[_bpp*i+byte], _data[_bpp*i+(_bpp-1-byte)]);
        }
    }
}

void RawData::setBpp(int bpp)
{
    _bpp = bpp;
    _length = _bpp*_nrows*_ncols;
}

} // end namespace Data

} // end namespace SX
