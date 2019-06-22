//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/opengl/GLScene.h
//! @brief     Defines class GLScene
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <Eigen/Dense>

#include <QGLWidget>
#include <QHash>
#include <QQueue>

class GLActor;

class GLScene {
 public:
    GLScene();
    ~GLScene();
    GLActor* addActor(GLActor*);
    void deleteActor(int);
    GLActor* pickObjet(int);
    void render(QGLWidget*);
    void renderForPicking();
    GLActor* picking(int r, int g, int b);

 private:
    QHash<int, GLActor*> _actors;
    int _current;
    int _selected;
    QQueue<int> _numbers;
    Eigen::Vector3d _bbmin, _bbmax;
};
