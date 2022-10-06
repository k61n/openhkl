//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/MatrixParser.cpp
//! @brief     Implements class MatrixParser
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <sstream>
#include <stdexcept>

#include "core/detector/MatrixParser.h"

namespace nsx {

bool MatrixParser::operator()(
    DataOrder dataOrder, const char* begin, size_t buffer_size, Eigen::MatrixXi& matrix) const
{
    size_t n_rows = matrix.rows();
    size_t n_cols = matrix.cols();
    size_t n_elements = matrix.size();

    int s1, s2, offset;

    switch (dataOrder) {
        case (DataOrder::TopLeftColMajor):
            s1 = -1;
            s2 = n_rows;
            offset = n_rows - 1;
            break;
        case (DataOrder::TopLeftRowMajor):
            s1 = -n_cols;
            s2 = 1;
            offset = n_cols * (n_rows - 1);
            break;
        case (DataOrder::TopRightColMajor):
            s1 = -1;
            s2 = -n_rows;
            offset = n_rows * n_cols - 1;
            break;
        case (DataOrder::TopRightRowMajor):
            s1 = -n_cols;
            s2 = -1;
            offset = n_rows * n_cols - 1;
            break;
        case (DataOrder::BottomLeftColMajor):
            s1 = 1;
            s2 = n_rows;
            offset = 0;
            break;
        case (DataOrder::BottomLeftRowMajor):
            s1 = n_cols;
            s2 = 1;
            offset = 0;
            break;
        case (DataOrder::BottomRightColMajor):
            s1 = 1;
            s2 = -n_rows;
            offset = n_rows * (n_cols - 1);
            break;
        case (DataOrder::BottomRightRowMajor):
            s1 = n_cols;
            s2 = -1;
            offset = n_cols - 1;
            break;
        default: throw std::runtime_error("Invalid data order");
    }

    Eigen::VectorXi data(n_elements);

    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin, buffer_size);
    while (index < n_elements && ss >> c)
        data(index++) = c;

    for (size_t i = 0; i < n_rows; ++i) {
        for (size_t j = 0; j < n_cols; ++j)
            matrix(i, j) = data(i * s1 + j * s2 + offset);
    }

    return ss.eof() && index == n_elements;
}

bool MatrixParser::operator()(
    DataOrder dataOrder, const std::string& buffer, Eigen::MatrixXi& matrix) const
{
    return this->operator()(dataOrder, buffer.c_str(), buffer.size(), matrix);
}

} // namespace nsx
