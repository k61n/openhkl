//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/opengl/GLWidget.cpp
//! @brief     Implements class GLWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QMouseEvent>
#include <QtOpenGL>

#include "apps/opengl/GLSphere.h"
#include "apps/opengl/GLWidget.h"

/*
#ifdef __APPLE__
        #include <OpenGL/gl.h>
#else
        #include <GL/gl.h>
#endif
*/

GLWidget::GLWidget(QWidget* parent) : QGLWidget(parent), _zoom(1.0), ball(0, 0), _picking(false) {}

void GLWidget::initializeGL()
{
    setCursor(Qt::CrossCursor);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    GLfloat lamp_ambient[4] = {0.40f, 0.0f, 1.0f, 0.0f};
    GLfloat lamp_diffuse[4] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat lamp_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lamp_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lamp_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lamp_specular);
    GLfloat lamp_pos[4] = {5.0, 5.0, 5.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lamp_pos);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        -100.0 * _zoom, 100.0 * _zoom, -100.0 * _zoom, 100.0 * _zoom, -100.0 * _zoom,
        100.0 * _zoom);
    glMatrixMode(GL_MODELVIEW);
    ball.setViewport(w, h);
    updateGL();
}

void GLWidget::paintGL()
{
    glMatrixMode(GL_MODELVIEW);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_FRONT_AND_BACK);

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    ball.IssueRotation();

    _scene.render(this);
    glPopMatrix();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    if (_picking) {
        setPicking(true);
        GLubyte color[4];
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glReadPixels(event->x(), viewport[3] - event->y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
        int r = color[0];
        int g = color[1];
        int b = color[2];
        GLActor* o = _scene.picking(r, g, b);
        if (o)
            updateGL();
    } else
        ball.initRotationFrom(event->x(), event->y());
}
void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!_picking) {
        ball.generateRotationTo(event->x(), event->y());
        updateGL();
        ball.initRotationFrom(event->x(), event->y());
    } else {
        setPicking(true);
        GLubyte color[4];
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glReadPixels(event->x(), viewport[3] - event->y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
        int r = color[0];
        int g = color[1];
        int b = color[2];
        GLActor* o = _scene.picking(r, g, b);
        if (o)
            updateGL();
    }
}

void GLWidget::setPicking(bool arg)
{
    _picking = arg;
    if (arg) {
        glDrawBuffer(GL_BACK);
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0, 0, 0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glPushMatrix();
        ball.IssueRotation();
        glDisable(GL_LIGHTING);
        _scene.renderForPicking();

        glPopMatrix();
        glFlush();

        glPopAttrib();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    double step = event->delta() / 120.0;
    double zoomfactor = (1.0 + step * 0.05);

    _zoom *= zoomfactor;
    resizeGL(width(), height());
}

GLScene& GLWidget::getScene()
{
    return _scene;
}
