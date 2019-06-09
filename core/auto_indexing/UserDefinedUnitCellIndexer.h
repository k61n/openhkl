//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/auto_indexing/UserDefinedUnitCellIndexer.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_AUTO_INDEXING_USERDEFINEDUNITCELLINDEXER_H
#define CORE_AUTO_INDEXING_USERDEFINEDUNITCELLINDEXER_H



#include "core/crystal/UnitCell.h"
#include "core/utils/Units.h"

namespace nsx {

struct UserDefinedUnitCellIndexerParameters {

    double distance_tolerance = 5.0e-2;

    double angular_tolerance = 5.0e-2;

    bool niggli_only = false;

    double niggli_tolerance = 1.0e-3;
    double gruber_tolerance = 4.0e-2;

    size_t n_solutions = 10;
    double indexing_tolerance = 2.0e-1;
    double indexing_threshold = 9.0e-1;

    size_t max_n_q_vectors = 300;

    double wavelength = 1.0;

    double a = 10.0;
    double b = 10.0;
    double c = 10.0;

    double alpha = 90.0 * nsx::deg;
    double beta = 90.0 * nsx::deg;
    double gamma = 90.0 * nsx::deg;

    void checkParameters() const;
};

class UserDefinedUnitCellIndexer {

public:
    UserDefinedUnitCellIndexer();

    UserDefinedUnitCellIndexer(const UserDefinedUnitCellIndexerParameters& parameters);

    UserDefinedUnitCellIndexer(const UserDefinedUnitCellIndexer& other) = default;

    UserDefinedUnitCellIndexer(UserDefinedUnitCellIndexer&& other) = default;

    ~UserDefinedUnitCellIndexer() = default;

    UserDefinedUnitCellIndexer& operator=(const UserDefinedUnitCellIndexer& other) = default;

    UserDefinedUnitCellIndexer& operator=(UserDefinedUnitCellIndexer&& other) = default;

    const UserDefinedUnitCellIndexerParameters& parameters() const;
    void setParameters(const UserDefinedUnitCellIndexerParameters& parameters);

    void run();

    const std::vector<std::pair<sptrUnitCell, double>>& solutions() const;

    void setPeaks(const PeakList& peaks);

private:
    void index();

    void rankUnitCells();

    void refineUnitCells();

    void removeBadUnitCells();

private:
    UserDefinedUnitCellIndexerParameters _parameters;

    std::vector<std::pair<sptrUnitCell, double>> _solutions;

    PeakList _peaks;
};

} // end namespace nsx

#endif // CORE_AUTO_INDEXING_USERDEFINEDUNITCELLINDEXER_H
