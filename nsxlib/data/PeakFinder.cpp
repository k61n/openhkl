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
#include "MathematicsTypes.h"
#include "Peak3D.h"
#include "PeakFinder.h"
#include "ProgressHandler.h"
#include "Sample.h"

namespace nsx {

PeakFinder::PeakFinder()
{
    // default values
    _thresholdValue = 3.0;
    _thresholdType = 0;
    _integrationConfidence = nsx::getConfidence(3.0);
    _searchConfidence = nsx::getConfidence(1.0);
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

        auto dectector = numor->diffractometer()->getDetector();
        int nrows = dectector->getNRows();
        int ncols = dectector->getNCols();
        int nframes = numor->nFrames();

        // Finding peaks
        std::map<int,Blob3D> blobs;

        try {
            BlobFinder blob_finder(numor);
            blob_finder.setProgressHandler(_handler);
            blob_finder.setMedian(_median);
            blob_finder.setThreshold(_thresholdValue);
            blob_finder.setMinComp(_minComp);
            blob_finder.setMaxComp(_maxComp);
            blob_finder.setConfidence(_searchConfidence);
            blob_finder.setRelative(_thresholdType == 0);

            if ( _handler ) {
                _handler->log("Median value is: " + std::to_string(_median));
                _handler->log("threshold value is " + std::to_string(_thresholdValue));
                _handler->log("min comp is " + std::to_string(_minComp));
                _handler->log("max comp is " + std::to_string(_maxComp));
                _handler->log("search confidence is " + std::to_string(_searchConfidence));
                _handler->log("relative threshold is " + std::to_string(_thresholdType == 0));
            }

            // set image filter, if selected
            if ( _kernel ) {
                if ( !_convolver) {
                    _convolver = sptrConvolver(new Convolver());
                }

                // update the convolver with the kernel
                _convolver->setKernel(_kernel->matrix(nrows,ncols));

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

        AABB dAABB(Eigen::Vector3d(0,0,0),
                   Eigen::Vector3d(ncols, nrows, nframes-1)
                    );

        for (auto& blob : blobs) {

            Eigen::Vector3d center, eigenvalues;
            Eigen::Matrix3d eigenvectors;

            blob.second.toEllipsoid(nsx::getConfidence(1.0), center, eigenvalues, eigenvectors);
            auto shape = Ellipsoid(center, eigenvalues, eigenvectors);

            auto p = sptrPeak3D(new Peak3D(numor, shape));
            const auto extents = p->getShape().aabb().extents();

            // peak too small or too large
            if (extents.maxCoeff() > 1e5 || extents.minCoeff() < 1e-5) {
                p->setSelected(false);
            }

            // peak's bounding box not completely contained in detector image
            if (!dAABB.contains(p->getShape().aabb())) {
                p->setSelected(false);
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
        const double bkg_begin = getScale(_integrationConfidence)+3;
        numor->integratePeaks(numor_peaks, bkg_begin, bkg_begin+3, _handler);
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

void PeakFinder::setSearchConfidence(double confidence)
{
    _searchConfidence = confidence;
}

double PeakFinder::searchConfidence() const
{
    return _searchConfidence;
}

void PeakFinder::setIntegrationConfidence(double confidence)
{
    _integrationConfidence = confidence;
}

double PeakFinder::integrationConfidence() const
{
    return _integrationConfidence;
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
