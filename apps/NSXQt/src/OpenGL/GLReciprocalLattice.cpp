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
        glColor3f(1,0,0);
        glVertex3d(0,0,0);
        glVertex3dv(&_ptrCell->getReciprocalAVector()[0]);
        glColor3f(0,1,0);
        glVertex3d(0,0,0);
        glVertex3dv(&_ptrCell->getReciprocalBVector()[0]);
        glColor3f(0,0,1);
        glVertex3d(0,0,0);
        glVertex3dv(&_ptrCell->getReciprocalCVector()[0]);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

}
