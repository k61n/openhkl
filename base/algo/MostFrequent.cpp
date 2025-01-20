//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/algo/MostFrequent.cpp
//! @brief     Implements finding the most frequent element of a vector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/algo/MostFrequent.h"

#include <algorithm>

namespace ohkl {

std::pair<int, int> mostFrequentElement(std::vector<int>& vector, std::optional<int> exclude)
{
    // Sort the vector
    std::sort(std::begin(vector), std::end(vector));

    int current_freq = 0;
    int max_freq = 0;
    int most_freq_value = vector.front();
    int last_seen_value = vector.front();

    for (int value : vector) {
        if (exclude != std::nullopt && exclude.value() == value)
            continue;
        if (value == last_seen_value) ++current_freq;
        else {
            if (current_freq > max_freq) {
                max_freq = current_freq;
                most_freq_value = last_seen_value;
            }
            last_seen_value = value;
            current_freq = 1;
        }
    }

    if (current_freq > max_freq) {
        max_freq = current_freq;
        most_freq_value = last_seen_value;
    }

    return {most_freq_value, max_freq};
}

} // namespace ohkl
