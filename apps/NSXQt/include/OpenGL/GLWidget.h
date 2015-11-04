#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "GLTrackball.h"
#include "GLScene.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void setPicking(bool);
    void setContextMenu();
    GLScene& getScene();
signals:

public slots:
public:
    double _viewport_transX,_viewport_transY;
    double _zoom;
    GLTrackball ball;
private:
    bool _picking;
    GLScene _scene;
    GLuint _list;
};

#endif // GLWIDGET_H
