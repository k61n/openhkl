#ifndef NSXQT_GLRECIPROCALLATTICE_H
#define NSXQT_GLRECIPROCALLATTICE_H

#include <nsxlib/CrystalTypes.h>

#include "GLActor.h"

class GLReciprocalLattice : public GLActor
{

public:

    GLReciprocalLattice(const char* name);
    void setUnitCell(nsx::sptrUnitCell cell);
    void setPeriodicCells(int xmin,int xmax,int ymin,int ymax,int zmin,int zmax);
    void setSingleCell();
    int pickableElements() {return 1;}

private:
    //! Pointer to the UnitCell
    nsx::sptrUnitCell _cell;
    int _xmin, _xmax, _ymin, _ymax, _zmin, _zmax;
    //! If false draw only primitive cell,otherwise use multiple cells between xmin, xmax...
    bool _periodicCell;
    void GLCode();
};

#endif // NSXQT_GLRECIPROCALLATTICE_H
