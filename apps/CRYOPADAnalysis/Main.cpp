#include "HeCell.h"
#include "Numor.h"
#include "NumorSet.h"
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "Bpb.h"
#include <algorithm>
#include <iterator>
#include <iomanip>

int main()
{
	using namespace nsx::Data;
	using namespace nsx;

	boost::shared_ptr<NumorSet> ptrnumor(new NumorSet("Set"));
	std::cout << "Give data directory\n";
	std::string dir;
	std::cin >> dir;
	std::cout << "Numor start and end \n";
	int begin,end;
	std::cin >> begin >> end;
    for (int i=begin;i<=end;++i)
	{
		std::ostringstream os;
		os << i;
		ptrnumor->addNumorFromFile(dir+os.str());
	}
	HeCell cell;
	cell.setNumors(ptrnumor);
	std::cout << "Add peak (h,k,l) for cell decay";
	double h,k,l;
	std::cin >> h >> k >> l;
	cell.addPeak(h,k,l);
	double a,b,c,d;
	boost::shared_ptr<DTCorrection> dt(new DTCorrection("dead"));
	cell.setDeadTime(dt);
	cell.calculateDecay();
	cell.fitExponential(a,b,c,d);

	std::cout << "***************** Starts correction ******************** \n";
    for (int i=begin;i<=end;++i)
	{

		std::ostringstream os;
		os << i;
		Numor n(dir+os.str());
		if (n.isbpb())
		{
			double fr, frs;
			calculateFlipping(n,fr,frs,*(dt.get()),true);
			double pcell, spcell;
			cell.getPolar(n,pcell,spcell);
			double value=fr/pcell;
			double saA=frs/fr;
			double sbB=spcell/pcell;
			saA*=saA;
			sbB*=sbB;
			double erreur=std::abs(value)*sqrt(saA+sbB);
			const MetaData& m=n.getMetaData();
			double h=m.getKey<double>("qH");
			double k=m.getKey<double>("qK");
			double l=m.getKey<double>("qL");

			int pxi=m.getKey<double>("px-input");
			int pyi=m.getKey<double>("py-input");
			int pzi=m.getKey<double>("pz-input");

			int pxo=m.getKey<double>("px-output");
			int pyo=m.getKey<double>("py-output");
			int pzo=m.getKey<double>("pz-output");
			bool inyz=false;
			bool outx=false;
			if (pxi!=0)
				os << (pxi == 1 ? " +x" : " -x") ;
			if (pyi!=0)
			{
				os << (pyi == 1 ? " +y" : " -y") ;
				inyz=true;
			}
			if (pzi!=0)
			{
				os << (pzi == 1 ? " +z" : " -z") ;
				inyz=true;
			}
			if (pxo!=0)
			{
				os << (pxo == 1 ? " +x" : " -x") ;
				outx=true;
			}
			if (pyo!=0)
				os << (pyo == 1 ? " +y" : " -y") ;
			if (pzo!=0)
				os << (pzo == 1 ? " +z" : " -z") ;

			double polarin=0.934;
			if (inyz && outx)
			{
				value*=polarin;
				erreur*=polarin;
			}
			std::cout << os.str() << " " << std::fixed << std::setw(8) << std::setprecision(3) << h <<
					std::fixed << std::setw(8) << std::setprecision(4) << k <<
					std::fixed << std::setw(8) << std::setprecision(4) << l <<
					std::fixed << std::setw(8) << std::setprecision(4) << fr <<
					std::fixed << std::setw(8) << std::setprecision(4) << frs <<
					std::fixed << std::setw(8) << std::setprecision(4) << value <<
					std::fixed << std::setw(8) << std::setprecision(4) << erreur << std::endl;
		}
	}
    std::cout <<  "*****************  End  corrections ******************** \n";


}
