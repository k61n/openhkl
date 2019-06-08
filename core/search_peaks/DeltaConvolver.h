//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/search_peaks/DeltaConvolver.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_SEARCH_PEAKS_DELTACONVOLVER_H
#define CORE_SEARCH_PEAKS_DELTACONVOLVER_H

#include "core/search_peaks/AtomicConvolver.h"

namespace nsx {

class DeltaConvolver : public AtomicConvolver {

public:
    DeltaConvolver();

    DeltaConvolver(const DeltaConvolver& other) = default;

    DeltaConvolver(const std::map<std::string, double>& parameters);

    ~DeltaConvolver() = default;

    DeltaConvolver& operator=(const DeltaConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // end namespace nsx

#endif // CORE_SEARCH_PEAKS_DELTACONVOLVER_H
