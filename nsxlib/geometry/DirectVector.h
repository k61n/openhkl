#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

//! \brief Class to define a vector defined in direct space.
//!
//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
class DirectVector
{
public:

    //! Default constructor
    DirectVector()=default;

    //! Copy constructor
    DirectVector(const DirectVector& other)=default;

    //! Construct a DirectVector from an Eigen column vector
    explicit DirectVector(const Eigen::Vector3d& dvector);

    //! Assignment operator
    DirectVector& operator=(const DirectVector& other)=default;

    //! Destructor
    ~DirectVector()=default;

    //! Cast operator to an Eigen column vector
    explicit operator const Eigen::Vector3d& () const;

    //! Return a reference to an element of the vector
    double& operator[](unsigned int index);

    //! Return a reference to an element of the vector
    double& operator()(unsigned int index);

    //! Print information about a DirectVector to a stream
    void print(std::ostream& os) const;

    double get2Theta(const Eigen::Vector3d& si) const;
    
    Eigen::Vector3d getKf(double wave, const Eigen::Vector3d& from) const;
    
    Eigen::Vector3d getQ(double wave, const Eigen::Vector3d& from) const;
    
    void getGammaNu(double& gamma, double& nu, const Eigen::Vector3d& from) const;

    double getLorentzFactor(const Eigen::Vector3d& from) const;
    
private:
    Eigen::Vector3d _dvector;
};

//! Overload operator<< with DirectVector type
std::ostream& operator<<(std::ostream& os, const DirectVector& dvector);

} // end namespace nsx
