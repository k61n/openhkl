//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/ParallelFor.h
//! @brief     Defines class ParallelFor
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_UTILS_PARALLELFOR_H
#define OHKL_BASE_UTILS_PARALLELFOR_H

#include <algorithm>
#include <functional>
#include <thread>
#include <vector>

#include <iostream>

namespace ohkl {

/// @param[in] nb_elements : size of your for loop
/// @param[in] functor(start, end) :
/// your function processing a sub chunk of the for loop.
/// "start" is the first index to process (included) until the index "end"
/// (excluded)
/// @code
///     for(int i = start; i < end; ++i)
///         computation(i);
/// @endcode
/// @param use_threads : enable / disable threads.
///
///
static void parallel_for(
    unsigned int nb_elements, std::function<void (int start, int end)> functor,
    bool use_threads = true, unsigned int max_threads = 0)
{
    unsigned int nb_threads_hint = std::thread::hardware_concurrency();
    unsigned int nb_threads = nb_threads_hint == 0 ? 8 : nb_threads_hint;
    if (max_threads > 0) // Do not exceed the maximum number of threads (prevents OOM)
        nb_threads = nb_threads > max_threads ? max_threads : nb_threads;

    unsigned int batch_size = nb_elements / nb_threads;
    unsigned int batch_remainder = nb_elements % nb_threads;

    std::vector<std::thread> my_threads(nb_threads);

    if (use_threads) {
        // Multithread execution
        for(unsigned int i = 0; i < nb_threads; ++i) {
            int start = i * batch_size;
            my_threads[i] = std::thread(functor, start, start+batch_size);
        }
    }
    else {
        // Single thread execution (for easy debugging)
        for(unsigned int i = 0; i < nb_threads; ++i){
            int start = i * batch_size;
            functor( start, start+batch_size );
        }
    }

    // Deform the elements left
    int start = nb_threads * batch_size;
    functor( start, start+batch_remainder);

    // Wait for the other thread to finish their task
    if (use_threads)
        std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));
}

} // namespace ohkl

#endif // OHKL_BASE_UTILS_PARALLELFOR_H
