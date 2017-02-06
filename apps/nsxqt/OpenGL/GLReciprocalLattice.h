#ifndef GLRECIPROCALLATTICE_H
#define GLRECIPROCALLATTICE_H

#include "GLActor.h"
#include <nsxlib/crystal/UnitCell.h>
#include <memory>

class GLReciprocalLattice : public GLActor
{
public:
    GLReciprocalLattice(const char* name);
    void setUnitCell(std::shared_ptr<SX::Crystal::UnitCell> pUnitCell);
    void setPeriodicCells(int xmin,int xmax,int ymin,int ymax,int zmin,int zmax);
    void setSingleCell();
    int pickableElements() {return 1;}
private:
    //! Pointer to the UnitCell
    std::shared_ptr<SX::Crystal::UnitCell> _ptrCell;
    int _xmin, _xmax, _ymin, _ymax, _zmin, _zmax;
    //! If false draw only primitive cell,otherwise use multiple cells between xmin, xmax...
    bool _periodicCell;
    void GLCode();
};

#endif // GLRECIPROCALLATTICE_H
