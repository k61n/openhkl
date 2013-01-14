#include "HeCell.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Bpb.h"

namespace SX
{

HeCell::HeCell(const char* name):_name(name), _pvst(new Scan1D("Time (s)", "Polar.")),_peaks(0)
{

}

HeCell::~HeCell()
{
}

void HeCell::setDeadTime(const boost::shared_ptr<DTCorrection>& dt)
{
	_deadt=dt;
}
void HeCell::setNumors(const boost::shared_ptr<NumorSet>& numors)
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
			double h=nv[i]->getMetaData().getKey<double>("H(Hmin)");
			double k=nv[i]->getMetaData().getKey<double>("K(Kmin)");
			double l=nv[i]->getMetaData().getKey<double>("L(Lmin)");
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
const boost::shared_ptr<Scan1D>& HeCell::getDecay() const
{
	return _pvst;
}
void HeCell::addPeak(double h, double k, double l)
{
	_peaks.push_back(boost::tuple<double,double,double>(h,k,l));
}
int HeCell::fitExponential(double& A0,double& A0s, double& alpha,double& alphas)
{

	const gsl_multifit_fdfsolver_type *T;
   gsl_multifit_fdfsolver *s;
   int status;
   unsigned  iter = 0;
   const size_t n = _pvst->npoints();
   const size_t p = 2;

   gsl_matrix *covar = gsl_matrix_alloc (p, p);

   // This is OK since std::vector is garanteed contiguous
   struct data d = { n, &_pvst->getX()[0],&_pvst->getY()[0],&_pvst->getE()[0]};
   gsl_multifit_function_fdf f;
   double param_init[2] = { 1.0, 0.0 };
   gsl_vector_view param = gsl_vector_view_array (param_init, p);

   f.f = &exp_f;
   f.df = &exp_df;
   f.fdf = &exp_fdf;
   f.n = n;
   f.p = p;
   f.params = &d;

   T = gsl_multifit_fdfsolver_lmsder;
   s = gsl_multifit_fdfsolver_alloc (T, n, p);
   gsl_multifit_fdfsolver_set (s, &f, &param.vector);

   do
	 {
	   iter++;
	   status = gsl_multifit_fdfsolver_iterate (s);

	  if (status)
		 break;

	   status = gsl_multifit_test_delta (s->dx, s->x,
										 1e-4, 1e-4);
	 }
   while (status == GSL_CONTINUE && iter < 500);

   gsl_multifit_covar (s->J, 0.0, covar);
    _P0= gsl_vector_get(s->x,0);
    _P0s=sqrt(gsl_matrix_get(covar,0,0)) ;
    _alpha0= gsl_vector_get(s->x,1);
     _alpha0s=sqrt(gsl_matrix_get(covar,1,1)) ;
     A0=_P0;
     A0s=_P0s;
     alpha=_alpha0;
     alphas=_alpha0s;
	return 0;
}

double HeCell::getPolar(const Numor& numor,double& Pcell, double& Pcells)
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
double HeCell::normalizebpb(const Numor& numor,double& polar, double& polars)
{
	double polarin=polar;
	double pcell, pcells;
	double time=getPolar(numor,pcell,pcells);
	polar/=pcell;
	polars=std::fabs(polar)*sqrt(polars*polars/polarin/polarin+pcells*pcells/pcell/pcell);
	return time;
}

} /* namespace SX */
