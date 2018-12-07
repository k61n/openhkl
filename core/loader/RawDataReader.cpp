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
#include <string>

#include "Component.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "EigenToVector.h"
#include "Gonio.h"
#include "Monochromator.h"
#include "Parser.h"
#include "RawDataReader.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace nsx {

RawDataReader::RawDataReader(const std::string &filename, Diffractometer *diffractometer)
: IDataReader(filename, diffractometer),
  _parameters(),
  _length(0),
  _data()
{
    // ensure that there is at least one monochromator!
    if ( _diffractometer->source().nMonochromators() == 0 ) {
        Monochromator mono("mono");
        _diffractometer->source().addMonochromator(mono);
    }

    setParameters(_parameters);

    addFrame(filename);

}

IDataReader* RawDataReader::clone() const
{
    return new RawDataReader(*this);
}

void RawDataReader::addFrame(const std::string &filename)
{
    _filenames.push_back(filename);

    _nFrames = _filenames.size();

    const auto& detector_gonio = _diffractometer->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();
    Eigen::VectorXd dm(n_detector_gonio_axes);
    _detectorStates.push_back(eigenToVector(dm));

    // Getting Scan parameters for the sample
    const auto &sample_gonio = _diffractometer->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    dm.resize(n_sample_gonio_axes);

    int omega, phi, chi;
    for (size_t i = 0, omega = -1, phi = -1, chi = -1; i < n_sample_gonio_axes; ++i) {
        const std::string axis_name = sample_gonio.axis(i).name();
        omega = axis_name == "omega" ? int(i) : omega;
        chi = axis_name == "chi"? int(i) : chi;
        phi = axis_name == "phi"? int(i) : phi;
    }

    assert(omega != -1);
    assert(phi   != -1);
    assert(chi != -1);

    size_t idx = _nFrames - 1;

    dm(omega) = idx*_parameters.delta_omega;
    dm(phi) = idx*_parameters.delta_phi;
    dm(chi) = idx*_parameters.delta_chi;

    // Use natural units internally (rad)
    dm*=deg;

    _sampleStates.push_back(eigenToVector(dm));
}

void RawDataReader::open() {
}

void RawDataReader::close() {
}

const RawDataReaderParameters& RawDataReader::parameters() const
{
    return _parameters;
}

void RawDataReader::setParameters(const RawDataReaderParameters &parameters)
{
    _parameters = parameters;

    _length = _parameters.bpp * _nRows * _nCols;
    auto& mono = _diffractometer->source().selectedMonochromator();
    mono.setWavelength(_parameters.wavelength);

    _metadata.add<std::string>("Instrument", _diffractometer->name());
    _metadata.add<double>("wavelength", _parameters.wavelength);
    _metadata.add<int>("npdone", int(_filenames.size()));
    _metadata.add<double>("monitor", 0.0);
    _metadata.add<int>("Numor", 0.0);

    _data.resize(_parameters.bpp*_nRows*_nCols);
}

void RawDataReader::swapEndian() {

    if (!_parameters.swap_endian) {
        return;
    }

    for (unsigned int i = 0; i < _nRows*_nCols; ++i) {
        for (unsigned int byte = 0; byte < _parameters.bpp/2; ++byte) {
            std::swap(_data[_parameters.bpp*i+byte], _data[_parameters.bpp*i+(_parameters.bpp-1-byte)]);
        }
    }
}

Eigen::MatrixXi RawDataReader::data(size_t frame) {

    std::string filename = _filenames.at(frame);

    std::ifstream file;
    file.open(filename, std::ios_base::binary | std::ios_base::in);

    if (!file.is_open()) {
        throw std::runtime_error("could not open data file " + filename);
    }

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

    switch(_parameters.bpp) {
    case 1:
        return matrixFromData<uint8_t>().cast<int>();
    case 2:
        return matrixFromData<uint16_t>().cast<int>();
    case 3:
        return matrixFromData<uint32_t>().cast<int>();
    default:
        throw std::runtime_error("bpp unsupported: " + std::to_string(_parameters.bpp));
    }
}

} // end namespace nsx
