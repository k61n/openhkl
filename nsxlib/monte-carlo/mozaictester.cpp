#include "mozaictester.h"
#include <random>

#include <vector>
#include <iostream>

#include "Blob3D.h"
#include "Logger.h"

namespace {

double xor128(void) {
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

} // namespace

namespace nsx {


MozaicTester::MozaicTester()
{
}

Eigen::Vector3d MozaicTester::buildQVector(double alpha1, double ca2, double sa2, double znorm, const Eigen::Vector3d& u, const Eigen::Vector3d& v, const Eigen::Vector3d& w)
{
    double ca1 = cos(alpha1);
    double sa1 = sin(alpha1);

    Eigen::Vector3d qvect(sa1*ca2*u+sa1*sa2*v+ca1*w);

    qvect *= znorm;

    return qvect;
}

void MozaicTester::updateDetector(double alpha1,
                                  double znorm,
                                  double calpha2,
                                  double salpha2,
                                  double lambda,
                                  const Eigen::Vector3d& u,
                                  const Eigen::Vector3d& v,
                                  const Eigen::Vector3d& w,
                                  const Eigen::Vector3d& n,
                                  double px,
                                  double py,
                                  double pz,
                                  Eigen::MatrixXd& detector)
{

    Eigen::Vector3d qvect = buildQVector(alpha1,calpha2,salpha2,znorm,u,v,w);

    // kf is first initialized to ki
    Eigen::Vector3d kf=n/lambda;
    // and then incremented by qvect to provide the final kf
    kf += qvect;

    double theta=M_PI/2.0-acos(qvect.dot(-n)/znorm);

    double theta2 = 2.0*theta;
    double c2Theta = cos(theta2);
    double s2Theta = sin(theta2);
    Eigen::Matrix3d rotTheta2;
    rotTheta2 << c2Theta,-s2Theta,0.0,s2Theta,c2Theta,0.0,0,0,1;

    Eigen::Vector3d from(px,py,pz);
    from = rotTheta2*from;
    kf = rotTheta2*kf;

    double t2 = (_dDet - from[1])/kf[1];

    from += t2*kf;

    int pixz = floor((from[2] + _hDet/2.0)*_nRows/_hDet);
    int pixx = floor((from[0] + _wDet/2.0)*_nCols/_wDet);

    if (pixx > 0 && pixx < _nCols && pixz > 0 && pixz < _nRows)
        detector(pixz,pixx) += 1.0;

}

Eigen::MatrixXd MozaicTester::run1(double h, double k, double l, double px, double py, double pz)
{

    double toRad = M_PI/180.0;
    double murad = _mu*toRad;

    // Compute the z vector from [hkl]
    Eigen::Vector3d z=_ub*Eigen::Vector3d(h,k,l);
    double znorm=z.norm();
    double invznorm = 1.0/znorm;

    // Compute the theta corresponding to a brag peak for the average lambda
    double theta = asin(0.5*_l*znorm);
    double ctheta = cos(theta);
    double stheta = sin(theta);
    Eigen::Matrix3d rottheta;
    rottheta<< ctheta,stheta,0.0,-stheta,ctheta,0.0,0,0,1;

    // Create a w vector along x axis and rotate in xy plane with the Bragg angle defined above
    Eigen::Vector3d w;
    w << 1,0,0;
    w=rottheta*w;
    w.normalize();

    // Define u, v vectors such as (u,v,w) form an orthonormal basis
    Eigen::Vector3d u;
    u <<-w[1],w[0],0;
    u.normalize();
    Eigen::Vector3d v;
    v << -w[0]*w[2], -w[1]*w[2], w[0]*w[0]+w[1]*w[1];
    v.normalize();

    double dlover2 = _dl/2.0;

    int nPtsMonoX  = 10;
    int nPtsMonoY  = 10;
    int nPtsAlpha2 = 1000;

    std::vector<double> calpha2;
    std::vector<double> salpha2;
    calpha2.reserve(nPtsAlpha2);
    salpha2.reserve(nPtsAlpha2);

    double dalpha2=2.0*M_PI/nPtsAlpha2;
    for (int k=0;k<nPtsAlpha2;++k)
    {
        double alpha2=static_cast<double>(k)*dalpha2;
        calpha2.push_back(cos(alpha2));
        salpha2.push_back(sin(alpha2));
    }

    Eigen::MatrixXd detector=Eigen::MatrixXd::Zero(_nRows,_nCols);

    // Generate the points coming from the monochromator
    for (int i=0;i<=nPtsMonoX;++i)
    {
        double stepi=static_cast<double>(i)/nPtsMonoX*_wMono;
        for (int j=0;j<=nPtsMonoY;++j)
        {
            double stepj=static_cast<double>(j)/nPtsMonoY*_hMono;
            Eigen::Vector3d mp(px+_wMono/2.0-stepi,py+_dMono,pz+_hMono/2.0-stepj);
            Eigen::Vector3d n=mp.normalized();

            for (int k=0;k<nPtsAlpha2;++k)
            {

                double ca2 = calpha2[k];
                double sa2 = salpha2[k];

                // We have sin(theta) = lambda*q/2 = cos(pi/2-theta) = cos(u,Q)//||Q|| with u=MP/||MP||
                // hence
                // uzcos(alpha1) + (uxcos(alpha2)+uycos(alphap2))*sin(alpha1) = lambda*q/2
                // which is an equation of the form sin(phi)cos(alpha1) + cos(phi)sin(alpha1) = sin(omega)
                double a = -n[0]*(u[0]*ca2 + v[0]*sa2) - n[1]*(u[1]*ca2 + v[1]*sa2) - n[2]*(u[2]*ca2 + v[2]*sa2);
                double b = -n.dot(w);
                double fact = sqrt(a*a + b*b);
                double lambda=2.0*fact*invznorm;

                double lmin(_l-dlover2), lmax(-_l+dlover2);

                if (lambda<lmin)
                    continue;
                else
                {
                    if (lambda<lmax)
                        lmax = lambda;

                    double phi = atan2(b/fact,a/fact);

                    double omega;
                    double alpha1;

                    omega = asin(0.5*lmin*znorm/fact);

                    alpha1 = omega - phi;
                    if (alpha1 <= murad && alpha1 >= 0.0)
                    {
                        updateDetector(alpha1,znorm,ca2,sa2,lmin,u,v,w,n,px,py,pz,detector);
                        continue;
                    }

                    alpha1 = M_PI - omega - phi;
                    if (alpha1 <= murad && alpha1 >= 0.0)
                    {
                        updateDetector(alpha1,znorm,ca2,sa2,lmin,u,v,w,n,px,py,pz,detector);
                        continue;
                    }

                    omega = asin(0.5*lmax*znorm/fact);

                    alpha1 = omega - phi;
                    if (alpha1 <= murad && alpha1 >= 0.0)
                    {
                        updateDetector(alpha1,znorm,ca2,sa2,lmax,u,v,w,n,px,py,pz,detector);
                        continue;
                    }

                    alpha1 = M_PI - omega - phi;
                    if (alpha1 <= murad && alpha1 >= 0.0)
                    {
                        updateDetector(alpha1,znorm,ca2,sa2,lmax,u,v,w,n,px,py,pz,detector);
                        continue;
                    }

                }
            }
        }
    }

    return detector;

}

Eigen::MatrixXd MozaicTester::runCenter(double h, double k, double l, int n)
{

    double toRad = M_PI/180.0;

    std::random_device rd;
    std::ranlux48_base gen(rd());

    double murad = _mu*toRad;
    double cmurad = cos(murad);

    // Compute the z vector corresponding to [hkl]
    Eigen::Vector3d zvect=_ub*Eigen::Vector3d(h,k,l);
    double znorm = zvect.norm();
    double znorm2= znorm*znorm;
    double st=0.5*_l*znorm;
    // Check that we can have a Bragg condition for this z vector
    if (st>1.0)
        nsx::info() << "Warning': No Bragg condition possible";

    // Compute a rotation matrix along z axis of theta
    double theta = asin(0.5*_l*znorm);
    double cTheta = cos(theta);
    double sTheta = sin(theta);
    Eigen::Matrix3d rotTheta;
    rotTheta<< cTheta,sTheta,0.0,-sTheta,cTheta,0.0,0,0,1;

    // Compute a rotation matrix along z axis of 2*theta
    double theta2 = 2.0*theta;
    double c2Theta = cos(theta2);
    double s2Theta = sin(theta2);
    Eigen::Matrix3d rotTheta2;
    rotTheta2 << c2Theta,-s2Theta,0.0,s2Theta,c2Theta,0.0,0,0,1;

    Eigen::MatrixXd detector=Eigen::MatrixXd::Zero(200,200);

    // The Monte-Carlo loop
    for(int i=0; i<n; ++i)
    {
        // Generate a random point on the monochromator
        double xMono = (-0.5*+xor128())*_wMono;
        double zMono = (-0.5*+xor128())*_hMono;

        // Generate a random mosaic angle in [-mu,mu]
        double ca = cmurad+xor128()*(1.0-cmurad);
        double beta = -M_PI+xor128()*2.0*M_PI;
        double sa=sqrt(1-ca*ca);

        // Build a Q vector along X axis on which a mosaicity is applied to get it out its axis
        Eigen::Vector3d qvect(znorm*ca,znorm*sa*cos(beta),znorm*sa*sin(beta));
        // Rotate this Q vector to put it in Bragg condition
        qvect = rotTheta*qvect;

        // Generate a normalized ki vector
        Eigen::Vector3d ki(-xMono,-_dMono,-zMono);
        ki.normalize();

        // Compute the 'real' wavelength that corresponds to this Q vector on which we applied the mosaicity
        double ll=2.0*ki.dot(qvect)/znorm2;
        if (ll<_l-_dl || ll >_l+_dl)
            continue;
        ki/=ll;

        // The kf is computed as kf = ki + q
        Eigen::Vector3d kf(ki+qvect);

        double omega=acos(kf[2]/znorm);
        double phi=atan2(kf[1],kf[0]);

        double px = floor(omega*200.0/M_PI);
        double py = floor((M_PI+phi)*200.0/2.0/M_PI);
        detector(px,py) += 1.0;
    }
    return detector;
}

Eigen::MatrixXd MozaicTester::run(double h, double k, double l, int n)
{

    double toRad = M_PI/180.0;

    std::random_device rd;
    std::ranlux48_base gen(rd());

    Eigen::Vector3d zvect=_ub*Eigen::Vector3d(h,k,l);

    Eigen::VectorXd lambs(1000);
    for (int i=0;i<1000;++i)
    {
        double x=_l-3*_dl+((double)i/1000)*6*_dl;
        lambs[i]=1.0/sqrt(2.0*M_PI)/_dl*exp(-0.5*(x-_l)*(x-_l)/_dl/_dl);
    }

    double znorm = zvect.norm();
    double znorm2= znorm*znorm;
    double st=0.5*_l*znorm;
    if (st>1.0) {
        nsx::info() << "Warning: No Bragg condition possible";
    }

    double theta = asin(0.5*_l*znorm);
    double cTheta = cos(theta);
    double sTheta = sin(theta);
    Eigen::Matrix3d rotTheta;
    rotTheta<< cTheta,sTheta,0.0,-sTheta,cTheta,0.0,0,0,1;

    double theta2 = 2.0*theta;
    double c2Theta = cos(theta2);
    double s2Theta = sin(theta2);
    Eigen::Matrix3d rotTheta2;
    rotTheta2 << c2Theta,-s2Theta,0.0,s2Theta,c2Theta,0.0,0,0,1;

    Eigen::MatrixXd detector=Eigen::MatrixXd::Zero(_nRows,_nCols);
    Blob3D bb;

    double murad=cos(_mu*toRad);

    for(int i=0; i<n; ++i)
    {
        double xMono = (-0.5*+xor128())*_wMono;
        double zMono = (-0.5*+xor128())*_hMono;

        double xSample = (-1+2*xor128())*_sRadius;
        double zSample = (-1+2*xor128())*_sRadius;

        double t1, t2;
        bool test = intersect(xMono,0.0,zMono,xSample-xMono,_dMono-_sRadius,zSample-zMono,_dMono,_sRadius,t1,t2);
        if (!test)
            continue;

        double t=t1 + xor128()*(t2-t1);

        Eigen::Vector3d from(xMono + t*(xSample-xMono),t*(_dMono-_sRadius),zMono + t*(zSample-zMono));

        double alpha = murad+xor128()*(1.0-murad);
        double beta = -M_PI+xor128()*2.0*M_PI;
        double sa=sqrt(1-alpha*alpha);
        Eigen::Vector3d qvect(znorm*alpha,znorm*sa*cos(beta),znorm*sa*sin(beta));
        qvect = rotTheta*qvect;

        double sangle=-from.dot(qvect)/from.norm()/znorm2;
        double ll=2.0*sangle;
        if (ll<_l-_dl/2.0 || ll >_l+_dl/2.0)
            continue;
        Eigen::Vector3d kf;
        kf = from/(from.norm()*ll) + qvect;
        from[1]-=_dMono;

        from = rotTheta2*from;
        kf = rotTheta2*kf;

        t2 = (_dDet - from[1])/kf[1];

        from += t2*kf;

        int pz = floor((from[2]+_hDet/2.0)*_nRows/_hDet);
        int px = floor((from[0] + _wDet/2.0)*_nCols/_wDet);

        if (px > 0 && px < _nCols && pz > 0 && pz < _nRows)
        {
                bb.addPoint(pz,px, 0.0, 1);
        }

    }
    return detector;
}

Eigen::MatrixXd MozaicTester::runContour(double h, double k, double l)
{
    Eigen::MatrixXd detector=Eigen::MatrixXd::Zero(_nRows,_nCols);

    int Npoints=3;
    int nstacks=3;
    int nCones1=3;
    int nCones2=3;

    Eigen::Vector3d zvect=_ub*Eigen::Vector3d(h,k,l);
    double znorm = zvect.norm();
    double znorminv2=2.0/znorm/znorm;
    double st=0.5*_l*znorm;
    if (st>1.0)
        nsx::info() << "Warning: No Bragg condition possible";

    double theta = asin(0.5*_l*znorm);
    double cTheta = cos(theta);
    double sTheta = sin(theta);
    Eigen::Matrix3d rotTheta;
    rotTheta<< cTheta,sTheta,0.0,-sTheta,cTheta,0.0,0,0,1;

    double theta2 = 2.0*theta;
    double c2Theta = cos(theta2);
    double s2Theta = sin(theta2);
    Eigen::Matrix3d rotTheta2;
    rotTheta2 << c2Theta,-s2Theta,0.0,s2Theta,c2Theta,0.0,0,0,1;

    std::vector<Eigen::Vector3d> qvects;
    qvects.reserve(nCones1*nCones2);

    for (int c=0;c<nCones1;c++)
    {
        double step=2*M_PI*(double)c/nCones1;
        for (int c2=0;c2<=nCones2;c2++)
        {
            double step2=cos(_mu*M_PI/180.0)+(double)c2/nCones2*(1-cos(_mu*M_PI/180.0));
            Eigen::Vector3d v=rotTheta*Eigen::Vector3d(znorm*step2,znorm*sqrt(1-step2*step2)*cos(step),znorm*sqrt(1-step2*step2)*sin(step));
            qvects.push_back(v);
        }
    }
    qvects.push_back(Eigen::Vector3d(znorm,0,0));

    std::vector<Eigen::Vector3d> tos;

    for (int th=0;th<=nstacks;++th)
    {
        double theta=static_cast<double>(th)*M_PI/nstacks;

        double ctheta=cos(theta);
        double stheta=sin(theta);
        int nslices=2.0*M_PI*nstacks*stheta + 1;
        for (int ph=0;ph<nslices;++ph)
        {
            double phi=static_cast<double>(ph)*2.0*M_PI/nslices;
            double sp=sin(phi);
            double cp=cos(phi);
            Eigen::Vector3d to(_sRadius*stheta*cp,_sRadius*stheta*sp,_sRadius*ctheta);
            tos.push_back(to);
        }
    }
    double temp=0;
    double hdet=0.5*_hDet;
    double wdet=0.5*_wDet;
    std::cout << tos.size() << std::endl;
        double steph=_nRows/_hDet;
        double stepw=_nCols/_wDet;
        for (int i=0;i<=Npoints;++i)
        {
            double stepi=(double)i/Npoints*_wMono;
            for (int j=0;j<=Npoints;++j)
            {
               double stepj=(double)j/Npoints*_hMono;
                Eigen::Vector3d from(-_wMono/2.0+stepi,-_dMono,-_hMono/2.0+stepj);
                Eigen::Vector3d ki;
                for (auto& to : tos)
                {
                        ki=(to-from);
                        ki.normalize();
                        to = rotTheta2*to;
                        for (auto& qvect : qvects)
                        {
                            double ll=-ki.dot(qvect)*znorminv2;

                            if (ll>=_l-0.5*_dl && ll<=_l+0.5*_dl)
                            {
                            double kfx=rotTheta2(0,0)*(ki[0]/ll+qvect[0])+rotTheta2(0,1)*(ki[1]/ll+qvect[1]);
                            double kfy=rotTheta2(1,0)*(ki[0]/ll+qvect[0])+rotTheta2(1,1)*(ki[1]/ll+qvect[1]);
                            double kfz=(ki[2]/ll+qvect[2]);

                            double t2 = (_dDet - to[1])/kfy;

                            double x=to[0]+t2*kfx;
                            double z=to[2]+t2*kfz;
                            int pz = int((z+hdet)*steph);
                            int px = int((x+wdet)*stepw);

                            if (px >= 0 && px < _nCols && pz >= 0 && pz < _nRows)
                                detector(pz,px) += 1.0;
                            }

                        }


                }
            }
        }
    std::cout << temp;
        return detector;

}

} // namespace nsx
