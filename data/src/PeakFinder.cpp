
#include <vector>
#include <memory>

#include "IData.h"
#include "Experiment.h"
#include "Detector.h"
#include "BlobFinder.h"
#include "PeakFinder.h"
#include "Types.h"
#include "Convolver.h"
#include "Sample.h"
#include "IFrameIterator.h"



using SX::Types::RealMatrix;
using SX::Imaging::Convolver;
using SX::Utils::ProgressHandler;

namespace SX {

namespace Data {

PeakFinder::PeakFinder()
{
    // default values
    _thresholdValue = 3.0;
    _thresholdType = 0;
    _confidence = 0.997;
    _median = 0;

    _minComp = 30;
    _maxComp = 10000;
}


bool PeakFinder::find(std::vector<std::shared_ptr<IData>> numors)
{
    // needed to compile:
    //double threshold, confidence;
    //int minComp, maxComp;
    //std::unique_ptr<Convolver> convolver(new Convolver);


    //qWarning() << "Peak find algorithm: Searching peaks in " << numors.size() << " files";
    //int max=numors.size();
    int max = 100;

    //QCoreApplication::processEvents();
    //_ui->progressBar->setEnabled(true);
    //_ui->progressBar->setMaximum(max);
    //_ui->progressBar->setValue(0);

    std::size_t npeaks=0;
    int comp = 0;

    //QProgressDialog progressDialog(this);


    for (auto numor : numors)
    {
        numor->clearPeaks();

        numor->readInMemory(_handler);

        try {
            //progressDialog->setMaximum(100);
            //progressDialog->setLabelText("Computing background level...");
            //progressDialog->show();

            // jmf: why do we round median to an integer??
            _median = static_cast<int>(numor->getBackgroundLevel(_handler))+1;

            //progressDialog->close();
        }
        catch (...) {
            //qCritical() << "Error computing background level of dataset";
            return false;
        }


        //qDebug() << ">>>> the background level is " << median;
        //qDebug() << ">>>> finding blobs... ";

        // Finding peaks
        SX::Geometry::blob3DCollection blobs;
        try
        {
            //progressDialog->setLabelText("Finding blobs...");
            //progressDialog->setValue(0);

            SX::Geometry::BlobFinder blob_finder(numor);

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
                _handler->log("min comp is" + std::to_string(_minComp));
                _handler->log("max comp is " + std::to_string(_maxComp));
                _handler->log("confidence is " + std::to_string(_confidence));
                _handler->log("relative threshold is" + std::to_string(_thresholdType == 0));
            }

            // set image filter, if selected
            if ( _kernel ) {

                if ( !_convolver)
                    _convolver = std::shared_ptr<SX::Imaging::Convolver>(new SX::Imaging::Convolver);

                // update the convolver with the kernel
                _convolver->setKernel(_kernel->getKernel());

                // this is the filter function to be applied to each frame
                auto callback = [&] (const RealMatrix& input) -> RealMatrix
                {
                    return _convolver->apply(input);
                };

                if (_handler) {
                    _handler->log("kernel " + std::string(_kernel->getName()) + " selected");
                    for (auto& it: _kernel->getParameters())
                        _handler->log(it.first + " " + std::to_string(it.second));
                }

                blob_finder.setFilter(callback);
            }
            else {
                if ( _handler )
                    _handler->log("no convolution filter selected");
            }

            blobs = blob_finder.find(0, numor->getNFrames());

            if ( _handler ) {
                _handler->log("Found " + std::to_string(blobs.size()) + " blobs");
            }

            //blobs=SX::Geometry::findBlobs3D(numor->begin(), numor->end(), median*threshold, 30, 10000, confidence);
        }
        catch(std::exception& e) // Warning if error
        {
            if ( _handler )
                _handler->log(std::string("Peak finder caused an exception: ") + e.what());

            // pass exception back to callee
            throw e;
        }

        //qDebug() << ">>>> found blobs";

        int ncells=numor->getDiffractometer()->getSample()->getNCrystals();
        std::shared_ptr<SX::Crystal::UnitCell> cell;
        if (ncells)
            cell=numor->getDiffractometer()->getSample()->getUnitCell(0);

        //qDebug() << ">>>> iterating over blobs";

        if (_handler ) {
            _handler->setStatus("Computing bounding boxes...");
            _handler->setProgress(0);
        }

        int count = 0;
        SX::Geometry::AABB<double,3> dAABB(Eigen::Vector3d(0,0,0),Eigen::Vector3d(numor->getDiffractometer()->getDetector()->getNCols(),numor->getDiffractometer()->getDetector()->getNRows(),numor->getNFrames()-1));
        for (auto& blob : blobs)
        {
            Eigen::Vector3d center, eigenvalues;
            Eigen::Matrix3d eigenvectors;
            blob.second.toEllipsoid(_confidence, center, eigenvalues, eigenvectors);
            SX::Crystal::Peak3D* p = new Peak3D(numor);
            p->setPeakShape(new SX::Geometry::Ellipsoid3D(center,eigenvalues,eigenvectors));
            eigenvalues[0]*=2.0;
            eigenvalues[1]*=2.0;
            eigenvalues[2]*=3.0;
            p->setBackgroundShape(new SX::Geometry::Ellipsoid3D(center,eigenvalues,eigenvectors));
            //
            int f=std::floor(center[2]);
            p->setSampleState(new SX::Instrument::ComponentState(numor->getSampleInterpolatedState(f)));
            ComponentState detState=numor->getDetectorInterpolatedState(f);
            p->setDetectorEvent(new SX::Instrument::DetectorEvent(numor->getDiffractometer()->getDetector()->createDetectorEvent(center[0],center[1],detState.getValues())));
            p->setSource(numor->getDiffractometer()->getSource());

            if (!dAABB.contains(*(p->getPeak())))
                p->setSelected(false);
            if (cell)
                p->setUnitCell(cell);
            numor->addPeak(p);
            npeaks++;
            ++count;

            if ( _handler ) {
                double progress = count * 100.0 / blobs.size();
                _handler->setProgress(progress);
            }
        }



        //qDebug() << ">>>> integrating " << numor->getPeaks().size() << " peaks...";

        int peak_counter = 0;

        //qDebug() << ">>>>>>>> initializing peak intensities...";

        if (_handler) {
            _handler->setStatus(("Integrating " + std::to_string(numor->getPeaks().size()) + " peaks...").c_str());
            _handler->setProgress(0);
        }

        for ( auto& peak: numor->getPeaks() )
            peak->framewiseIntegrateBegin();

        //qDebug() << ">>>>>>>> iterating over data frames...";

        //progressDialog->setValue(0);
        //progressDialog->setLabelText("Integrating peak intensities...");

        int idx = 0;

        for ( auto it = numor->getIterator(0); it->index() != numor->getNFrames(); it->advance(), ++idx) {
            Eigen::MatrixXi frame = it->getFrame().cast<int>();
            for ( auto& peak: numor->getPeaks() ) {
                peak->framewiseIntegrateStep(frame, idx);
            }
            //double progress = it->index() / static_cast<double>(numor->getNFrames()) * 100.0;
            //progressDialog->setValue(static_cast<int>(progress));
            //processEvents();

            if (_handler) {
                double progress = it->index() * 100.0 / numor->getNFrames();
                _handler->setProgress(progress);
            }
        }

        //qDebug() << ">>>>>>>> finalizing peak calculation....";

        for ( auto& peak: numor->getPeaks() )
            peak->framewiseIntegrateEnd();


        //progressDialog->close();

        numor->releaseMemory();
        numor->close();
        //_ui->progressBar->setValue(++comp);
    }

    //qDebug() << "Found " << npeaks << " peaks";
    // Reinitialise progress bar
    //_ui->progressBar->setValue(0);
    //_ui->progressBar->setEnabled(false);

    //_ui->_dview->getScene()->updatePeaks();

    if (_handler) {
        _handler->setStatus("Peak finding completed.");
        _handler->setProgress(100);
    }

    return true;
}

void PeakFinder::setHandler(std::shared_ptr<ProgressHandler> handler)
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

double PeakFinder::getConfidence()
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

void PeakFinder::setConvolver(std::shared_ptr<SX::Imaging::Convolver> convolver)
{
    _convolver = convolver;
}

int PeakFinder::getKernelType()
{
    if ( _kernel )
        return _kernel->getType();
    else
        return 0;
}

void PeakFinder::setKernel(std::shared_ptr<Imaging::ConvolutionKernel> kernel)
{
    _kernel = kernel;
}

std::shared_ptr<Imaging::ConvolutionKernel> PeakFinder::getKernel()
{
    return _kernel;
}

} // namespace Data

} // namespace SX


