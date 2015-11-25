#include <iostream>
#include <cmath>
#include <ios>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <Eigen/Dense>
#include <sstream>


struct Scan
{
	int numor;
	int npoints;
	double omega_center;
	double omega_width;
	double cx,cy,cz;
	double sx,sy,sz;
	double chi,phi,theta2;
	double Intensity;
};

void write_FakeD9(const Scan& scan, const char* filestr)
{
	std::ofstream file(filestr,std::ios::out);
	if (!file.is_open())
			return;

file <<
"RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR" <<std::endl;
file << std::fixed << std::setw(8) << scan.numor;
file << "       0       4                                                        " << std::endl;
file <<
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
file <<
"      80       1                                                                " << std::endl;
file <<
"Inst User L.C.   Date     Time                                                  " <<std::endl;
file <<
"D9  Chapo Chap25-Jun-15 22:16:24                                                " << std::endl;
file <<
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
file <<
"      80       1                                                                " << std::endl;
file <<
"Title                                                                   Scantype" << std::endl;
file <<
"Fake data                                                               omega   " << std::endl;
file <<
"IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII" <<std::endl;
file <<
"      31       4                                                                " << std::endl;
file <<
"   nvers   ntype   kctrl   manip   nbang   nkmes  npdone   jcode   ipara   ianal" << std::endl;
file <<
"   imode    itgv  iregul   ivolt    naxe npstart  ilast1     isa  flgkif      ih"<< std::endl;
file <<
"      ik   nbsqs  nb_det  nbdata icdesc1 icdesc2 icdesc3 icdesc4 icdesc5 icdesc6"<< std::endl;
file <<
" icdesc7                                                                        "<< std::endl;
file <<
"       4       2       0       2       1" << std::fixed << std::setw(8) << scan.npoints << std::fixed << std::setw(8) << scan.npoints << "       0       1       0"<< std::endl;
file <<
"       0       1       1       0       1       0       0       0       0       0"<< std::endl;
file <<
"       0       0       1    1024       2       0       0       0       0       0"<< std::endl;
file <<
"       0                                                                        "<< std::endl;
file <<
"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"<< std::endl;
file <<
"       5       1                                                                " << std::endl;
file <<
"      wavelength           omega  2theta (gamma)             phi             chi" << std::endl;
file <<
"  1.00000000E+00" <<
std::fixed << std::setw(16) << scan.omega_center <<
std::fixed << std::setw(16) << scan.theta2 <<
std::fixed << std::setw(16) << scan.phi  <<
std::fixed << std::setw(16) << scan.chi  << std::endl;

double prefactor=scan.Intensity/(std::pow(2*M_PI,1.5)*scan.sx*scan.sy*scan.sz);

for (int z=0;z<scan.npoints;++z)
{
	file << "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS" << std::endl;
	file << std::fixed << std::setw(8) << z+1 << std::fixed << std::setw(8) << scan.npoints-z-1 << std::fixed << std::setw(8) << scan.npoints << std::fixed << std::setw(8) << scan.numor << "       0       1                                " << std::endl;
	file << "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" <<std::endl;
	file << "       4       1                                                                " <<std::endl;
	file << "            time         monitor       Total Cou     angles*1000                " << std::endl;
	file << "  0.46410000E+04  0.50000000E+05  0.73200000E+03" <<std::fixed << std::setw(16) << 1000*(scan.omega_center-scan.omega_width/2.0+z/static_cast<double>(scan.npoints-1)*scan.omega_width) << "                " << std::endl;
	file << "IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII" << std::endl;
	file << "    1024                                                                        " << std::endl;
	int ii=0;
	for (int x=0;x<32;++x)
	{
		for (int y=0;y<32;++y)
		{
			if (ii>9)
			{
				file <<std::endl;
				ii=0;
			}
			double in=exp(-0.5*((x-scan.cx)*(x-scan.cx)/scan.sx/scan.sx+(y-scan.cy)*(y-scan.cy)/scan.sy/scan.sy+(z-scan.cz)*(z-scan.cz)/scan.sz/scan.sz));
			file << std::fixed << std::setw(8) << int(prefactor*in);
			ii++;
		 }
	}
	file << "                                                " << std::endl;
}

file.close();
}
int main()
{
	double wavelength = 1.0;
	Scan scan;
	scan.npoints=31;
	scan.omega_width=3.0;
	scan.cz=15.5;
	scan.sx=3.0;
	scan.sy=3.0;
	scan.sz=3.0;
	scan.Intensity=10000;
	Eigen::Matrix3d ub;
	ub << 0.1,0.0,0.0,
		  0.0,0.1,0.0,
		  0.0,0.0,0.1;

	// Offset of sample (in mm) at rest
	Eigen::Vector3d sample(0,0,0); //

	int p=0;
	for (int h=-5;h<6;++h)
	{
		for (int k=-5;k<6;++k)
		{
			for (int l=-5;l<6;++l)
			{

				if (h==0 && k==0 && l==0)
					continue;

				scan.numor=++p;

				Eigen::Vector3d z1=ub*Eigen::Vector3d(h,k,l);

				double theta2=2.0*asin(0.5*z1.norm()*wavelength);
				double omega=theta2/2.0;
				double phi=atan2(z1[1],z1[0]);
				double chi=atan2(z1[2],sqrt(z1[0]*z1[0]+z1[1]*z1[1]));

				// Rotation of sample offset
				Eigen::Matrix3d OM;
				OM << cos(omega), sin(omega), 0,
					 -sin(omega), cos(omega), 0,
					           0,           0, 1;

				Eigen::Matrix3d CH;
				CH << cos(chi),  0, sin(chi),
							 0,  1,         0,
				     -sin(chi),  0, cos(chi);

				Eigen::Matrix3d PH;
				PH << cos(phi), sin(phi), 0,
					 -sin(phi), cos(phi), 0,
					         0,         0, 1;

				Eigen::Vector3d newpos=OM*CH*PH*sample;

				// Calculate offset on detector
				double angle=atan2(newpos[0],newpos[1]);
				// Offset along x
				scan.cx=31-(15.5+sqrt(newpos[0]*newpos[0]+newpos[1]*newpos[1])*sin(angle-theta2)/64.0*32);
				// Offset along z detector
				scan.cy=15.5+newpos[2]/64.0*32;
				std::cout << "Peak " << h << " " << k << " " << l << std::endl;
				std::cout << scan.cx << " " << scan.cy << std::endl;
				// Write angles in metadata
				scan.theta2=theta2*180.0/M_PI;
				scan.omega_center=omega*180.0/M_PI;
				scan.chi=chi*180.0/M_PI;
				scan.phi=phi*180.0/M_PI;

				// Write it
				std::ostringstream os;
				os << "Cubic/scan" << p;
				write_FakeD9(scan,os.str().c_str());
			}
		}
	}
}
