#ifndef GLSCENE_H
#define GLSCENE_H

#include <QHash>
#include <QQueue>
#include <QGLWidget>
#include <Eigen/Dense>

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

#endif // GLSCENE_H
