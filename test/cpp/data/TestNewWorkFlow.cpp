//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/data/TestNewWorkFlow.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "base/geometry/DirectVector.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/algo/DataReaderFactory.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/ShapeCollection.h"

//#define OUTPUT_INTERMEDIATE 1


TEST_CASE("test/data/TestNewWorkFlow.cpp", "")
{
    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.getDiffractometer()));
    dataf->setName("TestNewWorkFlow");

    std::cout << "Dataset columns: " << dataf->nCols() << ", rows: " << dataf->nRows()
              << ", frames: " << dataf->nFrames() << std::endl;


#ifdef OUTPUT_INTERMEDIATE
    // export frames to gnuplot
    for (int frame = 0; frame < dataf->nFrames(); ++frame) {
        std::cout << "Saving frame " << frame << " ... ";
        std::ofstream ofstrFrame("frame_" + std::to_string(frame) + ".gpl");
        // Eigen::MatrixXi frameDat = dataf->frame(frame);
        // ofstrFrame << frameDat << std::endl;
        Eigen::MatrixXd frameDat_corr = dataf->transformedFrame(frame);
        ofstrFrame << "set xrange [0:" << dataf->nCols() << "]\n";
        ofstrFrame << "set yrange [0:" << dataf->nRows() << "]\n";
        ofstrFrame << "set cbrange [0:100]\n";
        ofstrFrame << "set logscale cb\n";
        // ofstrFrame << "plot '-' u 1:(" << dataf->nRows() << "-$2):3 matrix w image\n";
        ofstrFrame << "plot '-' matrix w image\n";
        ofstrFrame << frameDat_corr << "\n";
        ofstrFrame << "e" << std::endl;
        std::cout << "done" << std::endl;
    }
#endif

    nsx::Detector* detector = experiment.getDiffractometer()->detector();
    std::cout << "Detector distance: " << detector->distance() << ", "
              << "width: " << detector->width() << ", height: " << detector->height() << std::endl;

    experiment.addData(dataf);


    // output sample angles for each frame
    const auto& sample_gonio = experiment.getDiffractometer()->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    std::cout << "\nSample angles for each frame:" << std::endl;
    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const auto& axis = sample_gonio.axis(i);

        std::cout << axis.name() << ": ";
        for (size_t j = 0; j < dataf->nFrames(); ++j)
            std::cout << (dataf->reader()->sampleStates()[j][i] / M_PI * 180.) << ", ";
        std::cout << std::endl;
    }

    // output detector angles for each frame
    const auto& detector_gonio = experiment.getDiffractometer()->detector()->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    std::cout << "\nDetector angles for each frame:" << std::endl;
    for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
        const auto& axis = detector_gonio.axis(i);

        std::cout << axis.name() << ": ";
        for (size_t j = 0; j < dataf->nFrames(); ++j)
            std::cout << (dataf->reader()->detectorStates()[j][i] / M_PI * 180.) << ", ";
        std::cout << std::endl;
    }
    std::cout << std::endl;


    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);

    auto callback = [progressHandler]() {
        auto log = progressHandler->getLog();
        for (const auto& msg : log)
            std::cout << msg << std::endl;
    };

    progressHandler->setCallback(callback);

    // #########################################################
    // test the finder
    nsx::DataList numors;
    numors.push_back(dataf);

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular", {});

    nsx::PeakFinder* peak_finder = experiment.peakFinder();

    auto finder_params = peak_finder->parameters();
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->peak_end = 1.0;
    finder_params->maximum_frames = 10;
    finder_params->frames_begin = 0;
    finder_params->frames_end = dataf->nFrames();
    finder_params->threshold = 10;

    peak_finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
    peak_finder->setHandler(progressHandler);
    peak_finder->find(numors);

    auto found_peaks = peak_finder->currentPeaks();

#ifdef OUTPUT_INTERMEDIATE
    for (const nsx::Peak3D* pk : found_peaks) {
        if (!pk->enabled())
            continue;

        nsx::Ellipsoid elli_real = pk->shape();
        nsx::Ellipsoid elli_recip = pk->qShape();
        nsx::Intensity intensity = pk->rawIntensity();

        std::cout << "real peak: " << elli_real.center().transpose() << ", ";
        std::cout << "recip peak: " << elli_recip.center().transpose() << ", ";
        std::cout << "intensity: " << intensity.value() << " +- " << intensity.sigma() << std::endl;
    }
#endif


    try {
        CHECK(static_cast<int>(found_peaks.size()) >= 0);
    } catch (...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    CHECK(found_peaks.size() >= 800);

    nsx::IPeakIntegrator* integrator =
        experiment.getIntegrator(nsx::IntegratorType::PixelSum);
    nsx::IntegrationParameters params{};
    params.peak_end = 3.0;
    params.bkg_begin = 3.5;
    params.bkg_end = 4.0;
    integrator->setParameters(params);
    integrator->setHandler(progressHandler);
    experiment.integrateFoundPeaks(nsx::IntegratorType::PixelSum);
    experiment.acceptFoundPeaks("found_peaks");

    // #########################################################
    // Filter the peaks
    nsx::PeakFilter* peak_filter = experiment.peakFilter();
    peak_filter->flags()->d_range = true;
    peak_filter->parameters()->d_min = 1.5;
    peak_filter->parameters()->d_max = 50.0;

    nsx::PeakCollection* found_collection = experiment.getPeakCollection("found_peaks");
    peak_filter->resetFiltering(found_collection);
    peak_filter->filter(found_collection);

    experiment.acceptFilter("filtered_peaks", found_collection);

    auto peakCollection = experiment.getPeakCollection("filtered_peaks");
    auto filteredPeaks = peakCollection->getPeakList();

    for (const nsx::Peak3D* pk : filteredPeaks) {
        if (!pk->enabled())
            continue;

        const nsx::Ellipsoid& elli_real = pk->shape();
        const nsx::Ellipsoid& elli_recip = pk->qShape();
        const nsx::Intensity intensity = pk->rawIntensity();
        const nsx::Intensity background = pk->meanBackground();

        std::cout << "real peak: " << elli_real.center().transpose() << ", ";
        std::cout << "recip peak: " << elli_recip.center().transpose() << ", ";
        std::cout << "intensity: " << intensity.value() << "+-" << intensity.sigma() << ", ";
        std::cout << "background: " << background.value() << "+-" << background.sigma()
                  << std::endl;
    }

#ifdef OUTPUT_INTERMEDIATE
    peakCollection->exportToGnuplot("peaks_recip.gpl", true);
    peakCollection->exportToGnuplot("peaks_real.gpl", false);
#endif

    CHECK(filteredPeaks.size() >= 100);

    // #########################################################
    // at this stage we have the peaks, now we index
    nsx::AutoIndexer* auto_indexer = experiment.autoIndexer();
    nsx::PeakCollection* filtered_peaks = experiment.getPeakCollection("filtered_peaks");

    CHECK_NOTHROW(auto_indexer->autoIndex(filtered_peaks->getPeakList()));
    CHECK(auto_indexer->solutions().size() > 1);

    std::cout << "Number of solutions: " << auto_indexer->solutions().size() << std::endl;
    for (std::size_t solidx = 0; solidx < auto_indexer->solutions().size(); ++solidx) {
        const auto& solution = auto_indexer->solutions()[solidx];
        const auto& cell = solution.first;
        double prob = solution.second;

        std::cout
            << "\n================================================================================"
            << std::endl;
        std::cout << "Solution " << solidx << " has " << prob << "% probability." << std::endl;
        std::cout << "Unit cell:" << std::endl;
        cell->printSelf(std::cout);
        std::cout
            << "================================================================================\n"
            << std::endl;
    }


    auto solution = auto_indexer->solutions().front();

    // correctly indexed at least 92% of peaks
    CHECK(solution.second > 92.0);

    // set unit cell
    const auto cell = solution.first;
    cell->printSelf(std::cout);

    for (const auto& peak : filtered_peaks->getPeakList()) {
        peak->setUnitCell(cell.get());
        // std::cout << "recip peak: " << peak->q().rowVector() << std::endl;
    }

    // reintegrate peaks
    params.peak_end = 3.0;
    params.bkg_begin = 4.0;
    params.bkg_end = 5.0;
    integrator->setParameters(params);
    experiment.integrateFoundPeaks(nsx::IntegratorType::PixelSum);

    // #########################################################
    // compute shape library
    int n_selected = 0;
    for (auto peak : filtered_peaks->getPeakList()) {
        std::vector<nsx::ReciprocalVector> q_vectors;
        q_vectors.push_back(peak->q());
        auto events = nsx::algo::qs2events(
            q_vectors, dataf->instrumentStates(), dataf->detector(), dataf->nFrames());

        if (events.empty())
            continue;

        ++n_selected;

        const Eigen::Vector3d p0 = peak->shape().center();
        Eigen::Vector3d p1;

        // q could cross Ewald sphere multiple times, so find best match
        double diff = 1e200; // going to find smaller value
        for (const auto& event : events) {
            const Eigen::Vector3d pnew = {event._px, event._py, event._frame};
            if ((pnew - p0).squaredNorm() < diff) {
                diff = (pnew - p0).squaredNorm();
                p1 = pnew;
            }
        }

        const Eigen::RowVector3d q0 = nsx::Peak3D(dataf, nsx::Ellipsoid(p0, 1.0)).q().rowVector();
        const Eigen::RowVector3d q1 = nsx::Peak3D(dataf, nsx::Ellipsoid(p1, 1.0)).q().rowVector();

        CHECK(p0(0) == Approx(p1(0)).epsilon(3e-2));
        CHECK(p0(1) == Approx(p1(1)).epsilon(3e-2));
        CHECK(p0(2) == Approx(p1(2)).epsilon(3e-2));

        CHECK(q0(0) == Approx(q1(0)).epsilon(2e-2));
        CHECK(q0(1) == Approx(q1(1)).epsilon(2e-2));
        CHECK(q0(2) == Approx(q1(2)).epsilon(2e-2));

        /*std::cout << q0 << std::endl;
        std::cout << q1 << std::endl;
        std::cout << p0.transpose() << std::endl;
        std::cout << p1.transpose() << "\n" << std::endl;*/
    }

    CHECK(n_selected > 600);
}
