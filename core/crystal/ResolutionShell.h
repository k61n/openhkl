//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/crystal/ResolutionShell.h
//! @brief     Defines class ResolutionShell
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_CRYSTAL_RESOLUTIONSHELL_H
#define CORE_CRYSTAL_RESOLUTIONSHELL_H

#include "core/peak/Peak3D.h"

namespace nsx {

//! \class DShell
//! Utility structure to store a list of peaks with resolution in the
//! range [dmin, dmax].
struct DShell {
    //! Minimum value of d, i.e. maximum resolution
    double dmin;
    //! Maximum value of d, i.e. minimum resolution
    double dmax;
    //! List of peaks contained within the shell
    PeakList peaks;
};

//! Class to split a set of peaks into a number of shells based on resolution.
class ResolutionShell {
public:
    //! Construct the given number of cells with abolute minimum dmin and absolute maximum dmax.
    ResolutionShell(double dmin, double dmax, size_t num_shells);
    //! Add a peak to the list of shells.
    //! It will automatically be added to the appropriate shell.
    void addPeak(const sptrPeak3D& peak);
    //! Returns the given shell.
    const DShell& shell(size_t i) const;
    //! Returns the number of shells.
    size_t nShells() const;

private:
    std::vector<DShell> _shells;
};

} // namespace nsx

#endif // CORE_CRYSTAL_RESOLUTIONSHELL_H
