//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/monte-carlo/mozaictester.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_MONTE_CARLO_MOZAICTESTER_H
#define CORE_MONTE_CARLO_MOZAICTESTER_H

#include <Eigen/Dense>

namespace nsx {

//! Class to test the Monte-Carlo peak profile calculation
struct MozaicTester {
    MozaicTester();

    Eigen::Vector3d buildQVector(
        double alpha1, double ca2, double sa2, double znorm, const Eigen::Vector3d& u,
        const Eigen::Vector3d& v, const Eigen::Vector3d& w);
    void updateDetector(
        double alpha1, double znorm, double calpha2, double salpha2, double lambda,
        const Eigen::Vector3d& u, const Eigen::Vector3d& v, const Eigen::Vector3d& w,
        const Eigen::Vector3d& n, double px, double py, double pz, Eigen::MatrixXd& detector);
    Eigen::MatrixXd run(double h, double k, double l, int n);
    Eigen::MatrixXd runCenter(double h, double k, double l, int n);
    Eigen::MatrixXd runContour(double h, double k, double l);
    Eigen::MatrixXd
    run1(double h, double k, double l, double px = 0.0, double py = 0.0, double pz = 0.0);
    double _l;
    double _dl;
    double _sRadius;
    double _hMono;
    double _wMono;
    double _dMono;
    double _dDet;
    double _mu;
    double _wDet;
    double _hDet;
    double _nRows;
    double _nCols;

    Eigen::Matrix3d _ub;
};

} // namespace nsx

#endif // CORE_MONTE_CARLO_MOZAICTESTER_H
