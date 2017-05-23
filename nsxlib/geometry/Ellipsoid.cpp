#include "Ellipsoid.h"

namespace nsx {

Ellipsoid::Ellipsoid() : IShape()
{
}

Ellipsoid::Ellipsoid(const Ellipsoid& rhs) : IShape()
 {
    _eigenVal = rhs._eigenVal;
    _TRSinv = rhs._TRSinv;
    updateAABB();
 }

Ellipsoid& Ellipsoid::operator=(const Ellipsoid& other)
{
    if (this != &other) {
        IShape::operator=(other);
        _eigenVal=other._eigenVal;
        _TRSinv=other._TRSinv;
        updateAABB();
    }
    return *this;
}

IShape* Ellipsoid::clone() const
{
    return new Ellipsoid(*this);
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Vector3d& eigenvalues, const Eigen::Matrix3d& eigenvectors)
: IShape(),
  _eigenVal(eigenvalues)
{
    // Define the inverse scale matrix from the eigenvalues
    Eigen::DiagonalMatrix<double,4> Sinv;
    for (unsigned int i = 0; i < 3; ++i) {
        Sinv.diagonal()[i] = 1.0/eigenvalues[i];
    }
    Sinv.diagonal()[3]=1.0;

    // Now prepare the R^-1.T^-1 (rotation,translation)
    _TRSinv = HomMatrix::Constant(0.0);
    _TRSinv(3,3) = 1.0;

    for (unsigned int i = 0; i < 3; ++i) {
        _TRSinv.block(i,0,1,3) = eigenvectors.col(i).transpose().normalized();
    }

    // The translation part of the inverse transforation matrix is afected by rotation
    // (see https://fr.wikipedia.org/wiki/Coordonn%C3%A9es_homog%C3%A8nes)
    _TRSinv.block(0,3,3,1) = -_TRSinv.block(0,0,3,3)*center;

    // Finally compute (TRS)^-1 by left-multiplying (TR)^-1 by S^-1
    _TRSinv = Sinv*_TRSinv;
    updateAABB();
}


Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Matrix3d& RSinv): IShape()
{
    Eigen::Vector3d t = -RSinv*center;
    _TRSinv=HomMatrix::Constant(0.0);
    _TRSinv(3,3) = 1.0;

    for (unsigned int i = 0; i < 3; ++i) {
        _TRSinv(i, 3) = t(i, 0);

        for (unsigned int j = 0; j < 3; ++j) {
            _TRSinv(i, j) = RSinv(i, j);
        }
    }
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

bool Ellipsoid::collide(const Ellipsoid& other) const
{
    return collideEllipsoidEllipsoid(*this,other);
}

bool Ellipsoid::collide(const OBB& other) const
{
    return collideEllipsoidOBB(*this,other);
}

bool Ellipsoid::collide(const Sphere& other) const
{
    return collideEllipsoidSphere(*this,other);
}

void Ellipsoid::rotate(const Eigen::Matrix3d& eigenvectors)
{
    // Reconstruct S
    Eigen::DiagonalMatrix<double,4> S;
    for (unsigned int i = 0; i < 3; ++i) {
        S.diagonal()[i] = _eigenVal[i];
    }
    S.diagonal()[3] = 1.0;
    _TRSinv=S*_TRSinv;

    // Construct the inverse of the new rotation matrix
    HomMatrix Rnewinv = HomMatrix::Zero();
    Rnewinv(3,3) = 1.0;
    for (unsigned int i = 0; i < 3; ++i) {
        Rnewinv.block(i,0,1,3)=eigenvectors.col(i).transpose().normalized();
    }
    _TRSinv=Rnewinv*_TRSinv;

    // Reconstruct Sinv
    for (unsigned int i = 0; i < 3; ++i) {
        S.diagonal()[i] = 1.0/_eigenVal[i];
    }
    S.diagonal()[3] = 1.0;

    // Reconstruct the complete TRS inverse
    _TRSinv = S*_TRSinv;

    // Update the bounds of the AABB
    updateAABB();
}

void Ellipsoid::scale(double value)
{
    _eigenVal *= value;
    Eigen::DiagonalMatrix<double,4> Sinv;
    for (unsigned int i = 0; i < 3; ++i) {
        Sinv.diagonal()[i] = 1.0/value;
    }
    Sinv.diagonal()[3] = 1.0;
    _TRSinv = Sinv*_TRSinv;
    this->scaleAABB(value);
}

void Ellipsoid::scale(const Eigen::Vector3d& v)
{
    _eigenVal = _eigenVal.cwiseProduct(v);
    Eigen::DiagonalMatrix<double,4> Sinv;
    for (unsigned int i = 0; i < 3; ++i) {
        Sinv.diagonal()[i] = 1.0/v[i];
    }
    Sinv.diagonal()[3] = 1.0;
    _TRSinv = Sinv*_TRSinv;
    this->scaleAABB(v);
}

void Ellipsoid::translate(const Eigen::Vector3d& t)
{
    HomMatrix tinv = HomMatrix::Constant(0.0);
    tinv.block(0,3,3,1) = -t;
    for (unsigned int i = 0; i < 4; ++i) {
        tinv(i,i) = 1.0;
    }
    tinv(3,3) = 1.0;
    _TRSinv = _TRSinv*tinv;
    this->translateAABB(t);
}

bool Ellipsoid::isInside(const HomVector& point) const
{
    auto&& x = _TRSinv * point;
    return (x.squaredNorm() <= 2.0);
}

const HomMatrix& Ellipsoid::getInverseTransformation() const
{
    return _TRSinv;
}

const Eigen::Vector3d& Ellipsoid::getExtents() const
{
    return _eigenVal;
}

void Ellipsoid::updateAABB()
{
    HomMatrix TRS = getInverseTransformation().inverse();

    // See https://tavianator.com/exact-bounding-boxes-for-spheres-ellipsoids/ for details about how getting
    // AABB efficiently from transformation matrix
    // The width of the AABB in one direction is the norm of corresponding TRS matrix row
    Eigen::Vector3d width = Eigen::Vector3d::Constant(0.0);
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            width[i] += TRS(i,j)*TRS(i,j);
        }
        width[i] = sqrt(width[i]);
    }
    // Update the upper and lower bound of the AABB from center +-width
    _lowerBound=TRS.block(0,3,3,1)-width;
    _upperBound=TRS.block(0,3,3,1)+width;
}

bool Ellipsoid::rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const
{
    HomVector hFrom = _TRSinv * from.homogeneous();
    HomVector hDir;
    hDir.segment(0,3) = dir;
    hDir[3] = 0.0;
    hDir = _TRSinv*hDir;
    Sphere sphere(Eigen::Vector3d::Zero(),1.0);
    return sphere.rayIntersect(hFrom.segment(0,3),hDir.segment(0,3),t1,t2);
}

Eigen::Vector3d Ellipsoid::getCenter() const
{
    Eigen::Vector3d t;
    Eigen::Matrix3d A;

    for (int i = 0; i < 3; ++i) {
        t(i,0) = _TRSinv(i, 3);

        for (int j = 0; j < 3; ++j) {
            A(i, j) = _TRSinv(i, j);
        }
    }
    t = -A.inverse()*t;
    return t;
}

Eigen::Matrix3d Ellipsoid::getRSinv() const
{
    Eigen::Matrix3d A;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            A(i, j) = _TRSinv(i, j);
        }
    }
    return A;
}

bool collideEllipsoidAABB(const Ellipsoid& ell, const AABB& aabb)
{
    OBB obb(aabb);
    return collideEllipsoidOBB(ell,obb);
}

/** Based on the method described in:
 *  "Continuous Collision Detection for Ellipsoids"
 *	Choi, Yi-King; Jung-Woo Chang; Wenping Wang; Myung-Soo Kim; Elber, G.,
 *	Visualization and Computer Graphics, IEEE Transactions on , vol.15, no.2, pp.311,325, March-April 2009
 *  However, resolving the roots of the characteristic polynomial is
 *  done here using the diagonalization of the companion matrix, rather
 *  than using the Sturm's sequence
 */

bool collideEllipsoidEllipsoid(const Ellipsoid& eA, const Ellipsoid& eB)
{
    const HomMatrix trsA = eA.getInverseTransformation();
    const Eigen::Vector3d eigA = eA.getExtents();
    const HomMatrix trsB = eB.getInverseTransformation();
    const Eigen::Vector3d eigB = eB.getExtents();

    Eigen::DiagonalMatrix<double,4> SA;
    SA.diagonal() << eigA(0), eigA(1), eigA(2), 1.0;
    Eigen::DiagonalMatrix<double,4> SB;
    SB.diagonal() << eigB(0), eigB(1), eigB(2), 1.0;
    // Recover the MA matrix
    HomMatrix MA=SA*trsA;
    MA.block(0,0,3,3).transposeInPlace();
    MA.block(0,3,3,1)=-MA.block(0,0,3,3)*MA.block(0,3,3,1);
    // Recover the MB^-1 matrix
    HomMatrix B=SB*trsB;
    // Define the characteristic matrix of B in its frame of reference.
    SA.diagonal() << 1.0/std::pow(eigA(0),2), 1.0/std::pow(eigA(1),2), 1.0/std::pow(eigA(2),2), -1.0;
    SB.diagonal() << 1.0/std::pow(eigB(0),2), 1.0/std::pow(eigB(1),2), 1.0/std::pow(eigB(2),2), -1.0;
    // Calculate the [bij] matrix (reference to publication).
    B=MA.transpose()*B.transpose()*SB*B*MA;
    //
    double ea=SA.diagonal()[0],eb=SA.diagonal()[1],ec=SA.diagonal()[2];
    double ab=ea*eb, ac=ea*ec, bc= eb*ec;
    double abc=ea*eb*ec;
    double b12s=B(0,1)*B(0,1);
    double b13s=B(0,2)*B(0,2);
    double b14s=B(0,3)*B(0,3);
    double b23s=B(1,2)*B(1,2);
    double b24s=B(1,3)*B(1,3);
    double b34s=B(2,3)*B(2,3);
    double b2233=B(1,1)*B(2,2);
    double termA=B(0,0)*bc+B(1,1)*ac+B(2,2)*ab;
    double termB=(b2233-b23s)*ea+(B(0,0)*B(2,2)-b13s)*eb+(B(0,0)*B(1,1)-b12s)*ec;
    double T4=-abc;
    double T3=termA-B(3,3)*abc;
    double T2 = termA*B(3,3)-termB-b34s*ab-b14s*bc-b24s*ac;
    double tmp1=termB*B(3,3);
    double tmp2=B(0,0)*(b2233+eb*b34s+ec*b24s-b23s);
    double tmp3=B(1,1)*(ea*b34s+ec*b14s-b13s);
    double tmp4=B(2,2)*(ea*b24s+eb*b14s-b12s);
    double tmp5=B(2,3)*(ea*B(1,2)*B(1,3)+eb*B(0,2)*B(0,3))+ B(0,1)*(ec*B(0,3)*B(1,3)-B(0,2)*B(1,2));
    tmp5+= tmp5;
    double T1=-tmp1+tmp2+tmp3+tmp4-tmp5;
    double T0 = (-B).determinant();
    // Normalize the polynomial coeffs.
    T3/=T4;T2/=T4;T1/=T4;T0/=T4;T4=1.0;
    // Solve roots of the polynomial equation
    HomMatrix companion;
    companion << 0,0,0,-T0,
                 1,0,0,-T1,
                 0,1,0,-T2,
                 0,0,1,-T3;
    // Solve the eigenvalues problem
    Eigen::ComplexEigenSolver<HomMatrix> solver;
    solver.compute(companion);

    const std::complex<double> val0 = solver.eigenvalues()(0);
    const std::complex<double> val1 = solver.eigenvalues()(1);
    const std::complex<double> val2 = solver.eigenvalues()(2);
    const std::complex<double> val3 = solver.eigenvalues()(3);

    // One of the root is always positive.
    // Check whether two of the roots are negative and distinct, in which case the Ellipse do not collide.
    int count=0;
    double sol[2];
    if (std::fabs(imag(val0))< 1e-5 && real(val0)<0) {
        sol[count++]=real(val0);
    }
    if (std::fabs(imag(val1))< 1e-5 && real(val1)<0) {
        sol[count++]=real(val1);
    }
    if (std::fabs(imag(val2))< 1e-5 && real(val2)<0) {
        sol[count++]=real(val2);
    }
    if (std::fabs(imag(val3))< 1e-5 && real(val3)<0) {
        sol[count++]=real(val3);
    }
    return (!(count==2 && std::fabs(sol[0]-sol[1])>1e-5));
}

/** Based on the method described in:
 *  "Intersection of Box and Ellipsoid"
 *	Eberly, David.,
 *	Geometric Tools, LLC
 *	http://www.geometrictools.com
 */
bool collideEllipsoidOBB(const Ellipsoid& ell, const OBB& obb)
{
    // Get the TRS inverse matrix of the ellipsoid
    HomMatrix ellTRSinv=ell.getInverseTransformation();

    // Get the TRS inverse matrix of the OBB
    HomMatrix obbTRSinv=obb.getInverseTransformation();

    // Construct the S matrice for the ellipsoid
    Eigen::DiagonalMatrix<double,4> ellS;
    ellS.diagonal().segment(0,3) = ell.getExtents();
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

/*
 * To compute the intersection between an ellipsoid and a sphere a little trick is done.
 * It consists in building up a ellipsoid from the input sphere and just checking for
 * the intersection between two ellipsoids
 */
bool collideEllipsoidSphere(const Ellipsoid& eA, const Sphere& s)
{
    Eigen::Vector3d scale = Eigen::Vector3d::Constant(s.getRadius());
    HomMatrix rot = HomMatrix::Identity();
    Ellipsoid eB(s.getCenter(),scale,rot);
    return collideEllipsoidEllipsoid(eA,eB);
}

HomMatrix Ellipsoid::getTransformation() const
{
    return _TRSinv.inverse();
}

} // end namespace nsx

