/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forshungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#define BOOST_TEST_MODULE "Test Packer class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <nsxlib/utils/Packer.h>
#include <Eigen/Dense>

using namespace SX::Utils;
using namespace std;


int run_test()
{
//    double x= 3.14;
//    Eigen::Vector3d v(1.0, 2.0, 3.0);
//    Eigen::Matrix3d m;
//    m << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9;

//    Eigen::MatrixXd blah;

//    blah << m, v;

//    Eigen::Vector3d w;
//    Eigen::Matrix3d n;

    // NOT IMPLEMENTED YET

    //blah >> n, w;

   // BOOST_CHECK_CLOSE(n(1,1), m(1,1), 1e-6);


//    Packer<double, Eigen::Vector3d, Eigen::Matrix3d> pk;
//    pk.pack(3.14, v, m);

//    pk.unpack(a, x, b, y);

//    BOOST_CHECK(a == 4);
//    BOOST_CHECK(b == -3);
//    BOOST_CHECK_CLOSE(x, 3.14, 1e-12);
//    BOOST_CHECK_CLOSE(y, 4.5f, 1e-6);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Packer)
{
    BOOST_CHECK(run_test() == 0);
}
