//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/ShapeModelBuilder.h
//! @brief     Handles ShapeModel construction for Experiment object
//! //! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OPENHKL_CORE_EXPERIMENT_SHAPEMODELBUILDER_H
#define OPENHKL_CORE_EXPERIMENT_SHAPEMODELBUILDER_H

#include "core/data/DataTypes.h"
#include "core/shape/ShapeModel.h"

namespace ohkl {

class Peak3D;
class PeakCollection;
struct ShapeModelParameters;

/*! \addtogroup python_api
 *  @{*/

/*! \brief Build a shape model
 *
 */
class ShapeModelBuilder {
 public:
    ShapeModelBuilder();

    ShapeModel integrate(
        std::vector<Peak3D*> peaks, const sptrDataSet data, bool thread_parallel = true);
    ShapeModel build(
        PeakCollection* peaks, const sptrDataSet data, bool thread_parallel = true);

    AABB getAABB();

    ShapeModelParameters* parameters() { return _params.get(); };
    void setHandler(sptrProgressHandler handler) { _handler = handler; };

 private:
    sptrProgressHandler _handler;
    std::unique_ptr<ShapeModelParameters> _params;
};

/*! @}*/
} // namespace ohkl

#endif // OPENHKL_CORE_EXPERIMENT_SHAPEMODELBUILDER_H
