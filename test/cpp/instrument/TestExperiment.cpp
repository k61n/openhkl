//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/instrument/TestExperiment.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <stdexcept>

#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/shape/IPeakIntegrator.h"

TEST_CASE("test/instrument/TestExperiment.cpp", "")
{
    nsx::Experiment exp("my-exp", "D10");
    nsx::sptrDataSet data;

    CHECK(exp.name() == "my-exp");

    // Change the name of the experiment
    exp.setName("toto");
    CHECK(exp.name() == "toto");

    // Add some data
    const std::string data_name = "D10_ascii_example";
    try {
        data = nsx::sptrDataSet(
            nsx::DataReaderFactory().create("", data_name, exp.diffractometer()));
    } catch (std::exception& e) {
        FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        FAIL("unknown exception");
    }

    exp.addData(data);

    // Check that adding the same data is now taken into account
    exp.addData(data);
    exp.addData(data);
    exp.addData(data);

    CHECK(exp.getData(data_name)->filename() == data_name);

    // Remove the data from the experiment
    exp.removeData(data_name);
}
