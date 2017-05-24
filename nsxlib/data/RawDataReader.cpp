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

#include <cmath>
#include <cstring>
#include <map>
#include <fstream>
#include <set>
#include <stdexcept>

#include "../data/RawDataReader.h"
#include "../instrument/Component.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Gonio.h"
#include "../instrument/Monochromator.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../instrument/ComponentState.h"
#include "../utils/Units.h"
#include "../utils/Parser.h"
#include "../utils/EigenToVector.h"

namespace nsx {

IDataReader* RawDataReader::create(const std::string &filename, const std::shared_ptr<Diffractometer>& diffractometer) {
    std::vector<std::string> filenames;
    filenames.push_back(filename);
    return new RawDataReader(filenames, diffractometer, 0, 0, 0, 0, true, true, 2);
}

RawDataReader::RawDataReader(const std::vector<std::string>& filenames, const std::shared_ptr<Diffractometer>& diffractometer,
                 double wavelength, double delta_chi, double delta_omega, double delta_phi,
                 bool rowMajor, bool swapEndian, unsigned int bpp)
: IDataReader(filenames[0], diffractometer),
  _bpp(bpp),
  _length(0),
  _swapEndian(swapEndian),
  _rowMajor(rowMajor),
  _filenames(filenames),
  _data(),
  _wavelength(wavelength)
{
    // ensure that there is at least one monochromator!
    if ( _diffractometer->getSource()->getNMonochromators() == 0 ) {
        Monochromator mono("mono");
        _diffractometer->getSource()->addMonochromator(mono);
    }

    _length = _bpp * _nRows * _nCols;
    auto& mono = _diffractometer->getSource()->getSelectedMonochromator();
    mono.setWavelength(_wavelength);

    _metadata.add<std::string>("Instrument", _diffractometer->getName());
    _metadata.add<double>("wavelength", _wavelength);
    _metadata.add<int>("npdone", int(_filenames.size()));
    _metadata.add<double>("monitor", 0.0);
    _metadata.add<int>("Numor", 0.0);

    _data.resize(_bpp*_nRows*_nCols);

    _nFrames = _filenames.size();

    std::vector<std::string> axesS = _diffractometer->getDetector()->getGonio()->getPhysicalAxesNames();
    Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> dm
            = Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>::Zero(long(axesS.size()), long(_nFrames));

    _states.resize(_nFrames);

    for (unsigned int i = 0; i < _nFrames; ++i) {
        _states[i].detector = ComponentState(_diffractometer->getDetector().get(), eigenToVector((dm.col(i))));
        //_detectorStates.push_back(_diffractometer->getDetector()->createStateFromEigen(dm.col(i)));
    }

    // Getting Scan parameters for the sample
    axesS = _diffractometer->getSample()->getGonio()->getPhysicalAxesNames();
    dm.resize(long(axesS.size()), long(_nFrames));

    int omega, phi, chi;
    unsigned int i;

    for (i = 0, omega = -1, phi = -1, chi = -1; i < axesS.size(); ++i) {
        omega = axesS[i] == "omega" ? int(i) : omega;
        chi = axesS[i] == "chi"? int(i) : chi;
        phi = axesS[i] == "phi"? int(i) : phi;
    }

    assert(omega != -1);
    assert(phi   != -1);
    assert(chi != -1);

    for (i = 0; i < _nFrames; ++i) {
        dm(omega, i) = i*delta_omega;
        dm(phi, i) = i*delta_phi;
        dm(chi,i) = i*delta_chi;
    }

    // Use natural units internally (rad)
    dm*=deg;

    for (unsigned int i=0;i<_nFrames;++i) {
        _states[i].sample = ComponentState(_diffractometer->getSample().get(), eigenToVector(dm.col(i)));
    }
}

void RawDataReader::open() {
}

void RawDataReader::close() {
}

void RawDataReader::swapEndian() {

    if (!_swapEndian)
        return;

    for (unsigned int i = 0; i < _nRows*_nCols; ++i) {
        for (unsigned int byte = 0; byte < _bpp/2; ++byte) {
            std::swap(_data[_bpp*i+byte], _data[_bpp*i+(_bpp-1-byte)]);
        }
    }
}

void RawDataReader::setBpp(unsigned int bpp) {
    _bpp = bpp;
    _length = _bpp*_nRows*_nCols;
}

Eigen::MatrixXi RawDataReader::getData(std::size_t frame) {

    std::string filename = _filenames.at(frame);

    std::ifstream file;
    file.open(filename, std::ios_base::binary | std::ios_base::in);

    if (!file.is_open())
        throw std::runtime_error("could not open data file " + filename);

    file.seekg(0, std::ios_base::end);

    if (_length != size_t(file.tellg())) {
        std::string err_msg = "data file " + filename + " is not of the expected size: ";
        err_msg += "expected " + std::to_string(_length) + " bytes but found " + std::to_string(file.tellg());
        throw std::runtime_error(err_msg);
    }

    file.seekg(0, std::ios_base::beg);
    file.read(&_data[0], long(_length));

    if ( _length != size_t(file.gcount())) {
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

} // end namespace nsx
