#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "DataTypes.h"

namespace nsx {

//! \class PeakFilter
//! \brief Class to remove "bad" peaks based on various critera.
class PeakFilter {

public:
    //! Apply the filter to the given data set. Returns the number of removed peaks
    int apply(sptrDataSet data) const;

public:
    //! Remove peaks which are not indexed (incommensurate).
    bool _removeUnindexed;
    //! Remove peaks which are masked.
    bool _removeMasked;
    //! Remove peaks which are not selected.
    bool _removeUnselected;
    //! Remove peaks based on cutoff for \f$I/\sigma_I\f$.
    bool _removeIsigma;
    //! Remove peak based on cutoff for raw p-value.
    bool _removePValue;
    //! Remove peaks if they are overlapping.
    bool _removeOverlapping;
    //! Remove peaks with \f$d < d_{\mathrm{min}}\f$.
    bool _removeDmin;
    //! Remove peaks with \f$d > d_{\mathrm{max}}\f$.
    bool _removeDmax;
    //! Remove reflections which are forbidden by spacegroup symmetry.
    bool _removeForbidden;
    //! Remove peaks based on cutoff for merged p-value (chi-squared test).
    bool _removeMergedP;
    //! Value of the \f$I/\sigma_I\f$ cutoff.
    double _Isigma;
    //! Value of the minimum d cutoff.
    double _dmin;
    //! Value of the maximum d cutoff.
    double _dmax;
    //! Value of the raw p-value cutoff.
    double _pvalue;
    //! Value of the merged p-value cutoff.
    double _mergedP;
};

} // end namespace nsx
