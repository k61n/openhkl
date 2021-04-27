//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/raw/XFileHandler.cpp
//! @brief     Defines class XFileHandler
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/loader/XFileHandler.h"

#include <fstream>
#include <sstream>
#include <iterator>

namespace nsx {

// TODO: bounds checking against detector object (ncols/nrows)

const std::string XFileHandler::Reflection::stop_flag = "999";

std::vector<std::string> tokenize(const std::string& line)
{
    std::istringstream iss(line);
    std::vector<std::string> tokens(
        std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
    return tokens;
}

XFileHandler::XFileHandler(std::string filename) : _filename(filename) { }

void XFileHandler::readXFile(int frame)
{
    _frame = frame;
    std::ifstream ifs(_filename);
    std::string line;
    std::vector<std::string> tokens;
    // get the header data: name
    std::getline(ifs, line);
    _name = line;
    // get the header data: a/u cell orientation matrices
    std::getline(ifs, line);
    tokens = tokenize(line);
    _a(0,0) = std::stod(tokens[0]);
    _a(1,0) = std::stod(tokens[1]);
    _a(2,0) = std::stod(tokens[2]);
    _u(0,0) = std::stod(tokens[3]);
    _u(1,0) = std::stod(tokens[3]);
    _u(2,0) = std::stod(tokens[4]);
    std::getline(ifs, line);
    tokens = tokenize(line);
    _a(0,1) = std::stod(tokens[0]);
    _a(1,1) = std::stod(tokens[1]);
    _a(2,1) = std::stod(tokens[2]);
    _u(0,1) = std::stod(tokens[3]);
    _u(2,1) = std::stod(tokens[3]);
    _u(1,1) = std::stod(tokens[4]);
    std::getline(ifs, line);
    tokens = tokenize(line);
    _a(0,2) = std::stod(tokens[0]);
    _a(1,2) = std::stod(tokens[1]);
    _a(2,2) = std::stod(tokens[2]);
    _u(0,2) = std::stod(tokens[3]);
    _u(1,2) = std::stod(tokens[3]);
    _u(2,2) = std::stod(tokens[4]);

    // Other metadata (line 5)
    std::getline(ifs, line);
    tokens = tokenize(line);
    _osc_start = std::stod(tokens[0]);
    _osc_end = std::stod(tokens[1]);
    _dist_pixels = std::stod(tokens[2]);
    _wavelength = std::stod(tokens[3]);
    _rotz = std::stod(tokens[4]);
    _roty = std::stod(tokens[5]);
    _rotx = std::stod(tokens[6]);
    _mosaicity = std::stod(tokens[7]);

    // Peak centres
    while (std::getline(ifs, line)) {
        Reflection refl;
        bool stop = refl.parse(line);
        if (stop) {
            _mask = Eigen::MatrixXi(refl.maskx, refl.masky);
            break;
        } else {
            _reflections.emplace_back(refl);
        }
    }

    // The integration mask
    // for (int j = 0; j < _mask.rows(); ++j) {
    //     std::getline(ifs, line);
    //     tokens = tokenize(line);
    //     for (int i = 0; i < _mask.cols(); ++i) {
    //         _mask(i, j) = std::stoi(tokens[i]);
    //     }
    // }
    // TODO: rest of the restart metadata? I think this stuff is mainly for restarting a DENZO run.
}

std::vector<Eigen::Vector3d> XFileHandler::getPeakCenters()
{
    std::vector<Eigen::Vector3d> peaks;
    for (auto refl : _reflections) {
        Eigen::Vector3d peak(refl.px, refl.py, static_cast<double>(_frame));
        peaks.emplace_back(peak);
    }
    return peaks;
}

bool XFileHandler::Reflection::parse(std::string line)
{
    // TODO: read this in fixed format, only the px/py columns
    std::vector<std::string> tokens = tokenize(line);
    if (tokens[0] == stop_flag){
        maskx = std::stoi(tokens[1]);
        masky = std::stoi(tokens[2]);
        return true;
    }
    h = std::stoi(tokens[0]);
    k = std::stoi(tokens[1]);
    l = std::stoi(tokens[2]);
    if (tokens.size() == 13) {
        py = std::stod(tokens[9]);
        px = std::stod(tokens[10]);
    } else if (tokens.size() == 12) {
        // BioDiff does not count neutrons; has a photomultiplier of ~30. Therefore columns
        // Might start to overlap
        py = std::stod(tokens[8]);
        px = std::stod(tokens[9]);
    } else if (tokens.size() == 11) {
        py = std::stod(tokens[7]);
        px = std::stod(tokens[8]);
    } else if (tokens.size() == 10) {
        py = std::stod(tokens[6]);
        px = std::stod(tokens[7]);
    } else if (tokens.size() == 9) {
        py = std::stod(tokens[6]);
        px = std::stod(tokens[7]);
    } else {
        throw std::runtime_error("Malformed .x file");
    }

    return false;
}

void PeakCenterDataSet::init(int nframes)
{
    _xfiles.clear();
    for (int i = 0; i < nframes; ++i)
        _xfiles.push_back(nullptr);
}

XFileHandler* PeakCenterDataSet::getFrame(int frame)
{
    return _xfiles[frame].get();
}

void PeakCenterDataSet::addFrame(std::string filename, int frame)
{
    _xfiles[frame] = std::make_unique<XFileHandler>(filename);
    _xfiles[frame]->readXFile(frame);
}

} // namespace nsx
