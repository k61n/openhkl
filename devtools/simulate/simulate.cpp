#include <random>

#include <Eigen/Dense>

#include "base/utils/Random.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"
#include "core/data/FakeDataReader.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/geometry/Ellipsoid.h"
#include "core/peak/Peak3D.h"
#include "core/rec_space/ReciprocalVector.h"

//! Correct the generated image by adding baseline, gain, and optional Poisson
//! noise
void correct_image(Eigen::ArrayXXd& data, double gain, double baseline, bool noise)
{
    const int nrows = data.rows();
    const int ncols = data.cols();

    data += baseline / gain;

    if (noise) {
        for (auto i = 0; i < ncols; ++i) {
            for (auto j = 0; j < nrows; ++j) {
                const double x = Random::gauss(0., 1.);
                const double d = data(j, i);
                const double scale = d <= 0 ? 0.0 : std::sqrt(data(j, i));
                data(j, i) += scale * x;
            }
        }
    }

    data *= gain;
}

//! Generate simulated frames
std::vector<Eigen::ArrayXXd> generate_frames(
    nsx::PeakList& peaks, int nrows, int ncols, size_t frame_min, size_t frame_max,
    const Eigen::Matrix3d& A, const Eigen::Matrix3d& B, const nsx::SpaceGroup& grp)
{
    std::vector<Eigen::ArrayXXd> images;

    for (auto z = frame_min; z < frame_max; ++z) {
        Eigen::ArrayXXd image(nrows, ncols);
        image.setZero();
        images.emplace_back(std::move(image));
    }

    const double Imax = 200 * 200 * 5;
    const size_t nsigma = 4;

    Eigen::Matrix3d BI = B.inverse();
    auto elements = grp.groupElements();
    std::vector<Eigen::Matrix3d> gs;

    for (auto elem : elements) {
        gs.push_back(B * elem.getRotationPart() * BI);
    }

    for (auto peak : peaks) {
        auto aabb = peak->shape().aabb();
        auto shape = peak->shape();
        auto center = shape.center();
        auto inv_cov = shape.metric();
        auto extents = aabb.extents();

        double I = 0;
        Eigen::Vector3d q = peak->q().rowVector().transpose();

        for (auto g : gs) {
            auto gq = g * q;
            I += Imax * std::exp(-0.5 * gq.dot(A * gq));
        }

        I /= gs.size();

        peak->setRawIntensity(1.0);
        const double scale = peak->correctedIntensity().value();

        auto xmin = int(center(0) - nsigma * extents(0) / 2);
        auto xmax = int(center(0) + nsigma * extents(0) / 2);

        auto ymin = int(center(1) - nsigma * extents(1) / 2);
        auto ymax = int(center(1) + nsigma * extents(1) / 2);

        auto zmin = int(center(2) - nsigma * extents(2) / 2);
        auto zmax = int(center(2) + nsigma * extents(2) / 2);

        xmin = std::max(0, xmin);
        xmax = std::min(ncols, xmax);

        ymin = std::max(0, ymin);
        ymax = std::min(nrows, ymax);

        zmin = std::max(int(frame_min), zmin);
        zmax = std::min(int(frame_max), zmax);

        if (xmin >= xmax || ymin >= ymax || zmin >= zmax) {
            continue;
        }

        std::vector<Eigen::ArrayXXd> peak_images;
        double sum = 0.0;
        Eigen::Matrix3d M = peak->shape().metric();

        for (auto k = zmin; k < zmax; ++k) {
            const double dz = k - center(2);
            Eigen::ArrayXXd im(ymax - ymin, xmax - xmin);
            im.setZero();

            for (auto i = xmin; i < xmax; ++i) {
                const double dx = i - center(0);

                for (auto j = ymin; j < ymax; ++j) {
                    const double dy = j - center(1);

                    double arg = dx * dx * M(0, 0) + dy * dy * M(1, 1) + dz * dz * M(2, 2);
                    arg += 2 * dx * dy * M(0, 1) + 2 * dx * dz * M(0, 2) + 2 * dy * dz * M(1, 2);

                    im(j - ymin, i - xmin) = std::exp(-0.5 * arg);
                }
            }
            sum += im.sum();
            peak_images.emplace_back(std::move(im));
        }

        for (auto k = zmin; k < zmax; ++k) {
            images[k].block(ymin, xmin, ymax - ymin, xmax - xmin) +=
                peak_images[k - zmin] * I / scale / sum;
        }
    }

    std::cout << "returning..." << std::endl;
    return images;
}

double background()
{
    return 40.0;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: simulate infile outfile" << std::endl;
        return 0;
    }

    const double pi2 = M_PI / 2.0;
    const double wavelength = 2.67;
    const double dmin = 1.5;
    const double dmax = 50.0;
    const char* infile = argv[1];
    const char* outfile = argv[2];
    const char* group_name = "P 21 21 21";
    auto group = nsx::SpaceGroup(group_name);

    auto expt = std::make_shared<nsx::Experiment>("Simulated", "BioDiff2500");
    auto diff = expt->diffractometer();
    auto reader = std::make_shared<nsx::FakeDataReader>(infile, diff);
    auto data = std::make_shared<nsx::DataSet>(reader);
    expt->addData(data);

    Eigen::Matrix3d A, C;
    A << 43.0, 0.0, 0.0, 0.0, 53.0, 0.0, 0.0, 0.0, 61;

    C = Eigen::Matrix3d::Random();
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(C * C.transpose());
    Eigen::Matrix3d U = solver.eigenvectors();

    auto uc = std::make_shared<nsx::UnitCell>(U * A);
    uc->setSpaceGroup(group);
    Eigen::Matrix3d B = uc->reciprocalBasis();

    auto det_shape = nsx::Ellipsoid({800.0, 450.0, 10.0}, 5.0);
    auto peak = std::make_shared<nsx::Peak3D>(data, det_shape);

    auto hkls = uc->generateReflectionsInShell(dmin, dmax, wavelength);
    std::vector<nsx::ReciprocalVector> qs;

    for (auto hkl : hkls) {
        Eigen::RowVector3d q = hkl.rowVector().cast<double>() * B;
        qs.push_back(nsx::ReciprocalVector(q));
    }

    auto events = data->events(qs);

    const double sigmaD = 0.3;
    const double sigmaM = 0.2;

    nsx::PeakList peaks;

    for (auto&& event : events) {
        Eigen::Vector3d center = {event.px, event.py, event.frame};
        auto peak = std::make_shared<nsx::Peak3D>(data, nsx::Ellipsoid(center, 5.0));
        auto coords = nsx::PeakCoordinateSystem(peak);
        auto shape = coords.detectorShape(sigmaD, sigmaM);
        peak->setShape(shape);
        peaks.push_back(peak);
    }

    Eigen::Matrix3d R = Eigen::Matrix3d::Random();
    R.normalize();

    A = 0.5 * Eigen::Matrix3d::Identity() + R * R.transpose();
    A.normalize();
    A *= 10.0;

    auto&& simulated_frames =
        generate_frames(peaks, data->nRows(), data->nCols(), 0, data->nFrames(), A, B, group);
    std::cout << "simulated frames " << simulated_frames.size() << std::endl;

    for (size_t i = 0; i < simulated_frames.size(); ++i) {
        simulated_frames[i] += 20.0;
        correct_image(simulated_frames[i], 8.0, 200.0, true);
        std::cout << simulated_frames[i].sum() << std::endl;
        reader->setData(i, simulated_frames[i].cast<int>());
    }

    data->saveHDF5(outfile);

    return 0;
}
