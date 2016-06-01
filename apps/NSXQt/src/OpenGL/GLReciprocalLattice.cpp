#include "include/OpenGL/GLReciprocalLattice.h"

GLReciprocalLattice::GLReciprocalLattice(const char* name):GLActor(name)
{

}

void GLReciprocalLattice::setUnitCell(std::shared_ptr<SX::Crystal::UnitCell> pUnitCell)
{
    _ptrCell=pUnitCell;
}

void GLReciprocalLattice::setPeriodicCells(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax)
{
    _xmin=xmin;
    _xmax=xmax;
    _ymin=ymin;
    _ymax=ymax;
    _zmin=zmin;
    _zmax=zmax;

    _periodicCell = true;
}

void GLReciprocalLattice::setSingleCell()
{
    _periodicCell=false;
}

void GLReciprocalLattice::GLCode()
{
    if (!_ptrCell)
        return;
    //
    glDisable(GL_LIGHTING);
    glLineWidth(2.5);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_LINES);

        auto aStarVector = _ptrCell->getReciprocalAVector();
        auto bStarVector = _ptrCell->getReciprocalBVector();
        auto cStarVector = _ptrCell->getReciprocalCVector();

        double ax = aStarVector[0];
        double ay = aStarVector[1];
        double az = aStarVector[2];

        double bx = bStarVector[0];
        double by = bStarVector[1];
        double bz = bStarVector[2];

        double cx = cStarVector[0];
        double cy = cStarVector[1];
        double cz = cStarVector[2];

        glColor3f(1,0,0);
        glVertex3d(0,0,0);
        glVertex3d(ax,ay,az);

        glColor3f(0,1,0);
        glVertex3d(0,0,0);
        glVertex3d(bx,by,bz);

        glColor3f(0,0,1);
        glVertex3d(0,0,0);
        glVertex3d(cx,cy,cz);

        glColor3f(0,0,0);
        glVertex3d(ax,ay,az);
        glVertex3d(ax+bx,ay+by,az+bz);

        glColor3f(0,0,0);
        glVertex3d(bx,by,bz);
        glVertex3d(ax+bx,ay+by,az+bz);

        glColor3f(0,0,0);
        glVertex3d(ax,ay,az);
        glVertex3d(ax+cx,ay+cy,az+cz);

        glColor3f(0,0,0);
        glVertex3d(cx,cy,cz);
        glVertex3d(ax+cx,ay+cy,az+cz);

        glColor3f(0,0,0);
        glVertex3d(bx,by,bz);
        glVertex3d(bx+cx,by+cy,bz+cz);

        glColor3f(0,0,0);
        glVertex3d(cx,cy,cz);
        glVertex3d(bx+cx,by+cy,bz+cz);

        glColor3f(0,0,0);
        glVertex3d(ax+bx,ay+by,az+bz);
        glVertex3d(ax+bx+cx,ay+by+cy,az+bz+cz);

        glColor3f(0,0,0);
        glVertex3d(ax+cx,ay+cy,az+cz);
        glVertex3d(ax+bx+cx,ay+by+cy,az+bz+cz);

        glColor3f(0,0,0);
        glVertex3d(bx+cx,by+cy,bz+cz);
        glVertex3d(ax+bx+cx,ay+by+cy,az+bz+cz);


    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

}
