/*
 * mosaic.cpp
 *
 *  Created on: Feb 10, 2016
 *      Author: pellegrini
 */

#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include "Mosaic.h"

#include "../geometry/BlobFinder.h"
#include "../geometry/Blob3D.h"
//#include "ComponentState.h"
//#include "ConvexHull.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/DiffractometerStore.h"
#include "../instrument/Gonio.h"
#include "../data/IData.h"
//#include "ILLAsciDataSet.h"
//#include "Peak3D.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
//#include "Triangle.h"
#include "../utils/Units.h"
//#include "Ellipsoid.h"

namespace nsx {
namespace Crystal {

using nsx::Data::DataSet;

static double xor128(void) {
  static uint32_t x = 123456789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  static uint32_t w = 88675123;
  uint32_t t;
  t = x ^ (x << 11);
  x = y; y = z; z = w;
  w = w ^ (w >> 19) ^ (t ^ (t >> 8));
  return w/4294967295.0;
}

bool intersect(double p0, double p1, double p2, double u0, double u1, double u2, double d, double radius, double& t1, double& t2)
{

    double a = (u0*u0) + (u1*u1) + (u2*u2);
    double b = 2.0*(u0*p0 + u1*p1 + u2*p2 -u1*d);
    double c = (p0*p0) + (p1*p1) + (p2*p2) + d*d - radius*radius;

    double delta = b*b - 4.0*a*c;

    if (delta < 0)
        return false;

    double sdelta = sqrt(delta);

    t1 = 0.5*(-b - sdelta)/a;
    t2 = 0.5*(-b + sdelta)/a;

    return (t1>0 && t2>0);
}

double ellipsoids_overlap(const nsx::Geometry::Ellipsoid<double,3>& ell1,const nsx::Geometry::Ellipsoid<double,3>& ell2)
{
    const Eigen::Vector3d& lb1 = ell1.getLower();
    const Eigen::Vector3d& ub1 = ell1.getUpper();

    std::default_random_engine gen;
    std::uniform_real_distribution<double> d1(lb1[0],ub1[0]);
    std::uniform_real_distribution<double> d2(lb1[1],ub1[1]);
    std::uniform_real_distribution<double> d3(lb1[2],ub1[2]);

    int inside1{0};
    int inside2{0};
    for (int i=0;i<100000;++i)
        {
        Eigen::Vector3d point(d1(gen),d2(gen),d3(gen));
        Eigen::Vector4d hpoint = point.homogeneous();
        if (ell1.isInside(hpoint))
                {
            ++inside1;
            if (ell2.isInside(hpoint))
                ++inside2;
        }
        }

    double overlap = 4.0*M_PI*ell1.getExtents().prod()*static_cast<double>(inside2)/static_cast<double>(inside1)/3.0;

    return overlap;
}


Mosaic::Mosaic(const std::string& instr, double l, double dl, double dMonSam, double mu)
: _sample(nullptr),
  _l(l),
  _dl(dl),
  _dMonSam(dMonSam),
  _mu(mu*nsx::Units::deg)
{
    using namespace nsx::Instrument;

    // Set up the diffractometer
    DiffractometerStore* ds=DiffractometerStore::Instance();
    _diffractometer = std::shared_ptr<Diffractometer>(ds->buildDiffractomer(instr));
}

void Mosaic::setSample(nsx::Instrument::Sample* sample)
{
    _sample = sample;
}

void Mosaic::setMosaicity(double mosaicity)
{
    _mu = mosaicity*nsx::Units::deg;
}

bool Mosaic::run(std::vector<std::shared_ptr<DataSet>> datas, unsigned int n, double& overlap)
{
    using namespace nsx::Data;
    using namespace nsx::Crystal;
    using namespace nsx::Geometry;
    using namespace nsx::Instrument;

    double cmu = cos(_mu);
    double oneMinuscmu = 1.0-cmu;

    double sigmal =_dl/2.0/sqrt(2.0*log(2.0));

    // Build a look up table for a gaussian distribution around l with fwhm=dl
    Eigen::VectorXd lambdas(1000);
    int nlambdas = lambdas.size();
    for (int i=0;i<nlambdas;++i)
    {
        double x=-3*sigmal+(static_cast<double>(i)/static_cast<double>(nlambdas))*6.0*sigmal;
        lambdas[i]= 1.0/sqrt(2.0*M_PI)/sigmal*exp(-0.5*x*x/sigmal/sigmal);
    }

    double lmin = _l-sigmal;
    double lmax = _l+sigmal;

    // jonathan: modified to compile
//    double monHeight = _diffractometer->getSource()->getHeight();
//    double monWidth = _diffractometer->getSource()->getWidth();
    double monHeight = _diffractometer->getDetector()->getHeight();
    double monWidth = _diffractometer->getDetector()->getWidth();

    // Read the numors
//    std::vector<DataSet*> datas;
//    for (const auto& num: numors)
//        datas.push_back(ILLAsciDataSet::create(num,_diffractometer));

    // Create and get the unit cell of the sample
    std::shared_ptr<UnitCell> uc = _sample->addUnitCell();

    // Loop over the datas
    for (auto d : datas)
    {
        // Fetch h,k,l from the current data
        Eigen::Vector3d hkl;
        hkl[0] = d->getMetadata()->getKey<double>("qH");
        hkl[1] = d->getMetadata()->getKey<double>("qK");
        hkl[2] = d->getMetadata()->getKey<double>("qL");

        // Fetch the UB matrix from the current data
        Eigen::Matrix3d ub;
        ub(0,0) = d->getMetadata()->getKey<double>("ub(1,1)");
        ub(0,1) = d->getMetadata()->getKey<double>("ub(1,2)");
        ub(0,2) = d->getMetadata()->getKey<double>("ub(1,3)");
        ub(1,0) = d->getMetadata()->getKey<double>("ub(2,1)");
        ub(1,1) = d->getMetadata()->getKey<double>("ub(2,2)");
        ub(1,2) = d->getMetadata()->getKey<double>("ub(2,3)");
        ub(2,0) = d->getMetadata()->getKey<double>("ub(3,1)");
        ub(2,1) = d->getMetadata()->getKey<double>("ub(3,2)");
        ub(2,2) = d->getMetadata()->getKey<double>("ub(3,3)");

        Eigen::Vector3d zvect=ub*hkl;
        double znorm = zvect.norm();
        double znorm2 = znorm*znorm;

        uc->setBU(ub.transpose());

        // Get the U matrix (actually tU in NSXTool convention)
        Eigen::Matrix3d umat = uc->getBusingLevyU();

        // The convex hull of the sample is rotated by u
        ConvexHull<double>& hull = _sample->getShape();

        Blob3D blob;

        std::vector<int> countsPerFrame(d->getNFrames(),0);
        for (unsigned int z=0; z<d->getNFrames(); ++z)
        {
            ComponentState sampleState=d->getSampleState(z);

            ComponentState detectorState=d->getDetectorState(z);

            //Eigen::Matrix3d omchiphi = sampleState.getParent()->getGonio()->getHomMatrix(sampleState.getValues()).rotation();
            Eigen::Matrix3d omchiphi = d->getDiffractometer()->getSample()->getGonio()->getHomMatrix(sampleState.getValues()).rotation();

            Eigen::Vector3d qvect = omchiphi*zvect;

            std::cout<<qvect.transpose()<<std::endl;

            // Construct an orthonormal basis u,v,w with w // to q vect and u within xy plane
            Eigen::Vector3d w=qvect.normalized();
            Eigen::Vector3d u(-w[1],w[0],0.0);
            u.normalize();
            Eigen::Vector3d v=w.cross(u);

            std::vector<Triangle> faces=hull.createFaceCache(omchiphi*umat);
            double ymin=std::numeric_limits<double>::infinity();
            double xmin=std::numeric_limits<double>::infinity();
            double xmax=-std::numeric_limits<double>::infinity();
            double zmin=std::numeric_limits<double>::infinity();
            double zmax=-std::numeric_limits<double>::infinity();
            for (auto& f : faces)
            {
                if (f._A[1] < ymin)
                    ymin = f._A[1];
                if (f._B[1] < ymin)
                    ymin = f._B[1];
                if (f._C[1] < ymin)
                    ymin = f._C[1];
                if (f._A[0]>xmax)
                    xmax=f._A[0];
                if (f._B[0]>xmax)
                    xmax=f._B[0];
                if (f._C[0]>xmax)
                    xmax=f._C[0];
                if (f._A[0]<xmin)
                    xmin=f._A[0];
                if (f._B[0]<xmin)
                    xmin=f._B[0];
                if (f._C[0]<xmin)
                    xmin=f._C[0];
                if (f._A[2]>zmax)
                    zmax=f._A[2];
                if (f._B[2]>zmax)
                    zmax=f._B[2];
                if (f._C[2]>zmax)
                    zmax=f._C[2];
                if (f._A[2]<zmin)
                    zmin=f._A[2];
                if (f._B[2]<zmin)
                    zmin=f._B[2];
                if (f._C[2]<zmin)
                    zmin=f._C[2];
            }

            for (unsigned int i=0;i<n;++i)
            {
//				std::cout<<monWidth<<" "<<monHeight<<std::endl;
//				monWidth = 0.01;
//				monHeight = 0.01;
                double xrand = (-0.5+xor128())*monWidth;
                double zrand = (-0.5+xor128())*monHeight;

                Eigen::Vector3d from(xrand,-_dMonSam,zrand);

                Eigen::Vector3d to(xmin+xor128()*(xmax-xmin),ymin,zmin+xor128()*(zmax-zmin));

                unsigned int nIntersections{0};

                double times[2];

                for (const auto& triangle : faces)
                {
//					if (triangle.isOutsideBB(to[0],to[2]))
//						continue;

                    if (triangle.rayIntersect(from,to-from,times[nIntersections]))
                    {
                        if (++nIntersections==2)
                            break;
                    }
                }

                if (nIntersections!=2)
                    continue;

                if (times[0]>times[1])
                    std::swap(times[0],times[1]);

                double lpm=xor128()*(times[1]-times[0]);
                Eigen::Vector3d ki=(times[0] + lpm)*(to-from);
                to = from + ki;

                double ca = cmu+xor128()*oneMinuscmu;
                double sa = sqrt(1.0-ca*ca);

                double cb,sb,fact;
                do
                {
                    cb = 2.0*xor128()-1.0;
                    sb = 2.0*xor128()-1.0;
                    fact = cb*cb+sb*sb;

                } while (fact >= 1.0);

                fact = sqrt(fact);
                cb /= fact;
                sb /= fact;

                Eigen::Vector3d qvect1=sa*cb*u + sa*sb*v + ca*w;
                qvect1 *= znorm;

                double sangle=-ki.dot(qvect1)/ki.norm()/znorm2;
                double ll=2.0*sangle;
                if (ll<lmin || ll >lmax)
                    continue;

                Eigen::Vector3d kf;
                kf = ki/(ki.norm()*ll) + qvect1;

                double px,py,time;

                bool test=_diffractometer->getDetector()->receiveKf(px,py,kf,to,time,detectorState.getValues());
                if (test)
                {
                    int lbin=static_cast<int>((ll-_l+3.0*sigmal)*nlambdas/sigmal/6.0);
                    if (lbin < 0 || lbin >= nlambdas)
                        continue;
                    blob.addPoint(px,py,z,lambdas[lbin]);
                    countsPerFrame[z]++;
                }

            }
        }

        std::cout<<blob<<std::endl;

        if (blob.getComponents()==0)
            return false;

        Eigen::Vector3d center;
        Eigen::Vector3d eigenvalues;
        Eigen::Matrix3d eigenvectors;
        blob.toEllipsoid(0.997,center,eigenvalues,eigenvectors);
        Ellipsoid<double,3> ellmc(center,eigenvalues,eigenvectors);

        // d->readInMemory();
        std::vector<int*> temp(d->getNFrames());
        for (unsigned int i=0;i < d->getNFrames();++i)
        {
            const Eigen::MatrixXi& counts=d->getFrame(i);
            temp[i] = const_cast<int*>(counts.data());
        }
        int median = d->getBackgroundLevel(nullptr) + 1;
        blob3DCollection blobs;
        BlobFinder finder(d);

        //blobs=findBlobs3D<int>(temp,d->getDiffractometer()->getDetector()->getNRows(),d->getDiffractometer()->getDetector()->getNCols(),3.0*median,30,10000,0.997,0);
        finder.setThreshold(3.0);
        finder.setConfidence(0.997);
        finder.setMedian(median);
        finder.setMinComp(30);
        finder.setMaxComp(10000);
        finder.setRelative(true);
        blobs = finder.find(0, d->getNFrames());

        for (auto& p : blobs)
        {
            Eigen::Vector3d center1;
            Eigen::Vector3d eigenvalues1;
            Eigen::Matrix3d eigenvectors1;
            p.second.toEllipsoid(0.997,center1,eigenvalues1,eigenvectors1);
            Ellipsoid<double,3> ellexp(center1,eigenvalues1,eigenvectors1);
            ellexp.translate(center-center1);
//    		std::cout<<eigenvectors<<std::endl;
//    		std::cout<<eigenvectors1<<std::endl;
            std::cout<<ellexp.getAABBExtents()<<std::endl;
            std::cout<<ellmc.getAABBExtents()<<std::endl;
            std::cout<<"exp = "<<4.0*M_PI*ellexp.getExtents().prod()/3.0<<std::endl;
            std::cout<<"mc = "<<4.0*M_PI*ellmc.getExtents().prod()/3.0<<std::endl;
            overlap = ellipsoids_overlap(ellexp,ellmc);
        }

        for (auto v : countsPerFrame)
            std::cout<<v<<std::endl;
    }

    return true;
}

Mosaic::~Mosaic()
{
}

} // namespace Crystal
} // end namespace nsx
