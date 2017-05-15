#include "HeCell.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Bpb.h"
#include <cminpack-1/cminpack.h>
#include <Eigen/Dense>

namespace nsx
{

HeCell::HeCell(const char* name): _pvst(new nsx::Data::Scan1D("Time (s)", "Polar.")),_peaks(0)
{
	if (name)
		_name=name;
}

HeCell::~HeCell()
{
}

void HeCell::setDeadTime(const boost::shared_ptr<DTCorrection>& dt)
{
	_deadt=dt;
}
void HeCell::setNumors(const boost::shared_ptr<nsx::Data::NumorSet>& numors)
{
	_numors=numors;
}
void HeCell::resetNumors()
{
	_numors.reset();
}
void HeCell::calculateDecay()
{

	std::vector<sptrNumor> nv=_numors->getNumors();
	std::vector<double> time;
	std::vector<double> polar;
	std::vector<double> error;
	bool start=true;

	for (std::size_t i=0;i<nv.size();++i)
	{
		if (nv[i]->isbpb()) // Check that measurement are indeed bpbs
		{
			double h=nv[i]->getMetaData().getKey<double>("qH");
			double k=nv[i]->getMetaData().getKey<double>("qK");
			double l=nv[i]->getMetaData().getKey<double>("qL");
			bool found_peak=false;
			std::size_t peaksize=_peaks.size();
			for (std::size_t ii=0;ii<peaksize;ii++)
			{
				double ch=boost::get<0>(_peaks[ii]);
				double ck=boost::get<1>(_peaks[ii]);
				double cl=boost::get<2>(_peaks[ii]);
				if (h==ch && k==ck && l==cl)
				{
					found_peak=true;
					break;
				}
			}
			if (!found_peak)
				continue;
			if (start) // First data point, get absolute time of this
			{
				_starttime=nv[i]->getMetaData().getKey<boost::posix_time::ptime>("ptime");
				start=false;
			}
			boost::posix_time::time_duration deltat=nv[i]->getMetaData().getKey<boost::posix_time::ptime>("ptime")-_starttime;
			double fr,frs;
			calculateFlipping(*(nv[i]),fr,frs,*_deadt,true);
			if (fr>0.1)
			{
			time.push_back(static_cast<double	>(deltat.total_seconds()));
			polar.push_back(fr);
			error.push_back(frs);
			}
		}
	}
	_pvst->setData(time,polar,error);

}
const boost::shared_ptr<nsx::Data::Scan1D>& HeCell::getDecay() const
{
	return _pvst;
}
void HeCell::addPeak(double h, double k, double l)
{
	std::cout << "Registering peak " << h << "," << k << "," << l << " for cell decay\n";
	_peaks.push_back(boost::tuple<double,double,double>(h,k,l));
}
int HeCell::fitExponential(double& A0,double& A0s, double& alpha,double& alphas)
{

	const size_t n = _pvst->npoints();
	// This is OK since std::vector is garanteed contiguous
	struct data d = {(int)n, &_pvst->getX()[0],&_pvst->getY()[0],&_pvst->getE()[0]};
	std::cout << "----Cell decay---\n";
	std::cout << "Time(s) Polar Sigma\n";
	for (int i=0;i<d.n;++i)
	{
		std::cout << d.x[i] << " " << d.y[i] << " " << d.sigma[i] << std::endl;
	}
	// Number of parameters
	const int m=2;
	int maxfev, mode, nprint, info, nfev, ldfjac;
	int ipvt[m];
	double ftol, xtol, gtol, epsfcn, factor;
	double x[m], fvec[n], diag[m], fjac[n*m], qtf[m],
	wa1[m], wa2[m], wa3[m], wa4[n];

	// Initial guesses
	x[0] = 0.8;
	x[1] = 1e-6;

	ldfjac = n;
	ftol = sqrt(dpmpar(1));
	xtol = sqrt(dpmpar(1));
	gtol = 0.;

	maxfev = 800;
	epsfcn = 0.;
	mode = 1;
	factor = 1.e2;
	nprint = 1;
	info = lmdif(ExpDecay,&d, n, m, x, fvec, ftol, xtol, gtol, maxfev, epsfcn,
	diag, mode, factor, nprint, &nfev, fjac, ldfjac,
	ipvt, qtf, wa1, wa2, wa3, wa4);
	A0=x[0];
	alpha=x[1];
	std::cout << "Result of fit" << A0 << " " << alpha <<std::endl;
	_P0=x[0];
	_alpha0=x[1];
	_P0s=0;
	_alpha0s=0;

	std::cout <<"---End cell decay---\n";
	return info;
}

double HeCell::getPolar(const nsx::Data::Numor& numor,double& Pcell, double& Pcells)
{
	boost::posix_time::ptime ntime;
	try
	{
	ntime=numor.getMetaData().getKey<boost::posix_time::ptime>("ptime");
	}catch(...)
	{
		throw std::runtime_error("Could not determine time in numor");
	}
	boost::posix_time::time_duration dt=ntime-_starttime;
	int dts=dt.total_seconds();
	double expterm=exp(-_alpha0*dts);
	Pcell=_P0*expterm;
	double sigma=Pcell*(dts*_alpha0s); // dts is large (s) and alpha0  small
	sigma*=sigma;
	sigma+=std::pow(expterm*_P0s,2);
	Pcells=sqrt(sigma);
	return dts;
}
double HeCell::normalizebpb(const nsx::Data::Numor& numor,double& polar, double& polars)
{
	double polarin=polar;
	double pcell, pcells;
	double time=getPolar(numor,pcell,pcells);
	polar/=pcell;
	polars=std::fabs(polar)*sqrt(polars*polars/polarin/polarin+pcells*pcells/pcell/pcell);
	return time;
}

} // end namespace nsx
