/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXLIB_AUTOINDEXER_H
#define NSXLIB_AUTOINDEXER_H

#include "UnitCell.h"

#include <vector>
#include <memory>

namespace nsx {

class Experiment;

class ProgressHandler;

class Peak3D;

// note: cannot use nested classes in swig
struct IndexerParameters {
    double maxdim = 200.0;
    int nSolutions = 10;
    int nStacks = 30;
    int subdiv = 5;
    double HKLTolerance = 0.2;
    double niggliTolerance = 1e-3;
    double gruberTolerance = 4e-2;
    bool niggliReduction = false;
        //Parameters() = default;
};

class AutoIndexer {

public:
    AutoIndexer(std::shared_ptr<Experiment>& expt, const std::shared_ptr<ProgressHandler>& handler = nullptr);

    bool autoIndex(const IndexerParameters& params);
    void addPeak(const std::shared_ptr<Peak3D>& peak);
    const std::vector<std::pair<sptrUnitCell,double>>& getSolutions() const;
    //void buildSolutionsTable();
private:
    std::vector<std::shared_ptr<Peak3D>> _peaks;
    std::shared_ptr<Experiment> _experiment;
    std::vector<std::pair<sptrUnitCell,double>> _solutions;
    std::shared_ptr<ProgressHandler> _handler;
};

} // end namespace nsx

#endif // NSXLIB_AUTOINDEXER_H
