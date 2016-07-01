#ifndef GLACTOR_H
#define GLACTOR_H

/*
#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
*/


#include <QGLWidget>
#include <Eigen/Dense>

class GLActor
{
public:
    GLActor(const char* name);
    void draw(QGLWidget* w);
    void drawforPicking(int color);
    void drawforSelected();
    void setColor(double r, double g, double b);
    void setPos(double x,double y,double z);
    void setScale(double x,double y, double z);
    void setScale(double scale);
    virtual int pickableElements()=0;
private:
    virtual void GLCode()=0;
protected:
    GLfloat _color[3];
    Eigen::Vector3d _pos;
    double _scale[3];
    QString _name;
};

#endif // GLActor_H
