#include "Ellipsoid.h"
#include "GeometryTypes.h"
#include "AABB.h"
#include "OBB.h"


namespace nsx {

Ellipsoid::Ellipsoid() : IShape()
{
}

Ellipsoid::Ellipsoid(const Ellipsoid& rhs) : IShape()
 {
    //_eigenVal = rhs._eigenVal;
    //_TRSinv = rhs._TRSinv;
    _center = rhs._center;
    _metric = rhs._metric;
    updateAABB();
 }

Ellipsoid& Ellipsoid::operator=(const Ellipsoid& other)
{
    if (this != &other) {
        IShape::operator=(other);
        //_eigenVal = other._eigenVal;
        //_TRSinv = other._TRSinv;
        _metric = other._metric;
        _center = other._center;
        updateAABB();
    }
    return *this;
}

IShape* Ellipsoid::clone() const
{
    return new Ellipsoid(*this);
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Vector3d& eigenvalues, const Eigen::Matrix3d& eigenvectors)
: IShape()
//,  _eigenVal(eigenvalues)
{
    // new implementation below
    Eigen::Matrix3d D = Eigen::Matrix3d::Identity();
    for (auto i = 0; i < 3; ++i) {
        D(i,i) = 1.0 / (eigenvalues[i] * eigenvalues[i]);
    }
    _metric = eigenvectors * D * eigenvectors.transpose();
    _center = center;

    updateAABB();
}

bool Ellipsoid::collide(const IShape& other) const
{
    if (this->intercept(other)) {
        return other.collide(*this);
    }
    return false;
}

bool Ellipsoid::collide(const AABB& aabb) const
{
    return collideEllipsoidAABB(*this,aabb);
}

/** Based on the method described in:
 *  "Continuous Collision Detection for Ellipsoids"
 *	Choi, Yi-King; Jung-Woo Chang; Wenping Wang; Myung-Soo Kim; Elber, G.,
 *	Visualization and Computer Graphics, IEEE Transactions on , vol.15, no.2, pp.311,325, March-April 2009
 *  However, resolving the roots of the characteristic polynomial is
 *  done here using the diagonalization of the companion matrix, rather
 *  than using the Sturm's sequence
 */

bool Ellipsoid::collide(const Ellipsoid& other) const
{ 
    const auto& A = homogeneousMatrix();
    const auto& B = other.homogeneousMatrix();
    Eigen::Matrix4d M = A.inverse() * B;

    Eigen::ComplexEigenSolver<Eigen::Matrix4d> solver(M);
    const auto& val = solver.eigenvalues();

    // One of the root is always positive.
    // Check whether two of the roots are negative and distinct, in which case the Ellipse do not collide.
    int count=0;
    double sol[2];
    if (std::fabs(imag(val(0)))< 1e-5 && real(val(0))<0) {
        sol[count++]=real(val(0));
    }
    if (std::fabs(imag(val(1)))< 1e-5 && real(val(1))<0) {
        sol[count++]=real(val(1));
    }
    if (std::fabs(imag(val(2)))< 1e-5 && real(val(2))<0) {
        sol[count++]=real(val(2));
    }
    if (std::fabs(imag(val(3)))< 1e-5 && real(val(3))<0) {
        sol[count++]=real(val(3));
    }
    return (!(count==2 && std::fabs(sol[0]-sol[1])>1e-5));
}

/** Based on the method described in:
 *  "Intersection of Box and Ellipsoid"
 *	Eberly, David.,
 *	Geometric Tools, LLC
 *	http://www.geometrictools.com
 */

bool Ellipsoid::collide(const OBB& other) const
{
    const Ellipsoid& ell = *this;
    const OBB& obb = other;

    // Get the TRS inverse matrix of the ellipsoid
    HomMatrix ellTRSinv=ell.getInverseTransformation();

    // Get the TRS inverse matrix of the OBB
    HomMatrix obbTRSinv=obb.getInverseTransformation();

    // Construct the S matrice for the ellipsoid
    Eigen::DiagonalMatrix<double,4> ellS;
    ellS.diagonal().segment(0,3) = ell.eigenvalues();
    ellS.diagonal()[3] = 1.0;

    // Construct the S matrice for the OBB
    Eigen::DiagonalMatrix<double,4> obbS;
    obbS.diagonal().segment(0,3) = obb.getExtents();
    obbS.diagonal()[3] = 1.0;

    // Construct the (TR)^-1 matrices for the ellipsoid
    HomMatrix ellTRinv(ellS*ellTRSinv);

    // Construct the (TR)^-1 matrices for the OBB
    HomMatrix obbTRinv(obbS*obbTRSinv);

    // Construct the R^-1 matrix (non-homogeneous version) for the ellipsoid
    Eigen::Matrix3d ellRinv=ellTRinv.block(0,0,3,3);

    // Construct the R^-1 matrix (non-homogeneous version) for the obb
    Eigen::Matrix3d obbRinv=obbTRinv.block(0,0,3,3);

    // Construct T vector for the ellipsoid
    Eigen::Vector3d ellT=-(ellRinv.transpose())*(ellTRinv.block(0,3,3,1));

    // Construct T vector for the OBB
    Eigen::Vector3d obbT=-(obbRinv.transpose())*(obbTRinv.block(0,3,3,1));

    // Compute the D2 and M matrices (defined in p.2 of the documentation)
    Eigen::DiagonalMatrix<double,3> D2;
    for (unsigned int i=0;i<3;++i) {
        D2.diagonal()[i] = 1.0/(ellS.diagonal()[i]*ellS.diagonal()[i]);
    }

    Eigen::Matrix3d M=(ellRinv.transpose())*D2*ellRinv;

    /*
     * Here actually starts the Minkowski sum of box and ellipsoid algorithm (defined in p.6 of the documentation)
    */

    // Compute the increase in extents for the OBB
    Eigen::Vector3d L;
    for (unsigned int i=0;i<3;++i) {
        L(i)=sqrt((obbRinv.row(i)*(M.inverse())*(obbRinv.row(i).transpose()))(0,0));
    }

    // Transform the ellipsoid center to the OBB coordinate system
    Eigen::Vector3d KmC=ellT-obbT;
    Eigen::Vector3d x=obbRinv*KmC;

    for (unsigned int i=0;i<3;++i) {
        // The ellipsoid center is outside the OBB
        if (std::abs(x(i))>(obbS.diagonal()[i]+L(i)))
            return false;
    }

    Eigen::Vector3d s;
    Eigen::Vector3d PmC = Eigen::Vector3d::Zero();
    for (unsigned int i=0; i<3;++i) {
        s(i) = (x(i) >= 0 ? 1 : -1);
        PmC.array() += s(i)*obbS.diagonal()[i]*obbRinv.row(i).array();
    }

    Eigen::Vector3d Delta = KmC-PmC;
    Eigen::Vector3d MDelta = M*Delta;
    Eigen::Vector3d rsqr;

    double r;
    for (unsigned int i=0; i<3;++i) {
        r=((ellRinv.row(i)*Delta)(0,0)/ellS.diagonal()[i]);
        rsqr(i)=r*r;
    }

    Eigen::Vector3d UMD;
    for (unsigned int i=0; i<3;++i)
        UMD(i)=(obbRinv.row(i)*MDelta)(0,0);

    Eigen::Matrix3d UMU;
    Eigen::Vector3d product;

    for (unsigned int i=0; i<3;++i) {
        product << M*(obbRinv.row(i).transpose());
        for (unsigned int j=i; j<3;++j) {
            // Need to use template here for disambiguation of the triangularView method.
            UMU.template triangularView<Eigen::Upper>().coeffRef(i,j)=(obbRinv.row(j)*product)(0,0);
        }
    }

    // K is outside the elliptical cylinder <P,U2>
    if ((s(0)*(UMD(0)*UMU(2,2)-UMD(2)*UMU(0,2)) > 0) &&
        (s(1)*(UMD(1)*UMU(2,2)-UMD(2)*UMU(1,2)) > 0) &&
        ((rsqr(0)+rsqr(1)) > 1.0)) {
        return false;
    }
    // K is outside the elliptical cylinder <P,U1>
    if ((s(0)*(UMD(0)*UMU(1,1)-UMD(1)*UMU(0,1)) > 0) &&
        (s(2)*(UMD(2)*UMU(1,1)-UMD(1)*UMU(1,2)) > 0) &&
        ((rsqr(0)+rsqr(2)) > 1.0)) {
        return false;
    }
    // K is outside the elliptical cylinder <P,U0>
    if ((s(1)*(UMD(1)*UMU(0,0)-UMD(0)*UMU(0,1)) > 0) &&
            (s(2)*(UMD(2)*UMU(0,0)-UMD(0)*UMU(0,2)) > 0) &&
            ((rsqr(1)+rsqr(2)) > 1.0)) {
        return false;
    }
    // K is outside the ellipsoid at P
    if (((s(0)*UMD(0))>0.0) && ((s(1)*UMD(1))>0.0) && ((s(2)*UMD(2))>0.0) && ((rsqr.sum())>1.0)) {
        return false;
    }
    return true;
}


bool Ellipsoid::collide(const Sphere& other) const
{
    return collideEllipsoidSphere(*this,other);
}


void Ellipsoid::rotate(const Eigen::Matrix3d& U)
{
    _metric = U * _metric * U.transpose();
    // Update the bounds of the AABB
    updateAABB();
}

void Ellipsoid::scale(double value)
{
    _metric /= std::sqrt(value);
    this->scaleAABB(value);
}

void Ellipsoid::translate(const Eigen::Vector3d& t)
{
    _center += t;    
    this->translateAABB(t);
}

bool Ellipsoid::isInside(const HomVector& point) const
{
    const double s = 1.0 / point(3);
    Eigen::Vector3d u(point(0)*s, point(1)*s, point(2)*s);
    u -= _center;
    return u.transpose() * _metric * u < 1.0;
}

const HomMatrix& Ellipsoid::getInverseTransformation() const
{
    // new implementation
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(_metric);

    auto&& eigenvalues = solver.eigenvalues();
    auto&& eigenvectors = solver.eigenvectors();

    // Define the inverse scale matrix from the eigenvalues
    Eigen::DiagonalMatrix<double,4> Sinv;
    static Eigen::Matrix4d TRSinv;
    for (unsigned int i = 0; i < 3; ++i) {
        Sinv.diagonal()[i] = std::sqrt(eigenvalues[i]);
    }
    Sinv.diagonal()[3]=1.0;

    // Now prepare the R^-1.T^-1 (rotation,translation)
    TRSinv = HomMatrix::Constant(0.0);
    TRSinv(3,3) = 1.0;

    for (unsigned int i = 0; i < 3; ++i) {
        TRSinv.block(i,0,1,3) = eigenvectors.col(i).transpose().normalized();
    }

    // The translation part of the inverse transforation matrix is afected by rotation
    // (see https://fr.wikipedia.org/wiki/Coordonn%C3%A9es_homog%C3%A8nes)
    TRSinv.block(0,3,3,1) = -TRSinv.block(0,0,3,3)*_center;

    // Finally compute (TRS)^-1 by left-multiplying (TR)^-1 by S^-1
    TRSinv = Sinv*TRSinv;

    return TRSinv;
}

void Ellipsoid::updateAABB()
{
    const auto& B = _metric.inverse();
    Eigen::Vector3d a;

    for (auto i = 0; i < 3; ++i) {
        a(i) = std::sqrt(B(i,i));
    }
    _lowerBound = _center - a;
    _upperBound = _center + a;
}

bool Ellipsoid::rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const
{
    auto&& d = dir;
    auto&& a = from;
    auto&& c = _center;
    auto&& ac = a-c;
    auto&& A = _metric;

    const double alpha = d.dot(A*d);
    const double beta = 2*d.dot(A*ac);
    const double gamma = ac.dot(A*ac)-1.0;

    const double discr = beta*beta -4*alpha*gamma;

    if (discr < 0) {
        return false;
    }

    const double delta = std::sqrt(discr);

    t1 = (-beta-delta)/(2*alpha);
    t2 = (-beta+delta)/(2*alpha);

    return !(t1 < 0 && t2 < 0);
}

Eigen::Matrix3d Ellipsoid::getRSinv() const
{
    Eigen::Matrix3d A;
    Eigen::Matrix4d TRSinv = getInverseTransformation();

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            A(i, j) = TRSinv(i, j);
        }
    }
    return A;
}

bool collideEllipsoidAABB(const Ellipsoid& ell, const AABB& aabb)
{
    OBB obb(aabb);
    return ell.collide(obb);
}




/*
 * To compute the intersection between an ellipsoid and a sphere a little trick is done.
 * It consists in building up a ellipsoid from the input sphere and just checking for
 * the intersection between two ellipsoids
 */
bool collideEllipsoidSphere(const Ellipsoid& eA, const Sphere& s)
{
    Eigen::Vector3d scale = Eigen::Vector3d::Constant(s.getRadius());
    Eigen::Matrix3d rot = Eigen::Matrix3d::Identity();
    Ellipsoid eB(s.getCenter(),scale,rot);
    return eA.collide(eB);
}

double Ellipsoid::getVolume() const
{
    static constexpr double c = 4.0*M_PI / 3.0;
    return c * std::pow(_metric.determinant(), -0.5);
}

Eigen::Matrix4d Ellipsoid::homogeneousMatrix() const
{
    Eigen::Matrix4d Q = Eigen::Matrix4d::Zero();
    Q.block<3,3>(0, 0) = _metric;
    Q.block<3,1>(0, 3) = -_metric * _center;
    Q.block<1,3>(3,0) = (-_metric * _center).transpose();
    Q(3,3) = _center.dot(_metric*_center)-1.0;
    return Q;
}

Eigen::Vector3d Ellipsoid::eigenvalues() const
{
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(_metric);
    Eigen::Vector3d vals = solver.eigenvalues();

    for (auto i = 0; i < 3; ++i) {
        vals(i) = 1.0 / std::sqrt(vals(i));
    }
    return vals;
}

} // end namespace nsx
