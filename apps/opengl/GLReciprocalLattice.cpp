//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/opengl/GLReciprocalLattice.cpp
//! @brief     Implements class GLReciprocalLattice
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/crystal/UnitCell.h"

#include "apps/opengl/GLReciprocalLattice.h"

GLReciprocalLattice::GLReciprocalLattice(const char* name) : GLActor(name) {}

void GLReciprocalLattice::setUnitCell(nsx::sptrUnitCell cell)
{
    _cell = cell;
}

void GLReciprocalLattice::setPeriodicCells(
    int xmin, int xmax, int ymin, int ymax, int zmin, int zmax)
{
    _xmin = xmin;
    _xmax = xmax;
    _ymin = ymin;
    _ymax = ymax;
    _zmin = zmin;
    _zmax = zmax;

    _periodicCell = true;
}

void GLReciprocalLattice::setSingleCell()
{
    _xmin = 0;
    _xmax = 5;
    _ymin = 0;
    _ymax = 5;
    _zmin = 0;
    _zmax = 10;
    _periodicCell = false;
}

void GLReciprocalLattice::GLCode()
{
    if (!_cell)
        return;
    //
    glDisable(GL_LIGHTING);
    glLineWidth(2.5);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_LINES);

    auto aStarVector = _cell->basis().row(0);
    auto bStarVector = _cell->basis().row(1);
    auto cStarVector = _cell->basis().row(2);

    double ax = aStarVector[0];
    double ay = aStarVector[1];
    double az = aStarVector[2];

    double bx = bStarVector[0];
    double by = bStarVector[1];
    double bz = bStarVector[2];

    double cx = cStarVector[0];
    double cy = cStarVector[1];
    double cz = cStarVector[2];

    for (int h = _xmin; h < _xmax; ++h) {
        double hd = static_cast<double>(h);
        for (int k = _ymin; k < _ymax; ++k) {
            double kd = static_cast<double>(k);
            for (int l = _zmin; l < _zmax; ++l) {
                double ld = static_cast<double>(l);

                double ox = hd * ax + kd * bx + ld * cx;
                double oy = hd * ay + kd * by + ld * cy;
                double oz = hd * az + kd * bz + ld * cz;

                glColor3f(1, 0, 0);
                glVertex3d(ox, oy, oz);
                glVertex3d(ox + ax, oy + ay, oz + az);

                glColor3f(0, 1, 0);
                glVertex3d(ox, oy, oz);
                glVertex3d(ox + bx, oy + by, oz + bz);

                glColor3f(0, 0, 1);
                glVertex3d(ox, oy, oz);
                glVertex3d(ox + cx, oy + cy, oz + cz);

                glColor3f(0, 1, 0);
                glVertex3d(ox + ax, oy + ay, oz + az);
                glVertex3d(ox + ax + bx, oy + ay + by, oz + az + bz);

                glColor3f(1, 0, 0);
                glVertex3d(ox + bx, oy + by, oz + bz);
                glVertex3d(ox + ax + bx, oy + ay + by, oz + az + bz);

                glColor3f(0, 0, 1);
                glVertex3d(ox + ax, oy + ay, oz + az);
                glVertex3d(ox + ax + cx, oy + ay + cy, oz + az + cz);

                glColor3f(1, 0, 0);
                glVertex3d(ox + cx, oy + cy, oz + cz);
                glVertex3d(ox + ax + cx, oy + ay + cy, oz + az + cz);

                glColor3f(0, 0, 1);
                glVertex3d(ox + bx, oy + by, oz + bz);
                glVertex3d(ox + bx + cx, oy + by + cy, oz + bz + cz);

                glColor3f(0, 1, 0);
                glVertex3d(ox + cx, oy + cy, oz + cz);
                glVertex3d(ox + bx + cx, oy + by + cy, oz + bz + cz);

                glColor3f(0, 0, 1);
                glVertex3d(ox + ax + bx, oy + ay + by, oz + az + bz);
                glVertex3d(ox + ax + bx + cx, oy + ay + by + cy, oz + az + bz + cz);

                glColor3f(0, 1, 0);
                glVertex3d(ox + ax + cx, oy + ay + cy, oz + az + cz);
                glVertex3d(ox + ax + bx + cx, oy + ay + by + cy, oz + az + bz + cz);

                glColor3f(1, 0, 0);
                glVertex3d(ox + bx + cx, oy + by + cy, oz + bz + cz);
                glVertex3d(ox + ax + bx + cx, oy + ay + by + cy, oz + az + bz + cz);
            }
        }
    }
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}
