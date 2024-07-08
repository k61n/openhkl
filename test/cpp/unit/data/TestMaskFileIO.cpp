//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestMaskFileIO.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/geometry/AABB.h"
#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/mask/IMask.h"
#include "core/experiment/MaskExporter.h"
#include "core/experiment/MaskImporter.h"


TEST_CASE("test/data/TestMaskFileIO.cpp", "")
{
    const double eps = 1.0e-5;

    const int nframes = 169;
    const std::string filename = "masks_test.yml";

    const ohkl::AABB aabb1{{1725, 0, 0}, {1740, 900, nframes}};
    const ohkl::AABB aabb2{{1200, 400, 0}, {1300, 500, nframes}};

    ohkl::BoxMask box_mask(aabb1);
    ohkl::EllipseMask ellipse_mask(aabb2);

    std::vector<ohkl::IMask*> masks;
    masks.push_back(&box_mask);
    masks.push_back(&ellipse_mask);

    ohkl::MaskExporter mask_exporter(masks);
    mask_exporter.exportToFile(filename);

    ohkl::MaskImporter mask_importer(filename, nframes);
    std::vector<ohkl::IMask*> masks_from_file = mask_importer.getMasks();

    ohkl::IMask* box_mask_from_file = masks_from_file[0];
    ohkl::IMask* ellipse_mask_from_file = masks_from_file[1];

    CHECK(box_mask_from_file->aabb().lower()[0] == Approx(1725).epsilon(eps));
    CHECK(box_mask_from_file->aabb().upper()[1] == Approx(900).epsilon(eps));
    CHECK(box_mask_from_file->aabb().upper()[2] == Approx(nframes).epsilon(eps));
    CHECK(ellipse_mask_from_file->aabb().lower()[0] == Approx(1200).epsilon(eps));
    CHECK(ellipse_mask_from_file->aabb().upper()[1] == Approx(500).epsilon(eps));
    CHECK(ellipse_mask_from_file->aabb().upper()[2] == Approx(nframes).epsilon(eps));
}
