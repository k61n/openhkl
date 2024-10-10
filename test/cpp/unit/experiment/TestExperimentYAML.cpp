//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestExperimentYAML.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/algo/AutoIndexer.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/Integrator.h"
#include "core/experiment/PeakFinder.h"
#include "core/statistics/PeakMerger.h"
#include "core/shape/Predictor.h"
#include "core/experiment/ShapeModelBuilder.h"

TEST_CASE("test/data/TestExperimentYAML.cpp", "")
{
    const double eps = 1.0e-5;
    const std::string yml_file = "test.yml";

    ohkl::Experiment expt1("test", "BioDiff");

    CHECK(expt1.peakFinder()->parameters()->threshold == Approx(1.0).epsilon(eps));
    CHECK(expt1.autoIndexer()->parameters()->indexingTolerance == Approx(0.2).epsilon(eps));
    CHECK(expt1.shapeModelBuilder()->parameters()->nbins_z == 6);
    CHECK(expt1.predictor()->parameters()->d_min == Approx(1.5).epsilon(eps));
    CHECK(expt1.integrator()->parameters()->peak_end == Approx(3.0).epsilon(eps));
    CHECK(expt1.peakMerger()->parameters()->n_shells == 10);

    expt1.peakFinder()->parameters()->threshold = 0.8;
    expt1.autoIndexer()->parameters()->indexingTolerance = 0.1;
    expt1.shapeModelBuilder()->parameters()->nbins_z = 8;
    expt1.predictor()->parameters()->d_min = 1.8;
    expt1.integrator()->parameters()->peak_end = 2.5;
    expt1.peakMerger()->parameters()->n_shells = 15;

    expt1.saveToYaml(yml_file);

    ohkl::Experiment expt2("test", "BioDiff");
    expt2.readFromYaml(yml_file);

    CHECK(expt2.peakFinder()->parameters()->threshold == Approx(0.8).epsilon(eps));
    CHECK(expt2.autoIndexer()->parameters()->indexingTolerance == Approx(0.1).epsilon(eps));
    CHECK(expt2.shapeModelBuilder()->parameters()->nbins_z == 8);
    CHECK(expt2.predictor()->parameters()->d_min == Approx(1.8).epsilon(eps));
    CHECK(expt2.integrator()->parameters()->peak_end == Approx(2.5).epsilon(eps));
    CHECK(expt2.peakMerger()->parameters()->n_shells == 15);
}
