#include <cmath>
#include <stdexcept>
#include "Bpb.h"
#include <algorithm>
#include <iterator>

namespace nsx
{

void calculateFlipping(const std::vector<double>& data, double& fr, double& frs, DTCorrection& correction, bool asymmetry)
{
	if (data.size()!=18)
		throw std::runtime_error("Could not calculate flipping ratio from raw data, wrong number of points");

	// Time spent on background + and - and total (summing both sides of peak)
	double t_b_p=(data[2]+data[14]);
	double t_b_m=(data[3]+data[15]);
	double t_b=(t_b_p+t_b_m);
	// Monitor counts on background + and - and total (summing both sides of peak)
	double m_b_p=(data[4]+data[16]);
	double m_b_m=(data[5]+data[17]);
	double m_b=(m_b_p+m_b_m);
	// Counts on background + and background - (summing both sides of peak).
	// Deadtime correction on background could be neglected..
	double c_b_p=(data[0]+data[12]);
	double c_b_ps=sqrt(c_b_p);
	correction.apply(c_b_p,c_b_ps,t_b_p);
	double c_b_m=(data[1]+data[13]);
	double c_b_ms=sqrt(c_b_m);
	correction.apply(c_b_m,c_b_ms,t_b_m);

	// Time spent on peak + and - and total
	double t_p_p=data[8];
	double t_p_m=data[9];
	double t_p=t_p_p+t_p_m;
	// Monitor counts on peak + and - and total
	double m_p_p=data[10];
	double m_p_m=data[11];
	double m_p=m_p_p+m_p_m;
	// Counts on peak + and peak -
	double c_p_p=data[6];
	double c_p_ps=sqrt(c_p_p);
	correction.apply(c_p_p,c_p_ps,data[8]);
	double c_p_m=data[7];
	double c_p_ms=sqrt(c_p_m);
	correction.apply(c_p_m,c_p_ms,data[9]);

	// Calculate the flipping ratio.
	double nomi1=c_p_p*t_p/(m_p*t_p_p);
	double nomi2=c_b_p*t_b/(m_b*t_b_p);

	double denom1=c_p_m*t_p/(m_p*t_p_m);
	double denom2=c_b_m*t_b/(m_b*t_b_m);

	// In the event of spurious signal on the background measurement
	// and that both + and - channels do not exceed their respective backgrounds
	if (nomi1<nomi2 && denom1 < denom2)
		throw std::range_error("Can't calculate flipping ratio. Normalized background counts higher than peak in both channels");
	double nplus=nomi1-nomi2;
	double nminus=denom1-denom2;
	double R=nplus/nminus;
	// Now the sigmas of the FR, taking into account the Guassian errors in the counts and monitors
	// neglecting time uncertainty.
	double alpha=t_p/t_p_p;
	double beta=t_b/t_b_p;
	double gamma=t_p/t_p_m;
	double delta=t_b/t_b_m;
	double term1=(alpha*alpha*c_p_ps*c_p_ps);
	term1+=gamma*gamma*R*R*c_p_ms*c_p_ms;
	term1+=std::pow((R*gamma*c_p_m-alpha*c_p_p)/m_p,2)*m_p;
	double term2=beta*beta*c_b_ps*c_b_ps;
	term2+=delta*delta*R*R*c_b_ms*c_b_ms;
	term2+=std::pow((R*delta*c_b_m-beta*c_b_p)/m_b,2);
	double dR=1.0/nminus*sqrt(term1/m_p/m_p+term2/m_b/m_b);
	fr=R;
	frs=dR;
	if (asymmetry)
		converttoAsymmetry(fr,frs);


}

void converttoAsymmetry(double& fr, double& frs)
{
	double a=fr-1.0;double b=fr+1.0;
	fr=a/b;
	frs*=2.0/(b*b);
	frs=std::fabs(frs);
}

void calculateFlipping(const nsx::Data::Numor& n, double& fr, double& frs,DTCorrection& correction, bool asymmetry)
{
		std::vector<double> data=getFlippingData(n);
		calculateFlipping(data,fr,frs,correction,asymmetry);
}

std::vector<double> getFlippingData(const nsx::Data::Numor& n)
{
		if (!n.isbpb())
				throw std::runtime_error("Not a bpb measurement");

			// Merge the data of all measurements into a vector that contains the sum
			std::vector<double> data(18);
			const std::vector<double>& raw=n.getData();
			// Number of points in a measurement
			int nbang=n.getMetaData().getKey<int>("nbang");
			int stride=6+nbang;
			unsigned int nruns=raw.size()/stride;
			std::vector<double>::const_iterator rawit=raw.begin();
			std::vector<double>::iterator datait=data.begin();
			for (unsigned int j=0;j<nruns;j++)
			{
				std::transform(datait,datait+6,rawit,datait,std::plus<double>());
				datait+=6;
				rawit+=stride;
				if (datait==data.end())
					datait=data.begin();
			}
			return data;
}

} /* namespace nsx */
