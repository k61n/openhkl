//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/ResolutionShell.h
//! @brief     Defines class ResolutionShell
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_RESOLUTIONSHELL_H
#define OHKL_CORE_STATISTICS_RESOLUTIONSHELL_H

#include <vector>

namespace ohkl {

class Peak3D;

//! Utility structure to store a list of peaks with resolution in the range [dmin, dmax].
struct DShell {
    //! Minimum value of d, i.e. maximum resolution
    double dmin;
    //! Maximum value of d, i.e. minimum resolution
    double dmax;
    //! List of peaks contained within the shell
    std::vector<Peak3D*> peaks;
};

//! Splits a set of peaks into a number of shells based on resolution.

class ResolutionShell {
 public:
    //! Construct the given number of cells with abolute minimum dmin and absolute maximum dmax.
    ResolutionShell(double dmin, double dmax, std::size_t num_shells);
    //! Add a peak to the list of shells.
    //! It will automatically be added to the appropriate shell.
    void addPeak(Peak3D* peak);
    //! Returns the given shell.
    const DShell& shell(std::size_t i) const;
    //! Returns the number of shells.
    std::size_t nShells() const;

 private:
    std::vector<DShell> _shells;
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_RESOLUTIONSHELL_H
