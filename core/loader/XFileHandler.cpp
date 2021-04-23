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

const std::string XFileHandler::Reflection::stop_flag = "999";

std::vector<std::string> tokenize(const std::string& line)
{
    std::istringstream iss(line);
    std::vector<std::string> tokens(
        std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
    return tokens;
}

XFileHandler::XFileHandler(std::string filename) : _filename(filename) { }

void XFileHandler::readXFile(double frame)
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

    while (true) {
        if (!std::getline(ifs, line))
            break;
        Reflection refl;
        bool stop = refl.parse(line);
        if (stop) {
            _mask = Eigen::MatrixXi(refl.maskx, refl.masky);
            break;
        } else {
            _reflections.emplace_back(refl);
        }
    }

    for (int j = 0; j < _mask.rows(); ++j) {
        std::getline(ifs, line);
        tokens = tokenize(line);
        for (int i = 0; i < _mask.cols(); ++i) {
            _mask(i, j) = std::stoi(tokens[i]);
        }
    }
    // TODO: rest of the restart metadata? I think this stuff is mainly for restarting a DENZO run.
}

std::vector<Eigen::Vector3d> XFileHandler::getPeakCentres()
{
    std::vector<Eigen::Vector3d> peaks;
    for (auto refl : _reflections) {
        Eigen::Vector3d peak(refl.px, refl.py, _frame);
        peaks.emplace_back(peak);
    }
    return peaks;
}

bool XFileHandler::Reflection::parse(std::string line)
{
    std::vector<std::string> tokens = tokenize(line);
    if (tokens[0] == stop_flag){
        maskx = std::stoi(tokens[1]);
        masky = std::stoi(tokens[2]);
        return false;
    }
    h = std::stoi(tokens[0]);
    k = std::stoi(tokens[1]);
    l = std::stoi(tokens[2]);
    partial = bool(std::stoi(tokens[3]));
    I_profile = std::stod(tokens[4]);
    I_summation = std::stod(tokens[5]);
    chisq = std::stod(tokens[6]);
    sigma = std::stod(tokens[7]);
    cos_detector_angle = std::stod(tokens[8]);
    px = std::stod(tokens[9]);
    py = std::stod(tokens[10]);
    obliquity = std::stod(tokens[11]);
    strength = std::stod(tokens[12]);
    return true;
}

} // namespace nsx
