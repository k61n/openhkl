//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/Profile3DIntegrator.h
//! @brief     Defines class Profile3DIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INTEGRATION_PROFILE3DINTEGRATOR_H
#define NSX_CORE_INTEGRATION_PROFILE3DINTEGRATOR_H

#include "core/shape/IPeakIntegrator.h"
#include "core/shape/ShapeModel.h"

namespace nsx {

/*! \addtogroup python_api
 *  @{*/

/*! \brief Integrate a peak using 3D profile fitting
 *
 *  Described in:
 *  - W. Kabsch, J. Appl. Crystallography, 21:916, 1988. doi:10.1107/S0021889888007903
 *  - W. Kabsch, Acta Crystallographica D, 66:133, 2010. doi:10.1107/s0907444909047374
 *
 *  Here, we minimise the chi-squared loss subject to the normalisation \f$\sum_i p_i^2 = 1\f$,
 *  Resulting in the 2x2 linear system:
 *
 * \f[
 *   \begin{bmatrix} \sum 1/v_i^2 && \sum p_i/v_i^2 \\ p_i/v_i^2 &&
 *   p_i^2/v_i^2 \end{bmatrix}
 *   \begin{bmatrix} B \\ I \end{bmatrix} =
 *   \begin{bmatrix} \sum c_i/v_i^2 \\ \sum c_ip_i/v_i^2 \end{bmatrix}
 * \f]
 *
 *  Where \f$c_i\f$, \f$b_i\f$, \f$v_i\f$ and \f$p_i\f$ are the counts, background, variance and
 * profile respectively, and I and B are the computed intensity and background respectively. These
 * are solved via the following procedure:
 *  1. Set \f$v_i = b_i\f$ as an initial guess
 *  2. Solve the linear equations by matrix inversion
 *  3. Compute updated \f$v_i = b_i - I p_i\f$
 *  4. Repeat from 2 until convergence
 */
class Profile3DIntegrator : public IPeakIntegrator {
 public:
    Profile3DIntegrator() = default;
    //! Do the integration
    bool compute(
        Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region) override;
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_INTEGRATION_PROFILE3DINTEGRATOR_H
