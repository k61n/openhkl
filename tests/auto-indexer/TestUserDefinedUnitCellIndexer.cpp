#include <random>
#include <vector>

#include <nsxlib/UserDefinedUnitCellIndexer.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

NSX_INIT_TEST

Eigen::Matrix3d make_rotation(Eigen::Vector3d angles)
{
    const double com = std::cos(angles(0));
    const double som = std::sin(angles(0));

    Eigen::Matrix3d om;
    om << com,-som,0.0,
          som, com,0.0,
          0.0, 0.0,1.0;

    const double cch = std::cos(angles(1));
    const double sch = std::sin(angles(1));
    Eigen::Matrix3d ch;
    ch << cch,0.0,-sch,
          0.0,1.0, 0.0,
          sch,0.0, cch;

    const double cph = std::cos(angles(2));
    const double sph = std::sin(angles(2));
    Eigen::Matrix3d ph;
    ph << cph,-sph,0.0,
          sph, cph,0.0,
          0.0, 0.0,1.0;

    return om*ch*ph;
}

int main()
{
//    const double a =  5.557;
//    const double b =  5.770;
//    const double c = 16.138;

    const double a = 10.0;
    const double b = 10.0;
    const double c = 10.0;

//    const double alpha = 96.314*nsx::deg;
//    const double beta  = 90.000*nsx::deg;
//    const double gamma = 90.000*nsx::deg;

    const double alpha = 45.000*nsx::deg;
    const double beta  = 90.000*nsx::deg;
    const double gamma = 90.000*nsx::deg;

    const double wavelength = 1.0;

    nsx::UnitCell uc(a,b,c,alpha,beta,gamma);

    std::cout<<"ddd"<<std::endl;
    std::cout<<uc.reciprocalBasis()<<std::endl;

    const double dmin =  0.95;
    const double dmax = 1.00;

    auto hkls = uc.generateReflectionsInShell(dmin,dmax,wavelength);

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-0.00000,0.00000);

//    const double omega = 26.0*nsx::deg;
//    const double chi   = 32.0*nsx::deg;
//    const double phi   = 11.0*nsx::deg;

    const double omega = 0.0*nsx::deg;
    const double chi   = 0.0*nsx::deg;
    const double phi   = 0.0*nsx::deg;

    Eigen::Matrix3d u_matrix = make_rotation({omega,chi,phi});

    std::cout<<"TRUE matrix" << std::endl;
    std::cout<<u_matrix<<std::endl;

    std::vector<nsx::ReciprocalVector> q_vectors;
    q_vectors.reserve(hkls.size());

    std::cout<<uc.reciprocalBasis()<<std::endl;

    for (auto hkl : hkls) {

        const double qx_err = distribution(generator);
        const double qy_err = distribution(generator);
        const double qz_err = distribution(generator);

        Eigen::RowVector3d q = hkl.rowVector().cast<double>() * uc.reciprocalBasis() * u_matrix.transpose();

        q(0) += qx_err;
        q(1) += qy_err;
        q(2) += qz_err;

        q_vectors.push_back(nsx::ReciprocalVector(q));
    }

    auto indexer = nsx::UserDefinedUnitCellIndexer(a,b,c,alpha,beta,gamma,1);

    std::cout<<q_vectors.size()<<std::endl;

    indexer.run(q_vectors,wavelength);

    return 0;
}
