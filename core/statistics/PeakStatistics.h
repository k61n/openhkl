//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakStatistics.h
//! @brief     Defines class PeakStatistics
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_PEAKSTATISTICS_H
#define OHKL_CORE_STATISTICS_PEAKSTATISTICS_H

#include <gsl/gsl_histogram.h>

#include <vector>
#include <map>
#include <string>

namespace ohkl {

class PeakCollection;

/*! \addtogroup python_api
 *  @{*/

//! Specify the type of histogram to generate
enum class PeakHistogramType {
    Intensity,
    Sigma,
    Strength,
    Count,
};


/*! \brief Compute peaks statistics to facilitate outlier rejection
 *
 * Class to manage peak statistics.
 */
class PeakStatistics {
 public:
    PeakStatistics() = default;
    ~PeakStatistics();

    //! Set peak collection
    void setPeakCollection(PeakCollection* peaks, PeakHistogramType type);
    //! Get a pointer to the current histogram
    gsl_histogram* histogram() const { return _current_histogram; };
    //! Get descriptions for histogram types
    const std::map<PeakHistogramType, std::string>& getHistoStrings() { return _histogram_strings; };
    //! Get the maximum value from the data series
    double maxValue() const { return _max_value; };
    //! Get the minimum value from the data series (could be < 0)
    double minValue() const { return _min_value; };
    //! Get the maximum count from the histogram
    double maxCount() const;
    //! Get the minimum count from the histogram (could be < 0)
    double minCount() const;
    //! Compute a histogram given the current set of points
    gsl_histogram* computeHistogram(std::size_t nbins);

 private:
    //! Allocate histogram memory
    void _initHistogram(std::size_t nbins);
    //! Free histogram memory
    void _clearHistogram();
    //! Get the data for the histogram from the peak collection
    std::vector<double> _getPeakData(PeakHistogramType type);

    PeakCollection* _peaks;

    std::size_t _nbins;
    double _max_value;
    double _min_value;
    gsl_histogram* _current_histogram;
    std::vector<double> _peak_data;

    static const std::map<PeakHistogramType, std::string> _histogram_strings;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_PEAKSTATISTICS_H
