#ifndef GLACTOR_H
#define GLACTOR_H
#include <GL/gl.h>
#include <eigen3/Eigen/Dense>
#include <QGLWidget>

class GLActor
{
public:
    GLActor(const char* name);
    void draw(QGLWidget* w);
    void drawforPicking(int color);
    void drawforSelected();
    void setColor(double r, double g, double b);
    void setPos(double x,double y,double z);
    virtual int pickableElements()=0;
private:
    virtual void GLCode()=0;
    GLfloat _color[3];
    Eigen::Vector3d _pos;
    QString _name;
};

#endif // GLActor_H
