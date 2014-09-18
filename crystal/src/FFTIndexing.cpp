#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

#include <unsupported/Eigen/FFT>

#include "Units.h"
#include "FFTIndexing.h"

namespace SX
{

namespace Crystal
{

int FFTIndexing::nSubdiv=5;

FFTIndexing::FFTIndexing(double amax,double thetaStep,double phiStep) : _amax(amax),_thetaStep(thetaStep),_phiStep(phiStep)
{
}

std::vector<tVector> FFTIndexing::find(std::vector<Eigen::Vector3d> qVectors)
{

    std::vector<double> projs(qVectors.size());
    double qMax=0;
    for (uint i=0;i<qVectors.size();++i)
    {
        double norm=qVectors[i].squaredNorm();
        if (norm>qMax)
               qMax=norm;
    }
    qMax=sqrt(qMax);

    int nPoints=std::ceil(2*qMax*nSubdiv*_amax);
    int nPointsHalf=nPoints/2;
    double dq=2*qMax/nPoints;
    double dqInv=1.0/dq;

	std::vector<double> hist(nPoints,0);
    Eigen::FFT<double> fft;

    int nThetas=std::ceil(180.0*deg/_thetaStep);
    int nPhis=std::ceil(360.0*deg/_thetaStep);

    std::vector<tVector> solutions;
	solutions.reserve(nThetas*nPhis);

	for (int th=0;th<nThetas;++th)
	{
		double ctheta=(1.0-static_cast<double>(2.0*th/nThetas));
		double stheta=sin(acos(ctheta));
		for (int ph=0;ph<nPhis;++ph)
		{
			double pMin=qMax;
			double pMax=-qMax;

			std::fill(hist.begin(),hist.end(),0.0);
			if (th==0)
				ph=nPhis;
			double phi=M_PI+static_cast<double>(ph)/100*M_PI;
			double sp=sin(phi);
			double cp=cos(phi);
			const Eigen::Vector3d N(stheta*cp,stheta*sp,ctheta);

            for (const auto& vect: qVectors)
            {
                double proj=vect.dot(N);
                int index =static_cast<int>((proj+qMax)*dqInv);

                if (index==nPoints)
                    index--;

                if (proj<pMin)
                {
                        pMin=proj;
                }
                if (proj>pMax)
                {
                        pMax=proj;
                }
                hist[index]+= 1.0;
            }

            std::vector<std::complex<double>> spectrum;

            fft.fwd(spectrum,hist);
            std::vector<double> y;

            y.reserve(nPointsHalf);

            for (int i=0;i<nPointsHalf;++i)
                y.push_back(std::abs(spectrum[i]));

            auto it=std::max_element(y.begin()+1,y.end());
            int distance=std::distance(y.begin(),it);

            solutions.push_back(tVector(N*distance*static_cast<double>(nSubdiv*_amax)/nPointsHalf,*it));

        }
    }

	return solutions;

}

FFTIndexing::~FFTIndexing() {
}

} // end namespace Crystal

} // end namespace SX
