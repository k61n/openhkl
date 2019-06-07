//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/opengl/GLReciprocalLattice.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <core/CrystalTypes.h>

#include "GLActor.h"

class GLReciprocalLattice : public GLActor {

public:
    GLReciprocalLattice(const char* name);
    void setUnitCell(nsx::sptrUnitCell cell);
    void setPeriodicCells(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);
    void setSingleCell();
    int pickableElements() { return 1; }

private:
    //! Pointer to the UnitCell
    nsx::sptrUnitCell _cell;
    int _xmin, _xmax, _ymin, _ymax, _zmin, _zmax;
    //! If false draw only primitive cell,otherwise use multiple cells between
    //! xmin, xmax...
    bool _periodicCell;
    void GLCode();
};
