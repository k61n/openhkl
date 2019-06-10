//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/opengl/GLWidget.h
//! @brief     Defines class GLWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QGLWidget>

#include "apps/opengl/GLScene.h"
#include "apps/opengl/GLTrackball.h"

class GLWidget : public QGLWidget {
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = 0);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);
    void setPicking(bool);
    void setContextMenu();
    GLScene& getScene();
signals:

public slots:
public:
    double _viewport_transX, _viewport_transY;
    double _zoom;
    GLTrackball ball;

private:
    bool _picking;
    GLScene _scene;
    GLuint _list;
};
