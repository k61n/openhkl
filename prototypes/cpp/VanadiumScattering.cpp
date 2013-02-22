#include <boost/random.hpp> 
#include <ctime> 
#include <cmath>
#include "V3D.h" 
#include <vector> 
#include <boost/numeric/ublas/matrix.hpp> 

using namespace SX;

// x along beam, y towards 90 deg detector, z-up (left-handed system)

// Matrix of points
typedef boost::numeric::ublas::matrix<V3D> mpoints; 
const double deg2rad=M_PI/180.0;


struct CylDetector
{
    CylDetector(double Radius,double h, double th2min, double th2max, int hor_pix, int vert_pix)
    :_radius(Radius),_h(h),
    _th2min(th2min*deg2rad),_th2max(th2max*deg2rad),
    _hpix(hor_pix),_vpix(vert_pix),
    _normals(hor_pix),
    _tangents(hor_pix),
    _targets(vert_pix,hor_pix)
    {
        // Delta theta of a pixel
        double dtheta=(_th2max-_th2min)/_hpix;
        // Width of a pixel
        _hh=2.0*_radius*sin(0.5*dtheta);
        // Height of a pixel
        _hv=_h/_vpix;
        // Surface
        _pixSurface=_hh*_hv;
        //Store normals and positions of pixels
        for(int i=0;i<_hpix;++i)
        {
            double gam=_th2min+(i+0.5)*dtheta;
            double cgam=cos(gam);
            double sgam=sin(gam);
            _normals[i]=V3D(cgam,sgam,0);
            double th=_th2min+i*dtheta;
            double thn=th+dtheta;
            _tangents[i]=V3D(_radius*(cos(thn)-cos(th)),_radius*(sin(thn)-sin(th)),0.0);
            
            for (int j=0;j<_vpix;++j)
            {
                double hei=-0.5*h+j*h/vert_pix;
                _targets(j,i)=V3D(_radius*cgam,_radius*sgam,hei);
            }
        }
    }
    // Return the position of the j, i pixel
    inline void randomInPixel(int j, int i, double w01, double h01,const V3D& fromP, double& SA,V3D& t)
    {
        // Choose a random point in the pixel
        t=_targets(j,i);
        t[0]+=w01*(_tangents[i])[0];
        t[1]+=w01*(_tangents[i])[1];
        t[2]+=h01*_hv;
        t-=fromP;
        double d=t.normalize();
        SA=_pixSurface*t.scalar_prod(_normals[i])/(d*d);        
    }
    mpoints _targets; // Positions of pixels
    std::vector<V3D> _normals; //Pixel normals, depends only on Gamma
    std::vector<V3D> _tangents; // Pixel tangents, depends only on Gamma
    double _radius; // cm
    double _h;      // cm
    double _th2min; // radians
    double _th2max; // radians
    int _hpix, _vpix; //
    double _hh, _hv;
    double _pixSurface; //Surface of a pixel
};

int main(int narg,char* argc[])
{
   
    if (narg!=2)
        exit(1);
    // Number of neutrons per pixel
    const int N=1e6;
    // Wavelength
    const double wave=1.46;
    // Cross sections (barns) and number density
    const double abs=5.08/1.798*wave, inc=5.08, ndens=2.0/std::pow(3.03,3);
    double scat=ndens*inc;
    double atte=ndens*(abs+inc);
    // D19 detector
    CylDetector D19(76,40,7.49,127.49,640,256);
    // Vanadium diameter (cm) and height (cm)
    double van_d=0.8, van_h=1.0;
    double R=0.5*van_d;
    
    // Boost generator
    boost::lagged_fibonacci19937 mlagged(static_cast<uint32_t>(time(0)));
	boost::uniform_01<> unif01;
	boost::variate_generator<boost::lagged_fibonacci19937&,boost::uniform_01<> > distrib01(mlagged,unif01);
	// 
	int xs=atoi(argc[1]);
	double lf,l1,l2;
	double A,B,C;
	double solid_angle;
	std::vector<double> result(32);
	V3D P,V;
    double diaph=1.0; // Circular diaphragm of diameter 1.0 cm
    
    for (int y=0;y<256;y+=8)
    {   
        double& intens=result[y/8];
        for (int i=0;i<N;++i)
        {
           // y and z coordinates of the point of intersection
           P[1]=(-0.5+distrib01());
           P[2]=(-0.5+distrib01());
           if ((P[1]*P[1]+P[2]*P[2])>1) // Not inside circular opening
             continue;
           P[1]*=diaph;
           P[2]*=diaph;
           if (std::fabs(P[1])>R) // Not within the cylinder width
             continue;
           // x coordinate of the point of intersection
           P[0]=-sqrt(R*R-P[1]*P[1]);
           // Primary flight path
           lf=-2*P[0];
           l1=lf*distrib01();
           // x coordinate after l1
           P[0]+=l1;
           double r1=distrib01();
           double r2=distrib01();
           D19.randomInPixel(y,xs,r1,r2,P,solid_angle,V);
           A=V[0]*V[0]+V[1]*V[1];
           B=2.0*(P[0]*V[0]+P[1]*V[1]);
           C=P[0]*P[0]+P[1]*P[1]-R*R;
           l2=0.5*(-B+sqrt(B*B-4*A*C))/A;
           //double theta=0.5*acos(V[0]);
           //double Q=4*M_PI*sin(theta)/wave;
           //double DW=exp(-0.0072*Q*Q);
           intens+=solid_angle*lf*scat*exp(-atte*(l1+l2));   
        }                     
        intens/=(4.0*M_PI*N);
    }
    
    // Print-out the result along this strip
    int i=0;
    std::for_each(result.begin(),result.end(),[&i](double a) { std::cout << i << " " << a << std::endl;i+=8;});
    
}


