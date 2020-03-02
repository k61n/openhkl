//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/monte-carlo/MCAbsorption.h
//! @brief     Defines class MCAbsorption
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_MONTE_CARLO_MCABSORPTION_H
#define CORE_MONTE_CARLO_MCABSORPTION_H

#include "base/hull/ConvexHull.h"

namespace nsx {

//! Computes absorption correction by Monte-Carlo integration.

class MCAbsorption {
 public:
    //! Define absorption Engine with a rectangular source of WxH
    MCAbsorption(
        const ConvexHull& convex_hull, double source_width, double source_height,
        double source_y_pos);

    ~MCAbsorption();

    void setConvexHull(const ConvexHull& convex_hull);

    void setMuAbsorption(double mu_absorption);

    void setMuScattering(double mu_scattering);

    //! Run the Monte-Carlo calculation
    double
    run(unsigned int nIterations, const Eigen::Vector3d& outV,
        const Eigen::Matrix3d& sampleOrientation) const;

 private:
    //! Description of the sample in terms of individual oriented triangles
    ConvexHull _convex_hull;

    double _mu_scattering;

    double _mu_absorption;

    double _source_width;

    double _source_height;

    double _source_y_pos;
};

} // namespace nsx

#endif // CORE_MONTE_CARLO_MCABSORPTION_H
