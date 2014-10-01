#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

#include <unsupported/Eigen/FFT>

#include "Units.h"
#include "FFTIndexing.h"
#include <iostream>
namespace SX
{

namespace Crystal
{

int FFTIndexing::nSubdiv=10;

FFTIndexing::FFTIndexing(double amax) : _amax(amax)
{
}

void FFTIndexing::addVector(const Eigen::Vector3d& v)
{
	_qVectors.push_back(std::cref(v));
}

void FFTIndexing::addVectors(const std::vector<Eigen::Vector3d>& vec)
{
	for (const auto& v : vec)
	{
		_qVectors.push_back(std::cref(v));
	}
}

tVector FFTIndexing::findtVector(const Eigen::Vector3d& N) const
{
	// Vector holding the projections
    std::vector<double> projs(_qVectors.size());
    double qMax=0;
    for (const auto& v : _qVectors)
    {
        double norm=v.get().squaredNorm();
        if (norm>qMax)
               qMax=norm;
    }
    // qMax of all vectors
    qMax=sqrt(qMax);

    // Number of points in the histogram.
    int nPoints=std::ceil(2*qMax*nSubdiv*_amax);
    int nPointsHalf=nPoints/2;
    double dq=2*qMax/nPoints;
    double dqInv=1.0/dq;

    // Reciprocal space histogram
	std::vector<double> hist(nPoints,0);
	// FFT engine
    Eigen::FFT<double> fft;


	for (const auto& vect: _qVectors)
	{
		double proj=vect.get().dot(N);
		int index =static_cast<int>(std::floor((proj+qMax)*dqInv));
		if (index==nPoints)
			index--;
		hist[index]+= 1.0;
	}

	std::vector<std::complex<double>> spectrum;
	// Fourier transform the histogram
	fft.fwd(spectrum,hist);

	//
	double FZero=std::abs(spectrum[0]);
	double integral;
	int pos_max;
	double value=0;
	for (int i=nSubdiv;i<nPointsHalf;++i)
	{
		double current=std::abs(spectrum[i]);
		if (current<0.7*FZero)
			continue;
		if (current>value)
		{
			value=current;
			pos_max=i;
		}
		else
			break;
	}

	return tVector(N*(pos_max)*static_cast<double>(nSubdiv*_amax)/nPoints,value);

}

std::vector<tVector> FFTIndexing::findOnSphere(int nstacks,int nsolutions) const
{

		std::vector<double> projs(_qVectors.size());
	    double qMax=0;
	    for (const auto& v : _qVectors)
	    {
	        double norm=v.get().squaredNorm();
	        if (norm>qMax)
	               qMax=norm;
	    }
	    // qMax of all vectors
	    qMax=sqrt(qMax);

	    // Number of points in the histogram.
	    int nPoints=std::ceil(2*qMax*nSubdiv*_amax);
	    int nPointsHalf=nPoints/2;
	    double dq=2*qMax/nPoints;
	    double dqInv=1.0/dq;

	    // Reciprocal space histogram
		std::vector<double> hist(nPoints,0);
		// FFT engine
	    Eigen::FFT<double> fft;
		std::vector<tVector> result;
		result.reserve(nstacks*nstacks);

	for (int th=0;th<=nstacks;++th)
	{
		double theta=static_cast<double>(th)/nstacks*M_PI*0.5;

		double ctheta=cos(theta);
		double stheta=sin(theta);
		int nslices=2*M_PI*stheta*nstacks;

		for (int ph=0;ph<nslices;++ph)
		{
			double phi=static_cast<double>(ph)/nslices;

			double sp=sin(phi*2*M_PI);
			double cp=cos(phi*2*M_PI);
			const Eigen::Vector3d N(stheta*cp,stheta*sp,ctheta);
			std::fill(hist.begin(),hist.end(),0);
			for (const auto& vect: _qVectors)
			{
				double proj=vect.get().dot(N);
				int index =static_cast<int>(std::floor((proj+qMax)*dqInv));
				if (index==nPoints)
					index--;
				hist[index]+= 1.0;
			}

			std::vector<std::complex<double>> spectrum;
			// Fourier transform the histogram
			fft.fwd(spectrum,hist);

			//
			double FZero=std::abs(spectrum[0]);
			int max_search=nPoints/(nSubdiv);
			int pos_max=0;
			double value=0;
			for (int i=nSubdiv;i<nPointsHalf;++i)
			{
				double current=std::abs(spectrum[i]);
				if (current<0.9*FZero)
					continue;
				if (current>value)
				{
					value=current;
					pos_max=i;
				}
				else
					break;
			}
			if (pos_max>nSubdiv)
				result.push_back(tVector(N*(pos_max)*static_cast<double>(nSubdiv*_amax)/nPoints,value));
		}
	}


	std::sort(result.begin(),
			result.end(),
			[](const tVector& t1, const tVector& t2)->bool
			{
				return (t1._quality >t2._quality);
			});
	if (nsolutions>result.size())
		return result;
	else
	{
		result.erase(result.begin()+nsolutions,result.end());
		return result;
	}
}

FFTIndexing::~FFTIndexing() {
}

} // end namespace Crystal

} // end namespace SX
