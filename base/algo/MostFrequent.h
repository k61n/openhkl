//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/algo/MostFrequent.h
//! @brief     Find the most frequently occuring element of a vector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_ALGO_MOSTFREQUENT_H
#define OHKL_BASE_ALGO_MOSTFREQUENT_H

#include <optional>
#include <vector>

namespace ohkl {

std::pair<int, int> mostFrequentElement(
    std::vector<int>& vector, std::optional<int> exclude = std::nullopt);

} // namespace ohkl

#endif // OHKL_BASE_ALGO_MOSTFREQUENT_H
