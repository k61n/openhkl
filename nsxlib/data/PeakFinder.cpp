
#include <vector>
#include <memory>

#include "IData.h"
#include "IFrameIterator.h"
#include "PeakFinder.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Experiment.h"
#include "../instrument/Detector.h"
#include "../geometry/BlobFinder.h"
#include "../utils/Types.h"
#include "../utils/erf_inv.h"
#include "../imaging/Convolver.h"
#include "../crystal/Peak3D.h"
#include "../instrument/Sample.h"

namespace nsx {

PeakFinder::PeakFinder()
{
    // default values
    _thresholdValue = 3.0;
    _thresholdType = 0;
    _confidence = nsx::getConfidence(3.0);
    _median = 0;
    _minComp = 30;
    _maxComp = 10000;
    _convolver = std::make_shared<Convolver>();
}


bool PeakFinder::find(std::vector<std::shared_ptr<DataSet>> numors)
{
    std::size_t npeaks=0;

    for (auto&& numor : numors) {
        numor->clearPeaks();

        try {
            // compute median only if necessary
            if (_thresholdType == 0) {
                _median = static_cast<int>(numor->getBackgroundLevel(_handler))+1;
            }
        }
        catch (...) {
            //qCritical() << "Error computing background level of dataset";
            return false;
        }

        // Finding peaks
        blob3DCollection blobs;

        try {
            BlobFinder blob_finder(numor);
            blob_finder.setProgressHandler(_handler);
            blob_finder.setMedian(_median);
            blob_finder.setThreshold(_thresholdValue);
            blob_finder.setMinComp(_minComp);
            blob_finder.setMaxComp(_maxComp);
            blob_finder.setConfidence(_confidence);
            blob_finder.setRelative(_thresholdType == 0);

            if ( _handler ) {
                _handler->log("Median value is: " + std::to_string(_median));
                _handler->log("threshold value is " + std::to_string(_thresholdValue));
                _handler->log("min comp is " + std::to_string(_minComp));
                _handler->log("max comp is " + std::to_string(_maxComp));
                _handler->log("confidence is " + std::to_string(_confidence));
                _handler->log("relative threshold is " + std::to_string(_thresholdType == 0));
            }

            // set image filter, if selected
            if ( _kernel ) {
                if ( !_convolver) {
                    _convolver = std::make_shared<Convolver>();
                }

                // update the convolver with the kernel
                _convolver->setKernel(_kernel->getKernel());

                // this is the filter function to be applied to each frame
                auto callback = [&] (const RealMatrix& input) -> RealMatrix {
                    RealMatrix output;
                    #pragma omp critical
                    output = _convolver->apply(input);
                    return output;
                };

                if (_handler) {
                    _handler->log("kernel " + std::string(_kernel->getName()) + " selected");
                    for (auto& it: _kernel->getParameters()) {
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

            blobs = blob_finder.find(0, numor->getNFrames());

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
        std::shared_ptr<Detector> dect = numor->getDiffractometer()->getDetector();

        AABB<double,3> dAABB(
                    Eigen::Vector3d(0,0,0),
                    Eigen::Vector3d(dect->getNCols(), dect->getNRows(), numor->getNFrames()-1)
                    );

        for (auto& blob : blobs) {

            Eigen::Vector3d center, eigenvalues;
            Eigen::Matrix3d eigenvectors;

            blob.second.toEllipsoid(nsx::getConfidence(1.0), center, eigenvalues, eigenvectors);
            auto shape = Ellipsoid3D(center, eigenvalues, eigenvectors);

            sptrPeak3D p = std::make_shared<Peak3D>(Peak3D(numor, shape));
            const auto extents = p->getShape().getAABBExtents();

            // peak too small or too large
            if (extents.maxCoeff() > 1e5 || extents.minCoeff() < 1e-5) {
                p->setSelected(false);
            }

            // peak's bounding box not completely contained in detector image
            if (!dAABB.contains(p->getShape())) {
                p->setSelected(false);
            }

            numor->addPeak(p);
            npeaks++;
            ++count;

            if ( _handler ) {
                double progress = count * 100.0 / blobs.size();
                _handler->setProgress(progress);
            }
        }

        if (_handler) {
            _handler->setStatus(("Integrating " + std::to_string(numor->getPeaks().size()) + " peaks...").c_str());
            _handler->setProgress(0);
        }
        const double scale = getScale(_confidence);
        numor->integratePeaks(scale, 2.0*scale, false, _handler);
        numor->close();
        //_ui->progressBar->setValue(++comp);
        std::cout << "Found " << numor->getPeaks().size() << " peaks." << std::endl;
    }

    if (_handler) {
        _handler->setStatus("Peak finding completed.");
        _handler->setProgress(100);
    }
    return true;
}

void PeakFinder::setHandler(const std::shared_ptr<ProgressHandler>& handler)
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

void PeakFinder::setConfidence(double confidence)
{
    _confidence = confidence;
}

double PeakFinder::confidence() const
{
    return _confidence;
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

void PeakFinder::setConvolver(std::shared_ptr<Convolver> convolver)
{
    _convolver = convolver;
}

void PeakFinder::setKernel(std::shared_ptr<ConvolutionKernel> kernel)
{
    _kernel = kernel;
}

std::shared_ptr<Convolver> PeakFinder::getConvolver()
{
    return _convolver;
}

std::shared_ptr<ConvolutionKernel> PeakFinder::getKernel()
{
    return _kernel;
}

} // end namespace nsx
