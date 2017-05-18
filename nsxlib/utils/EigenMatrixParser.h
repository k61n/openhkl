#ifndef NSXLIB_EIGENMATRIXPARSER_H
#define NSXLIB_EIGENMATRIXPARSER_H

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/function/adapt_callable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/variant.hpp>
#include <Eigen/Dense>


namespace nsx {

namespace qi    = boost::spirit::qi;
namespace phx   = boost::phoenix;

// The convention for 2D data in NSXTOOL is to look at the detector
// from the sample point. In the rest position, the detector is along the beam
// (y-axis). The x-axis is horizontal and z-axis vertical, so that pixel (0,0)
// represents the bottom left and pixel (rows,cols) represents the top right.
// The functors in this file are used to do the mapping between written order
// of the data and parsing into a Eigen Matrix. For example TopRightColMajorMapper
// means that the datastream is written linearly from TopRight detector pixel and
// Column Major order (implicitly going down)


/* | .  .  .  .   n+1       1|
   | .  .  .  .   .         2|
   | .  .  .  .   .       ...|
   | .  .  .  .   .         n|
 */
struct TopRightColMajorMapper {
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=m.rows()-1;c=m.cols()-1;
        }
        m(r--, c) = v;

        if (int(r)<0)
        {
            r=m.rows()-1;
            c--;
        }
        return true;
    }
};
/* | n  .  .  .   2     1|
   | .  .  .  .   .   n+1|
   | .  .  .  .   .      |
   | .  .  .  .   .      |
 */
struct TopRightRowMajorMapper {
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=m.rows()-1;c=m.cols()-1;
        }
        m(r,c--)=v;
        if (int(c)<0) {
            c=m.cols()-1;
            r--;
        }
        return true;
    }
};

/* | .  .  .  .    .     n |
   | .  .  .  .    .    ...|
   | .  .  .  .    .     2 |
   | .  .  .  .   n+1    1 |
 */
struct BottomRightColMajorMapper {
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=0;c=m.cols()-1;
        }
        m(r++,c)=v;
        if (int(r)>=m.rows())
        {
            r=0;
            c--;
        }
        return true;
    }
};

/* | .  .  .  .    .     . |
   | .  .  .  .    .     . |
   | .  .  .  .    .    n+1|
   | n  .  .  .    2     1 |
 */
struct BottomRightRowMajorMapper {
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=0;c=m.cols()-1;
        }
        m(r,c--)=v;
        if (int(c)<0) {
            r++;
            c=m.cols()-1;
        }
        return true;
    }
};

/* | 1  n+1 .  .    .    .|
   | 2   .  .  .    .    .|
   | .   .  .  .    .    .|
   | n   .  .  .    .    .|
 */
struct TopLeftColMajorMapper {
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=m.rows()-1;c=0;
        }
        m(r--,c)=v;
        if (int(r)<0) {
            r=m.rows()-1;
            c++;
        }
        return true;
    }
};

/* | 1   2  .  .    .    n|
   |n+1  .  .  .    .    .|
   | .   .  .  .    .    .|
   | .   .  .  .    .    .|
 */
struct TopLeftRowMajorMapper {
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=m.rows()-1;c=0;
        }
        m(r,c++)=v;
        if (int(c)>=m.cols()) {
            c=0;
            r--;
        }
        return true;
    }
};

/* | n   .  .  .    .    .|
   | .   .  .  .    .    .|
   | 2   .  .  .    .    .|
   | 1  n+1 .  .    .    .|
 */
struct BottomLeftColMajorMapper
{
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=0;c=0;
        }
        m(r++,c)=v;
        if (int(r)>=m.rows()){
            r=0;
            c++;
        }
        return true;
    }
};

/* | .   .  .  .    .    .|
   | .   .  .  .    .    .|
   |n+1  .  .  .    .    .|
   | 1   2  .  .    .    n|
 */
struct BottomLeftRowMajorMapper
{
    template <typename Matrix, typename Row, typename Col, typename Value>
    bool operator()(Matrix &m, Row& r, Col& c, Value v) const
    {
        if (int(r)==-1 && int(c)==-1) {
            r=0;c=0;
        }
        m(r,c++)=v;
        if (int(c)>=m.cols()) {
            c=0;
            r++;
        }
        return true;
    }
};

template<typename It>
struct IMatrixParser : qi::grammar<It, Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::ColMajor>(), qi::blank_type, qi::locals<size_t,size_t> > {
    IMatrixParser() : IMatrixParser::base_type(matrix)
    {
    }
    qi::rule<It, Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::ColMajor>(), qi::blank_type, qi::locals<std::size_t,std::size_t> > matrix;
};

// Parser to read a chain of character (It can be a const char* or std::string::iterator)
// into a Eigen rowMajor matrix. Templated on the mapper depending on how the order
// of writing of the data. Return characters are skipped.
template<typename It, typename mapper>
struct EigenMatrixParser : IMatrixParser<It> {
    EigenMatrixParser() : IMatrixParser<It>()
    {
        using namespace qi;
        phx::function<mapper> const matrix_insert = mapper();
        // JMF modified to fix MSVC error C2872
        IMatrixParser<It>::matrix = eps [_a=-1, _b=-1] >> (+(int_ [ _pass = matrix_insert(_val, _a, _b, boost::spirit::_1) ])) % eol;
    }
};

} // end namespace nsx

#endif // NSXLIB_EIGENMATRIXPARSER_H
