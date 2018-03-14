#include <vector>

#include "AABB.h"
#include "BlobFinder.h"
#include "Convolver.h"
#include "ConvolutionKernel.h"
#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "ErfInv.h"
#include "Experiment.h"
#include "GeometryTypes.h"
#include "IFrameIterator.h"
#include "Logger.h"
#include "MathematicsTypes.h"
#include "Peak3D.h"
#include "PeakFinder.h"
#include "ProgressHandler.h"
#include "Sample.h"
#include "StrongPeakIntegrator.h"

namespace nsx {

PeakFinder::PeakFinder()
{
    // default values
    _thresholdValue = 3.0;
    _thresholdType = 0;
    _integrationScale = 4.0;
    _backgroundScale = 6.0;
    _searchScale = nsx::getConfidence(1.0);
    _median = 0;
    _minComp = 30;
    _maxComp = 10000;
    _convolver = sptrConvolver(new Convolver());
}


PeakList PeakFinder::find(DataList numors)
{
    std::size_t npeaks=0;
    PeakList peaks;

    for (auto&& numor : numors) {
        PeakList numor_peaks;

        try {
            // compute median only if necessary
            if (_thresholdType == 0) {
                _median = static_cast<int>(numor->backgroundLevel(_handler))+1;
            }
        }
        catch (...) {
            //qCritical() << "Error computing background level of dataset";
            return peaks;
        }

        // Finding peaks
        Blob3DUMap blobs;

        try {
            BlobFinder blob_finder(numor);
            blob_finder.setProgressHandler(_handler);
            blob_finder.setMedian(_median);
            blob_finder.setThreshold(_thresholdValue);
            blob_finder.setMinComp(_minComp);
            blob_finder.setMaxComp(_maxComp);
            blob_finder.setScale(_searchScale);
            blob_finder.setRelative(_thresholdType == 0);

            if ( _handler ) {
                _handler->log("Median value is: " + std::to_string(_median));
                _handler->log("threshold value is " + std::to_string(_thresholdValue));
                _handler->log("min comp is " + std::to_string(_minComp));
                _handler->log("max comp is " + std::to_string(_maxComp));
                _handler->log("search scale is " + std::to_string(_searchScale));
                _handler->log("relative threshold is " + std::to_string(_thresholdType == 0));
            }

            // set image filter, if selected
            if ( _kernel ) {
                if ( !_convolver) {
                    _convolver = sptrConvolver(new Convolver());
                }

                // update the convolver with the kernel
                _convolver->setKernel(_kernel->matrix());

                // this is the filter function to be applied to each frame
                auto callback = [&] (const RealMatrix& input) -> RealMatrix {
                    RealMatrix output;
                    #pragma omp critical
                    output = _convolver->apply(input);
                    return output;
                };

                if (_handler) {
                    _handler->log("kernel " + std::string(_kernel->name()) + " selected");
                    for (auto& it: _kernel->parameters()) {
                        _handler->log(it.first + " " + std::to_string(it.second));
                    }
                }

                blob_finder.setFilter(callback);
            }
            else {
                if ( _handler ) {
                    _handler->log("no convolution filter selected");
                }
            }

            blobs = blob_finder.find();

            if ( _handler ) {
                _handler->log("Found " + std::to_string(blobs.size()) + " blobs");
            }
        }
        // Warning if error
        catch(std::exception& e) {
            if ( _handler ) {
                _handler->log(std::string("Peak finder caused an exception: ") + e.what());
            }
            // pass exception back to callee
            throw e;
        }

        if (_handler ) {
            _handler->setStatus("Computing bounding boxes...");
            _handler->setProgress(0);
        }

        int count = 0;
        auto dect = numor->diffractometer()->getDetector();

        AABB dAABB(Eigen::Vector3d(0,0,0),
                   Eigen::Vector3d(dect->getNCols(), dect->getNRows(), numor->nFrames()-1)
                    );

        for (auto& blob : blobs) {

            Eigen::Vector3d center, eigenvalues;
            Eigen::Matrix3d eigenvectors;

            blob.second.toEllipsoid(1.0, center, eigenvalues, eigenvectors);
            auto shape = Ellipsoid(center, eigenvalues, eigenvectors);

            auto p = sptrPeak3D(new Peak3D(numor, shape));
            p->setSelected(true);
            const auto extents = p->getShape().aabb().extents();

            // peak too small or too large
            if (extents.maxCoeff() > 1e5 || extents.minCoeff() < 1e-5) {
                p->setSelected(false);
                nsx::info() << "Peak too small or large";
            }

            // peak's bounding box not completely contained in detector image
            if (!dAABB.contains(p->getShape().aabb())) {
                p->setSelected(false);
                nsx::info() << "Peak not contained in detector region";
            }

            numor_peaks.push_back(p);
            peaks.push_back(p);

            npeaks++;
            ++count;

            if ( _handler ) {
                double progress = count * 100.0 / blobs.size();
                _handler->setProgress(progress);
            }
        }

        if (_handler) {
            _handler->setStatus(("Integrating " + std::to_string(numor_peaks.size()) + " peaks...").c_str());
            _handler->setProgress(0);
        }

        // todo: user input bkg_begin and bkg_end directly
        StrongPeakIntegrator integrator;
        integrator.integrate(numor_peaks, numor, _integrationScale, 0.5*(_integrationScale+_backgroundScale), _backgroundScale); // TODO:, _handler);
        numor->close();
        //_ui->progressBar->setValue(++comp);
        std::cout << "Found " << numor_peaks.size() << " peaks." << std::endl;
    }

    if (_handler) {
        _handler->setStatus("Peak finding completed.");
        _handler->setProgress(100);
    }
    return peaks;
}

void PeakFinder::setHandler(const sptrProgressHandler& handler)
{
    _handler = handler;
}

void PeakFinder::setThresholdValue(double threshold)
{
    _thresholdValue = threshold;
}

double PeakFinder::getThresholdValue()
{
    return _thresholdValue;
}

void PeakFinder::setThresholdType(int type)
{
    _thresholdType = type;
}

int PeakFinder::getThresholdType()
{
    return _thresholdType;
}

void PeakFinder::setSearchScale(double scale)
{
    _searchScale = scale;
}

double PeakFinder::searchScale() const
{
    return _searchScale;
}

void PeakFinder::setIntegrationScale(double scale)
{
    _integrationScale = scale;
}

double PeakFinder::integrationScale() const
{
    return _integrationScale;
}

void PeakFinder::setBackgroundScale(double scale)
{
    _backgroundScale = scale;
}

double PeakFinder::backgroundScale() const
{
    return _backgroundScale;
}

void PeakFinder::setMinComponents(int minComp)
{
    _minComp = minComp;
}

int PeakFinder::getMinComponents()
{
    return _minComp;
}

void PeakFinder::setMaxComponents(int maxComp)
{
    _maxComp = maxComp;
}

int PeakFinder::getMaxComponents()
{
    return _maxComp;
}

void PeakFinder::setConvolver(sptrConvolver convolver)
{
    _convolver = convolver;
}

void PeakFinder::setKernel(sptrConvolutionKernel kernel)
{
    _kernel = kernel;
}

sptrConvolver PeakFinder::convolver()
{
    return _convolver;
}

sptrConvolutionKernel PeakFinder::kernel()
{
    return _kernel;
}

} // end namespace nsx
