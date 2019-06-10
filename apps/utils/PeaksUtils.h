//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/utils/PeaksUtils.h
//! @brief     Defines ###THINGS###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <functional>
#include <utility>

#include "core/peak/Peak3D.h"

std::pair<double, double> dRange(const nsx::PeakList& peaks);
