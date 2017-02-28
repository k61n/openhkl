#ifndef NSXLIB_UTILS_MATRIXPARSER_H_
#define NSXLIB_UTILS_MATRIXPARSER_H_

#include <Eigen/Dense>

#include "../utils/Enums.h"

namespace SX
{

namespace Utils
{

using SX::Instrument::DataOrder;

class IMatrixParser
{
public:

    IMatrixParser()=default;
    virtual ~IMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const=0;

    bool operator()(const std::string& buffer, Eigen::MatrixXi& matrix) const;

};

class TopLeftColMajorMatrixParser final: public IMatrixParser
{
public:

    TopLeftColMajorMatrixParser()=default;
    virtual ~TopLeftColMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

class TopLeftRowMajorMatrixParser final: public IMatrixParser
{
public:

    TopLeftRowMajorMatrixParser()=default;
    virtual ~TopLeftRowMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

class TopRightColMajorMatrixParser final: public IMatrixParser
{
public:

    TopRightColMajorMatrixParser()=default;
    virtual ~TopRightColMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

class TopRightRowMajorMatrixParser final: public IMatrixParser
{
public:

    TopRightRowMajorMatrixParser()=default;
    virtual ~TopRightRowMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

class BottomLeftColMajorMatrixParser final: public IMatrixParser
{
public:

    BottomLeftColMajorMatrixParser()=default;
    virtual ~BottomLeftColMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

class BottomLeftRowMajorMatrixParser final: public IMatrixParser
{
public:

    BottomLeftRowMajorMatrixParser()=default;
    virtual ~BottomLeftRowMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

class BottomRightColMajorMatrixParser final: public IMatrixParser
{
public:

    BottomRightColMajorMatrixParser()=default;
    virtual ~BottomRightColMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

class BottomRightRowMajorMatrixParser final: public IMatrixParser
{
public:

    BottomRightRowMajorMatrixParser()=default;
    virtual ~BottomRightRowMajorMatrixParser()=default;

    virtual bool operator()(const char* begin, size_t bufferSize, Eigen::MatrixXi& matrix) const override;

    // Used to solve name shadowing
    using IMatrixParser::operator();
};

// Helper function that returns a MAtrixParser for a given data order. No need for a factory for such a case.
IMatrixParser* getMatrixParser(DataOrder dataOrder);

} // end namespace Utils

} // end namespace SX

#endif /* NSXLIB_UTILS_MATRIXPARSER_H_ */
