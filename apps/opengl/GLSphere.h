//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/opengl/GLSphere.h
//! @brief     Defines class GLSphere
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "apps/opengl/GLActor.h"

class GLSphere : public GLActor {
public:
    GLSphere(const char* name);
    int pickableElements() { return 1; }

private:
    virtual void GLCode();
};
