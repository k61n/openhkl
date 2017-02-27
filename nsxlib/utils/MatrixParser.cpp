#include <stdexcept>
#include <sstream>

#include "MatrixParser.h"

namespace SX {

namespace Utils {

bool IMatrixParser::operator()(const std::string& buffer, Eigen::MatrixXi& matrix) const
{
    return this->operator()(buffer.c_str(),buffer.size(),matrix);
}

bool TopLeftColMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nRows = matrix.rows();
    size_t nCols = matrix.cols();
    size_t lastRow = nRows - 1;
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t col = index/nRows;
        size_t row = lastRow + nRows*col - index;
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

bool TopLeftRowMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nRows = matrix.rows();
    size_t nCols = matrix.cols();
    size_t lastRow = nRows - 1;
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t row = lastRow - index/nCols;
        size_t col = index - nCols*(lastRow - row);
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

bool TopRightColMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nRows = matrix.rows();
    size_t nCols = matrix.cols();
    size_t lastRow = nRows - 1;
    size_t lastCol = nCols - 1;
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t col = lastCol - index/nRows;
        size_t row = lastRow + nRows*(lastCol - col) - index;
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

bool TopRightRowMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nRows = matrix.rows();
    size_t nCols = matrix.cols();
    size_t lastRow = nRows - 1;
    size_t lastCol = nCols - 1;
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t row = lastRow - index/nCols;
        size_t col = lastCol + nCols*(lastRow - row) - index;
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

bool BottomLeftColMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nRows = matrix.rows();
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t col = index/nRows;
        size_t row = index - nRows*col;
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

bool BottomLeftRowMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nCols = matrix.cols();
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t row = index/nCols;
        size_t col = index - nCols*row;
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

bool BottomRightColMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nRows = matrix.rows();
    size_t nCols = matrix.cols();
    size_t lastCol = nCols - 1;
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t col = lastCol - index/nRows;
        size_t row = index - nRows*(lastCol - col);
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

bool BottomRightRowMajorMatrixParser::operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const
{
    size_t nCols = matrix.cols();
    size_t lastCol = nCols - 1;
    int c;
    size_t index(0);
    std::stringstream ss;
    ss.write(begin,bufferSize);
    while(ss >> c) {
        size_t row = index/nCols;
        size_t col = lastCol + nCols*row - index;
        matrix(row,col) = c;
        ++index;
    }

    return ss.eof() && index == matrix.size();
}

IMatrixParser* getMatrixParser(DataOrder dataOrder)
{

    IMatrixParser* parser;

    switch (dataOrder) {
    case(DataOrder::TopLeftColMajor):
        parser = new TopLeftColMajorMatrixParser();
        break;
    case(DataOrder::TopLeftRowMajor):
        parser = new TopLeftRowMajorMatrixParser();
        break;
    case(DataOrder::TopRightColMajor):
        parser = new TopRightColMajorMatrixParser();
        break;
    case(DataOrder::TopRightRowMajor):
        parser = new TopRightRowMajorMatrixParser();
        break;
    case(DataOrder::BottomLeftColMajor):
        parser = new BottomLeftColMajorMatrixParser();
        break;
    case(DataOrder::BottomLeftRowMajor):
        parser = new BottomLeftRowMajorMatrixParser();
        break;
    case(DataOrder::BottomRightColMajor):
        parser = new BottomRightColMajorMatrixParser();
        break;
    case(DataOrder::BottomRightRowMajor):
        parser = new BottomRightRowMajorMatrixParser();
        break;
    default:
        throw std::runtime_error("Invalid data order");
    }

    return parser;
}

} // end namespace Utils

} // end namespace SX
