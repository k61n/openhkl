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

FFTIndexing::FFTIndexing(int nSubdiv,double amax) : _nSubdiv(nSubdiv), _amax(amax)
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

tVector FFTIndexing::findtVector(const Eigen::Vector3d& N,std::vector<double>& hist, std::vector<double>& x) const
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
    int nPoints=std::ceil(2*qMax*_nSubdiv*_amax);
    if (nPoints%2)
    	nPoints++;

    double dq=2*qMax/nPoints;
    double dqInv=1.0/dq;

    x.resize(nPoints);
    for (int i=0;i<nPoints;++i)
    {
    	x[i]=-qMax+i*dq;
    }

    // Reciprocal space histogram
	hist=std::vector<double>(nPoints,0);
	// FFT engine
    Eigen::FFT<double> fft;

	for (const auto& vect: _qVectors)
	{
		double proj=vect.get().dot(N);
		int index =static_cast<int>(std::floor((proj+qMax)*dqInv));
		if (index<0)
			index = 0;
		if (index==nPoints)
			index--;
		hist[index]+= 1.0;
	}

	std::vector<std::complex<double>> spectrum;
	// Fourier transform the histogram
	fft.fwd(spectrum,hist);

	//
	double FZero=std::abs(spectrum[0]);
    int nPointsHalf=nPoints/2;
	int pos_max=0;
	double value=0;
	for (int i=_nSubdiv/2.0;i<nPointsHalf;++i)
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
	hist.resize(nPointsHalf);
	for (int i=0;i<nPointsHalf;++i)
		hist[i]=std::abs(spectrum[i]);

	return tVector(N*(pos_max)*static_cast<double>(_nSubdiv*_amax)/nPoints,value);

}

std::vector<tVector> FFTIndexing::findOnSphere(int nstacks, unsigned int nsolutions) const
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
	int nPoints=std::ceil(2*qMax*_nSubdiv*_amax);
    if (nPoints%2)
    	nPoints++;
	int nPointsHalf=nPoints/2;
	double dq=2*qMax/nPoints;
	double dqInv=1.0/dq;
	double twopi = 2.0*M_PI;
	double fact1 = 0.5*M_PI/nstacks;
	double fact2 = twopi*nstacks;

	// Reciprocal space histogram
	std::vector<double> hist(nPoints,0);
	// FFT engine
	Eigen::FFT<double> fft;
	std::vector<tVector> result;
	result.reserve(nstacks*nstacks);

	for (int th=0;th<=nstacks;++th)
	{
		double theta=static_cast<double>(th)*fact1;

		double ctheta=cos(theta);
		double stheta=sin(theta);
		int nslices=fact2*stheta + 1;

		for (int ph=0;ph<nslices;++ph)
		{
			double phi=static_cast<double>(ph)*twopi/nslices;
			double sp=sin(phi);
			double cp=cos(phi);
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
			int pos_max=0;
			double value=0;
			for (int i=_nSubdiv/2;i<nPointsHalf;++i)
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
			if (pos_max>2)
				result.push_back(tVector(N*(pos_max)*static_cast<double>(_nSubdiv*_amax)/nPoints,value));
		}
	}


	std::sort(result.begin(),
			result.end(),
			[](const tVector& t1, const tVector& t2)->bool
			{
				return (t1._quality > t2._quality);
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
