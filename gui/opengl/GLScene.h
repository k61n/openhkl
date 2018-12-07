#pragma once

#include <Eigen/Dense>

#include <QGLWidget>
#include <QHash>
#include <QQueue>

class GLActor;

class GLScene
{
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
    QHash<int,GLActor*> _actors;
    int _current;
    int _selected;
    QQueue<int> _numbers;
    Eigen::Vector3d _bbmin,_bbmax;
};
